// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"

#include "sdkconfig.h"
#include "ST7789.h"
#include "system_configuration.h"


//Warning: This gets squeezed into IRAM.
static uint32_t *currFbPtr=NULL;

SemaphoreHandle_t dispSem = NULL;
SemaphoreHandle_t dispDoneSem = NULL;

#define NO_SIM_TRANS 5 //Amount of SPI transfers to queue in parallel
#define MEM_PER_TRANS 240*2 //in 16-bit words

extern int16_t lcdpal[256];

st7789_driver_t display = {
		.pin_reset = HSPI_RST,
		.pin_dc = HSPI_DC,
		.pin_mosi = HSPI_MOSI,
		.pin_sclk = HSPI_CLK,
		.spi_host = HSPI_HOST,
		.dma_chan = 1,
		.display_width = 240,
		.display_height = 240,
		.buffer_size = 20 * 240, // 2 buffers with 20 lines
	};

void IRAM_ATTR displayTask(void *arg) {
	int x, i;
	int idx=0;
	static uint16_t *dmamem[NO_SIM_TRANS];


    ST7789_init(&display);
    ST7789_set_endian(&display);
	//We're going to do a fair few transfers in parallel. Set them all up.
	for (x=0; x<NO_SIM_TRANS; x++) {

        dmamem[x]=heap_caps_malloc(MEM_PER_TRANS*2, MALLOC_CAP_DMA);
		assert(dmamem[x]);

	}
	xSemaphoreGive(dispDoneSem);

	while(1) {
		xSemaphoreTake(dispSem, portMAX_DELAY);
        ST7789_set_window(&display,0,0,240,240);
		for (x=0; x<240*240; x+=MEM_PER_TRANS) {

			for (i=0; i<MEM_PER_TRANS; i+=4) {
				uint32_t d=currFbPtr[(x+i)/4];
				dmamem[idx][i+0]=lcdpal[(d>>0)&0xff];
				dmamem[idx][i+1]=lcdpal[(d>>8)&0xff];
				dmamem[idx][i+2]=lcdpal[(d>>16)&0xff];
				dmamem[idx][i+3]=lcdpal[(d>>24)&0xff];
			}


            display.current_buffer = dmamem[idx];
            display.buffer_size = MEM_PER_TRANS*16;
            ST7789_swap_buffers(&display);

			idx++;
			if (idx>=NO_SIM_TRANS) idx=0;

		}

	}
}


void spi_lcd_send(uint16_t *scr) {

	memcpy(currFbPtr, scr, 240*240);

	xSemaphoreGive(dispSem);
}

void spi_lcd_init() {
	printf("spi_lcd_init()\n");
    dispSem=xSemaphoreCreateBinary();
    dispDoneSem=xSemaphoreCreateBinary();

    currFbPtr=heap_caps_malloc(240*240, MALLOC_CAP_32BIT);

	xTaskCreatePinnedToCore(&displayTask, "display", 6000, NULL, 6, NULL, 1);

}

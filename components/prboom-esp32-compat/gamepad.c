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


#include <stdlib.h>

#include "doomdef.h"
#include "doomtype.h"
#include "m_argv.h"
#include "d_event.h"
#include "g_game.h"
#include "d_main.h"
#include "gamepad.h"
#include "lprintf.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "TCA9555.h"

//The gamepad uses keyboard emulation, but for compilation, these variables need to be placed
//somewhere. This is as good a place as any.
int usejoystick=0;
int joyleft, joyright, joyup, joydown;
event_t ev;

//atomic, for communication between joy thread and main game thread
volatile int joyVal=0;

uint32_t button_time[]

void get_gamepad_status(){
	uint16_t input = TCA9555_readInputs();
	uint32_t actual_time= xTaskGetTickCount()/portTICK_PERIOD_MS;

	if(input != 0xfff){

		//Down Button
		if(!((input >>0) & 0x01)){
			ev.data1=key_down;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_down;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Left Button
		if(!((input >>1) & 0x01)){
			ev.data1=key_left;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_left;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Up button
		if(!((input >>2) & 0x01)){
			ev.data1=key_up;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_up;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Right button
		if(!((input >>3) & 0x01)){
			ev.data1=key_right;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_right;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//A button -> fire and menu enter
		if(!((input >>8) & 0x01)){
			ev.data1=key_use;
			ev.type=ev_keydown;
			D_PostEvent(&ev);

			ev.data1=key_menu_enter;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_use;
			ev.type=ev_keyup;
			D_PostEvent(&ev);

			ev.data1=key_menu_enter;
			D_PostEvent(&ev);
		}

		//B button -> Action
		if(!((input >>9) & 0x01)){
			ev.data1=key_fire;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_fire;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//X button -> toggle weapon
		if(!((input >>6) & 0x01)){
			ev.data1=key_pause;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_pause;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Y button -> toggle weapon
		if(!((input >>7) & 0x01)){
			ev.data1=key_weapontoggle;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_weapontoggle;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//R button -> straferight
		if(!((input >>5) & 0x01)){
			ev.data1=key_straferight;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_straferight;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//R button -> strafeleft
		if(!((input >>13) & 0x01)){
			ev.data1=key_strafeleft;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_strafeleft;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Start button  -> escape
		if(!((input >>10) & 0x01)){
			ev.data1=key_escape;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_escape;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Select button  -> map
		if(!((input >>12) & 0x01)){
			ev.data1=key_map;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_map;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

		//Menu button  -> quick_save
		if(!((input >>11) & 0x01)){
			ev.data1=key_quicksave;
			ev.type=ev_keydown;
			D_PostEvent(&ev);
		}
		else{
			ev.data1=key_quicksave;
			ev.type=ev_keyup;
			D_PostEvent(&ev);
		}

	}
}



void init_gamepad() {
	TCA955_init();
}


#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := esp32-doom


EXTRA_COMPONENT_DIRS := components/prboom \
						components/prboom-esp32-compat \
						components/drivers/ST7789 \
						components/drivers/SYSTEM_CONFIG \
						components/drivers/INPUT \

include $(IDF_PATH)/make/project.mk


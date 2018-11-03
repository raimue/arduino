BOARD_TAG ?= generic

ARDUINO_DIR = $(realpath ../..)
ARDMK_DIR ?= $(realpath ../../tools/arduino-mk-esp)
ARDMK_FILE ?= $(ARDMK_DIR)/ArduinoESP.mk

ESP_UPLOAD_SPEED = 921600

include ../arduino-config.mk

BOARD_TAG ?= pro5v328
OBJDIR    ?= .build/$(BOARD_TAG)

MONITOR_CMD ?= picocom

ARDMK_DIR ?= $(realpath ../../tools/arduino-mk)
ARDMK_FILE ?= $(ARDMK_DIR)/Arduino.mk

ifneq ($(wildcard /opt/arduino),)
ARDUINO_DIR = $(realpath /opt/arduino)
endif

# Helpful aliases
.PHONY: default serial flash
default: all
serial: monitor
flash: upload

include $(ARDMK_FILE)

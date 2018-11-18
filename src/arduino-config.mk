BOARD_TAG ?= pro5v328
OBJDIR    ?= .build/$(BOARD_TAG)

MONITOR_CMD ?= screen -c /dev/null

ARDMK_DIR ?= $(realpath ../../tools/arduino-mk)
ARDMK_FILE ?= $(ARDMK_DIR)/Arduino.mk

# Helpful aliases
.PHONY: default serial flash
default: all
serial: monitor
flash: upload

include $(ARDMK_FILE)

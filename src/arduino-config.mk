BOARD_TAG = pro5v328
OBJDIR    = .build/$(BOARD_TAG)

ifeq ($(shell uname -s),Darwin)
MONITOR_PORT = /dev/cu.usb*
else
MONITOR_PORT = /dev/ttyACM*
endif
MONITOR_CMD = screen -c /dev/null

ARDMK_DIR = $(realpath ../../tools/arduino-mk)

# Helpful aliases
.PHONY: default serial flash
default: all
serial: monitor
flash: upload

BOARD_TAG = pro5v328
OBJDIR    = .build/$(BOARD_TAG)

MONITOR_PORT = /dev/cu.usb*
MONITOR_CMD = screen -c /dev/null

ARDMK_DIR = $(realpath ../../tools/arduino-mk)

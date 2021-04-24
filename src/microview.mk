BOARDS_TXT = ../../hardware/boards.txt
BOARD_TAG = uview
ARDUINO_LIBS += MicroView

ifneq ($(wildcard /dev/ttyUSBMicroView),)
MONITOR_PORT = /dev/ttyUSBMicroView
endif

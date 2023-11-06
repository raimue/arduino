BOARD_FQDN = esp8266:esp8266:generic:baud=921600
MONITOR_PORT ?= /dev/ttyUSB1
MONITOR_BAUDRATE ?= 115200

include ../arduino-cli.mk

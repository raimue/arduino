## Variables

ARDUINO_CLI ?= arduino-cli
BUILD_PATH ?= $(PWD)/.build

## Local options

ifneq ($(V),)
VERBOSE = -v
else
VERBOSE =
endif

## Makefile options

ifeq ($(BOARD_FQDN),)
$(error missing BOARD_FQDN)
endif

ifeq ($(SERIAL_PORT),)
$(error missing SERIAL_PORT)
endif

## Build rules

.PHONY: all
all: compile

.PHONY: compile
compile:
	$(ARDUINO_CLI) compile $(VERBOSE) -b $(BOARD_FQDN) --build-path $(BUILD_PATH)

.PHONY: upload
upload:
	$(ARDUINO_CLI) upload $(VERBOSE) -b $(BOARD_FQDN) -p $(SERIAL_PORT) --input-dir $(BUILD_PATH)

.PHONY: clean
clean:
	rm -rf $(BUILD_PATH)

## Aliases

.PHONY: serial
serial: monitor

.PHONY: flash
flash: upload

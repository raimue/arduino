## Variables

ARDUINO_CLI ?= arduino-cli
MONITOR_CMD ?= picocom
BUILD_PATH ?= $(PWD)/.build

## Local options

ifneq ($(V),)
VERBOSE = -v
else
VERBOSE =
endif

## Helpers

empty :=
space := $(empty) $(empty)
define newline

$(empty)
endef

## Makefile options

ifeq ($(BOARD_FQDN),)
$(error missing BOARD_FQDN)
endif

ifeq ($(MONITOR_PORT),)
$(error missing MONITOR_PORT)
endif

BUILD_DEFINES := MONITOR_BAUDRATE WIFI_SSID WIFI_PASSWORD
BUILD_DEFINES_H = $(BUILD_PATH)/defines.h
BUILD_FLAGS=--build-property "build.extra_flags=-include $(BUILD_DEFINES_H)"

## Build rules

.PHONY: all
all: compile

.PHONY: preprocess
preprocess: $(BUILD_DEFINES_H)

$(BUILD_DEFINES_H): Makefile $(wildcard *.mk) $(MAKEFILE_LIST)
	@mkdir -p $(BUILD_PATH)
	@echo -n > $@
	@for line in $(foreach def,$(BUILD_DEFINES),\
	               $(if $(strip $($(def))),\
	                 $(if $(filter %_BAUDRATE,$(def)),\
	                   "#define $(def) $($(def))",\
	                   "#define $(def) \"$($(def))\"")\
	                )$\
	              ); do \
		echo "$$line" >> $@; \
	done

.PHONY: compile
compile: preprocess
	$(ARDUINO_CLI) compile $(VERBOSE) -b $(BOARD_FQDN) $(BUILD_FLAGS) --build-path $(BUILD_PATH)

.PHONY: upload
upload:
	$(ARDUINO_CLI) upload $(VERBOSE) -b $(BOARD_FQDN) -p $(MONITOR_PORT) --input-dir $(BUILD_PATH)

.PHONY: monitor
monitor:
ifeq ($(MONITOR_CMD),picocom)
	$(MONITOR_CMD) -b $(MONITOR_BAUDRATE) $(MONITOR_PORT)
else
	echo "Unknown tool selected for MONITOR_CMD=$(MONITOR_CMD)" >&2
	exit 1
endif

.PHONY: clean
clean:
	rm -rf $(BUILD_PATH)

## Aliases

.PHONY: serial
serial: monitor

.PHONY: flash
flash: upload

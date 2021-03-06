# Experiments with Arduino

This repository contains all sorts of stuff related to Arduino. This is mostly
just my own stuff as I use it and is not necessarily meant for direct use by
others. Documentation might be missing, but feel free to ask if you find
anything interesting you would like to know more about.

## git submodule configuration

This repository uses external repositories using git submodules:

    $ git submodule init
    $ git submodule update


## Boards Management

### AVR

Everything is builtin into Arduino and Arduino-Makefile. No special
configuration necessary.


### ESP8266

Support for esp8266 can be enabled by downloading additional tools. Refer to
the official [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
instructions.

Additonallity, [arduino-cli](https://github.com/arduino/arduino-cli) is
required for compilation and for flashing the device.

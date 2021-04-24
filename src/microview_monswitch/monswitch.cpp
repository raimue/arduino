// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 Rainer Müller <raimue@codingfarm.de>

#include <avr/sleep.h>

#include <MicroView.h>
#include <EnableInterrupt.h>

#define BUTTON0      A1
#define BUTTON1      A4
#define BUTTON1_GND  A5
#define SWITCH0      A0

#define DEBOUNCE_INTERVAL  100 // ms

#define BUTTON_ISR(button)                              \
    static volatile uint8_t button ## _event = 0;       \
                                                        \
    void button ## _pressed() {                         \
        static uint32_t last = 0;                       \
        uint32_t now = millis();                        \
        if (now - last > DEBOUNCE_INTERVAL) {           \
            button ## _event++;                         \
        }                                               \
        last = now;                                     \
    }

BUTTON_ISR(button0)
BUTTON_ISR(button1)
BUTTON_ISR(switch0)

void setup() {
    // Button 0 as input with pull-up
    pinMode(BUTTON0, INPUT_PULLUP);
    enableInterrupt(BUTTON0, button0_pressed, FALLING);

    // Button 1 as input with pull-up,
    // GND provided by pin 2
    pinMode(BUTTON1_GND, OUTPUT);
    digitalWrite(BUTTON1_GND, LOW);
    pinMode(BUTTON1, INPUT_PULLUP);
    enableInterrupt(BUTTON1, button1_pressed, FALLING);

    // Switch 0 as input with pull-up
    pinMode(SWITCH0, INPUT_PULLUP);
    enableInterrupt(SWITCH0, switch0_pressed, CHANGE);

    // MicroView init
    uView.begin();
    uView.flipHorizontal(true);
    uView.flipVertical(true);
    uView.clear(PAGE);

    // Serial init
    Serial.begin(115200);
    Serial.println("BOOT MicroView ready!");
}

void loop() {
    static uint8_t button0_visible = 0;
    static uint8_t button1_visible = 0;
    static uint8_t switch0_visible = 0;

    uView.clear(PAGE);

    if (button0_event) {
        Serial.println("EVENT BUTTON0");
        button0_visible = 5;

        button0_event = 0;
    }

    if (button1_event) {
        Serial.println("EVENT BUTTON1");
        button1_visible = 5;

        button1_event = 0;
    }

    if (switch0_event) {
        Serial.print("EVENT SWITCH0 ");
        Serial.println(digitalRead(SWITCH0));
        switch0_visible = 5;

        switch0_event = 0;
    }

    if (button0_visible > 0) {
        for (int i = 0; i < 4; i++) {
            uView.lineH(0, i, 30);
        }
        button0_visible--;
    }
    if (button1_visible > 0) {
        for (int i = 0; i < 4; i++) {
            uView.lineH(34, i, 30);
        }
        button1_visible--;
    }

    if (switch0_visible > 0) {
        if (digitalRead(SWITCH0)) {
            uView.circle(32, 28, 15);
        } else {
            uView.circleFill(32, 28, 15);
        }
        switch0_visible--;
    }

    uView.display();

    delay(100);
}

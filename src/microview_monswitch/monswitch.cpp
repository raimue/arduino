// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 Rainer Müller <raimue@codingfarm.de>

#include <MicroView.h>

#define BUTTON0 A1
#define BUTTON1 A4
#define SWITCH0 A0

void setup() {
    // Button 0 as input with pull-up
    pinMode(BUTTON0, INPUT);
    digitalWrite(BUTTON0, HIGH);

    // Button 1 as input with pull-up,
    // GND provided by pin 2
    pinMode(BUTTON1, INPUT);
    digitalWrite(BUTTON1, HIGH);
    pinMode(A5, OUTPUT);
    digitalWrite(A5, LOW);

    // Switch 0 as input with pull-up
    digitalWrite(SWITCH0, INPUT);
    digitalWrite(SWITCH0, HIGH);

    // MicroView init
    uView.begin();
    uView.flipHorizontal(true);
    uView.flipVertical(true);
    uView.clear(PAGE);
}

void loop() {
    uView.setCursor(0, 0);
    uView.clear(PAGE);
    uView.print("HelloWorld");
    uView.print(digitalRead(BUTTON0));
    uView.print(digitalRead(BUTTON1));
    uView.print(digitalRead(SWITCH0));
    uView.display();

    delay(100);
}

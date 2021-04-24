/*
MicroView Arduino Library
Copyright (C) 2014 GeekAmmo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

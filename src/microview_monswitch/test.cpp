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

MicroViewWidget *widget;

void setup() {
    uView.begin();
    uView.flipHorizontal(true);
    uView.flipVertical(true);
    widget = new MicroViewGauge(32, 24, 0, 1023, WIDGETSTYLE0);
    uView.clear(PAGE);
}

void loop() {
    int sense = A0;
    pinMode(sense, INPUT);
    digitalWrite(sense, HIGH);

    int res = analogRead(sense);

    uView.clear(PAGE);
    widget->reDraw();
    widget->setValue(res);
    uView.display();

    delay(10);
}

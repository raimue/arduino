#include <MicroView.h>

void setup() {
    uView.begin();
    uView.flipVertical(true);
    uView.flipHorizontal(true);
}

void loop () {
    static uint8_t x = 0;
    static uint8_t y = 0;

    uView.setCursor(x, y);
    uView.clear(PAGE);
    uView.print("HelloWorld");
    uView.display();

    y++;
    if (y > uView.getLCDHeight() - uView.getFontHeight()) {
        y = 0;
    }

    delay(250);
}

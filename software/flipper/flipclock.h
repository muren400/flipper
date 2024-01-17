#ifndef FLIPCLOCK_H
#define FLIPCLOCK_H

#include "display.h"
#include "flipper.h"

class FlipClock {
 public:
  FlipClock(Display display) {
    this->display = display;
  }

  void drawTime(uint8_t hours, uint8_t minutes) {
    display.clearPixelBuffer();

    uint8_t hours1 = hours / 10;
    uint8_t hours2 = hours % 10;

    uint8_t minutes1 = minutes / 10;
    uint8_t minutes2 = minutes % 10;

    display.drawCharacter(2, 3, hours1);
    display.drawCharacter(8, 3, hours2);
    display.drawCharacter(13, 3, 10);
    display.drawCharacter(17, 3, minutes1);
    display.drawCharacter(23, 3, minutes2);

    display.writePixelBuffer();
  }

 private:
  Display display;
};

#endif
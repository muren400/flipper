#ifndef FLIPCLOCK_H
#define FLIPCLOCK_H

#include "display.h"
#include "flipper.h"

class FlipClock {
 public:
  FlipClock(Display display) {
    this->display = display;
    this->minutes = 0;
  }

  void tick(uint64_t millis) {
    // serialPrintCurrentTime(millis);
    drawCurrentTime(millis);
  }

  void configureOffsetFromHourAndMinute(uint8_t hour, uint8_t minute, uint8_t timezone) {
    offset = ((timezone + hour) * 1000*60*60) + (minute * 60*1000);
  }

  void configureOffsetFromTimestamp(uint64_t timestamp, uint8_t timezone) {
    Serial.print("Timestamp: ");
    Serial.println(timestamp);

    offset = (timezone * 1000*60*60) + (timestamp % (1000*60*60*24));

    Serial.print("offset: ");
    Serial.println(offset);
  }

 private:
  Display display;
  uint64_t minutes;
  bool minutesChanged;
  uint64_t offset;

  void increaseMinute() {
    minutes += 1;
    if (minutes == 1440) {
      minutes = 0;
    }
  }

  void updateMinutes(uint64_t millis) {
    millis += offset;
    uint64_t currentMinutes = millis / 60000;
    // uint64_t currentMinutes = millis / 1000;

    if(currentMinutes != minutes) {
      minutesChanged = true;
      minutes = currentMinutes;
    }
  }

  void serialPrintCurrentTime(uint64_t millis) {
    updateMinutes(millis);

    if(!minutesChanged) {
      return;
    }

    uint8_t hour = (minutes / 60) % 24;
    uint8_t minute = minutes % 60;

    Serial.print("time: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.println();

    minutesChanged = false;
  }

  void drawCurrentTime(uint64_t millis) {
     updateMinutes(millis);

    if(!minutesChanged) {
      return;
    }

    uint8_t hour = (minutes / 60) % 24;
    uint8_t minute = minutes % 60;

    display.clearPixelBuffer();
    drawTime(hour, minute);
    display.writePixelBuffer();

    minutesChanged = false;
  }

  void drawTime(uint8_t hours, uint8_t minutes) {
    uint8_t hours1 = hours / 10;
    uint8_t hours2 = hours % 10;

    uint8_t minutes1 = minutes / 10;
    uint8_t minutes2 = minutes % 10;

    display.drawCharacter(2, 3, hours1);
    display.drawCharacter(8, 3, hours2);
    display.drawCharacter(13, 3, 10);
    display.drawCharacter(17, 3, minutes1);
    display.drawCharacter(23, 3, minutes2);
  }
};

#endif
// #include "esp32-hal.h"
#ifndef DISPLAY_H
#define DISPLAY_H

#include "characters.h"
#include "flipper.h"

#define LATCH_PIN 19  // ROW_REG_CLK
#define CLOCK_PIN 22  // RC_SHIFT_CLK
#define DATA_PIN 23   // RC_DATA

#define CLEAR 21
#define ROW_EN 18

#define COL_DATA_PIN 17
#define COL_EN 26

#define A_0 16
#define A_1 4
#define A_2 32
#define B_0 33
#define B_1 25

class Display {
 public:
  Display() { init(); }

  void clearPixelBuffer() {
    for (int i = 0; i < 16; i++) {
      pixelBuffer[i] = 0;
    }
  }

  // maxY = 9
  void drawCharacter(uint8_t x, uint8_t y, char character) {
    x = 24 - x;

    for (int i = 0; i < 11; i++) {
      pixelBuffer[i + y] |= (uint32_t)((characters[i + (character * 11)]) << x);
    }
  }

  void drawPixel(uint8_t x, uint8_t y) {}

  void writePixelBuffer() {
    static bool isInitialWrite = true;

    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 28; j++) {
        uint32_t bitMask = (1 << 27) >> j;
        uint32_t maskedPixel = pixelBuffer[i] & bitMask;
        uint32_t oldMaskedPixel = oldPixelBuffer[i] & bitMask;

        bool pixelHasNotChanged = oldMaskedPixel == maskedPixel;
        if (pixelHasNotChanged && !isInitialWrite) {
          continue;
        }

        bool isOn = maskedPixel > 0;
        setPixel(i, j, isOn);
      }

      oldPixelBuffer[i] = pixelBuffer[i];
    }
    
    isInitialWrite = false;
  }

 private:
  bool dirty = true;

  uint8_t rowBuffer[4];
  uint32_t pixelBuffer[16];
  uint32_t oldPixelBuffer[16];

  void init() {
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(ROW_EN, OUTPUT);
    pinMode(CLEAR, OUTPUT);

    pinMode(COL_EN, OUTPUT);
    pinMode(COL_DATA_PIN, OUTPUT);
    pinMode(A_0, OUTPUT);
    pinMode(A_1, OUTPUT);
    pinMode(A_2, OUTPUT);
    pinMode(B_0, OUTPUT);
    pinMode(B_1, OUTPUT);

    digitalWrite(ROW_EN, LOW);
    digitalWrite(CLEAR, LOW);
    digitalWrite(CLEAR, HIGH);
    digitalWrite(LATCH_PIN, LOW);
    digitalWrite(CLOCK_PIN, LOW);

    digitalWrite(COL_EN, LOW);
    digitalWrite(COL_DATA_PIN, LOW);
    digitalWrite(A_0, LOW);
    digitalWrite(A_1, LOW);
    digitalWrite(A_2, LOW);
    digitalWrite(B_0, LOW);
    digitalWrite(B_1, LOW);

    setNeutral();
    fillDisplay();
    clearDisplay();
    setNeutral();
  }

  void setRowHigh(uint8_t row) {
    for (int i = 0; i < 4; i++) {
      uint8_t value = 0;

      for (int j = 0; j < 8; j += 2) {
        value |= (1 << j);
      }

      if (row / 4 == i) {
        uint8_t bitIndex = (row % 4) * 2;
        value &= ~(1 << bitIndex);
      }

      rowBuffer[i] = value;
    }

    writeRowBuffer();
  }

  // 1 1
  void setRowLow(uint8_t row) {
    for (int i = 0; i < 4; i++) {
      uint8_t value = 0;

      for (int j = 0; j < 8; j += 2) {
        value |= (1 << j);
      }

      if (row / 4 == i) {
        uint8_t bitIndex = ((row % 4) * 2) + 1;
        value |= (1 << bitIndex);
      }

      rowBuffer[i] = value;
    }

    writeRowBuffer();
  }

  void setNeutral() {
    rowBuffer[0] = 0b01010101;
    rowBuffer[1] = 0b01010101;
    rowBuffer[2] = 0b01010101;
    rowBuffer[3] = 0b01010101;
    writeRowBuffer();
  }

  // B1 B0 A2 A1 A0
  //  0  0  0  0  1 => 0
  //  1  1  1  1  1 => 28
  void setColumnAdress(uint8_t col) {
    col++;

    if (col > 7) {
      col++;
    }

    if (col > 15) {
      col++;
    }

    if (col > 23) {
      col++;
    }

    setColumnBit(col, 0, A_0);
    setColumnBit(col, 1, A_1);
    setColumnBit(col, 2, A_2);
    setColumnBit(col, 3, B_0);
    setColumnBit(col, 4, B_1);
  }

  void setColumnBit(uint8_t col, uint8_t bit, int pin) {
    if ((col & (1 << bit)) > 0) {
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }
  }

  void myShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder,
                  uint8_t val) {
    uint8_t i;

    for (i = 0; i < 8; i++) {
      if (bitOrder == LSBFIRST)
        digitalWrite(dataPin, !!(val & (1 << i)));
      else
        digitalWrite(dataPin, !!(val & (1 << (7 - i))));

      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
  }

  void writeShiftRegisters(uint8_t val) {
    digitalWrite(LATCH_PIN, LOW);
    myShiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, val);
    digitalWrite(LATCH_PIN, HIGH);
    digitalWrite(LATCH_PIN, LOW);
  }

  void writeRowBuffer() {
    digitalWrite(LATCH_PIN, LOW);
    myShiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowBuffer[3]);
    myShiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowBuffer[2]);
    myShiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowBuffer[1]);
    myShiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowBuffer[0]);
    digitalWrite(LATCH_PIN, HIGH);
    digitalWrite(LATCH_PIN, LOW);
  }

  void enableChip(bool enable) {
    if (enable) {
      digitalWrite(ROW_EN, LOW);
    } else {
      digitalWrite(ROW_EN, HIGH);
    }
  }

  void clearDisplay() {
    for (int j = 0; j < 28; j++) {
      for (int i = 0; i < 16; i++) {
        setPixel(i, j, false);
      }
    }
  }

  void fillDisplay() {
    for (int j = 0; j < 28; j++) {
      for (int i = 0; i < 16; i++) {
        setPixel(i, j, true);
      }
    }
  }

  void setPixel(uint8_t row, uint8_t col, bool isOn) {
    setColumnAdress(col);

    if (isOn) {
      digitalWrite(COL_DATA_PIN, LOW);
      setRowHigh(row);
    } else {
      digitalWrite(COL_DATA_PIN, HIGH);
      setRowLow(row);
    }

    digitalWrite(COL_EN, HIGH);
    delayMicroseconds(500);
    digitalWrite(COL_EN, LOW);
    setNeutral();
    delay(2);
    // delayMicroseconds(500);
  }
};

#endif
//#include <Arduino.h>
#include <SPI.h>

#include "xpt2046.h"

static SPISettings spiConfig;
static uint32_t timeout = 0;

extern int8_t encoderDiff;

uint8_t xpt2046_read_buttons() {
  uint16_t x, y;

  if (timeout > millis()) return 0;
  timeout = millis() + 250;

  spiConfig = SPISettings(XPT2046_SPI_CLOCK, MSBFIRST, SPI_MODE0);
  if (!getTouchPoint(&x, &y)) return 0;

  x = (uint16_t)((((int32_t)x * (int32_t)XPT2046_X_CALIBRATION) >> 16) + XPT2046_X_OFFSET);
  y = (uint16_t)((((int32_t)y * (int32_t)XPT2046_Y_CALIBRATION) >> 16) + XPT2046_Y_OFFSET);

  if (y < 185 || y > 224) return 0;
  if (x >  20 & x <  99) encoderDiff = - ENCODER_STEPS_PER_MENU_ITEM * ENCODER_PULSES_PER_STEP;
  if (x > 120 & x < 199) encoderDiff = ENCODER_STEPS_PER_MENU_ITEM * ENCODER_PULSES_PER_STEP;
  if (x > 220 & x < 299) return EN_C;
  return 0;
}

uint16_t getTouchCoordinate(uint8_t coordinate) {
    coordinate |= XPT2046_CONTROL | XPT2046_DFR_MODE;

    OUT_WRITE(TOUCH_CS, LOW);
    SPI.beginTransaction(spiConfig);

    uint16_t data[3], delta[3];

    SPI.transfer(coordinate);
    for (uint32_t i = 0; i < 3 ; i++) {
      data[i] = ((uint16_t) SPI.transfer(0x00)) << 4;
      data[i] |= ((uint16_t) SPI.transfer(coordinate)) >> 4;
    }
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);

    SPI.endTransaction();
    WRITE(TOUCH_CS, HIGH);

    delta[0] = data[0] > data[1] ? data[0] - data [1] : data[1] - data [0];
    delta[1] = data[0] > data[2] ? data[0] - data [2] : data[2] - data [0];
    delta[2] = data[1] > data[2] ? data[1] - data [2] : data[2] - data [1];

    if (delta[0] <= delta[1] && delta[0] <= delta[2]) return (data[0] + data [1]) >> 1;
    if (delta[1] <= delta[2]) return (data[0] + data [2]) >> 1;
    return (data[1] + data [2]) >> 1;
}

bool getTouchPoint(uint16_t *x, uint16_t *y) {
  if (!isTouched()) return false;
  *x = getTouchCoordinate(XPT2046_X);
  *y = getTouchCoordinate(XPT2046_Y);
  return isTouched();
}

/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016, 2017 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*

  u8g_dev_tft_320x240_upscale_from_128x64.cpp

  Universal 8bit Graphics Library

  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "../../inc/MarlinConfig.h"

#if HAS_GRAPHICAL_LCD

#include "U8glib.h"
#include "HAL_LCD_com_defines.h"
#include "string.h"

#include "../../lcd/ultralcd.h"
#if HAS_COLOR_LEDS && ENABLED(PRINTER_EVENT_LEDS)
#include "../../feature/leds/leds.h"
#endif

struct LCD_IO {
  uint32_t id;
  void (*writeRegister)(uint16_t reg);
  uint16_t (*readData)(void);
  void (*writeData)(uint16_t data);
  void (*writeMultiple)(uint16_t data, uint32_t count);
  void (*writeSequence)(uint16_t *data, uint16_t length);
  void (*setWindow)(uint16_t Xmin, uint16_t Ymin, uint16_t Xmax, uint16_t Ymax);
};
static LCD_IO lcd = {0, NULL, NULL, NULL, NULL, NULL, NULL};

#define WIDTH 128
#define HEIGHT 64
#define PAGE_HEIGHT 8
#if ENABLED(MKS_ROBIN_TFT35)
#define X_MIN (32+80)
#define Y_MIN (28+40)
#define X_MAX (X_MIN + 2 * WIDTH  - 1)
#define Y_MAX (Y_MIN + 2 * HEIGHT - 1)
#else
#define X_MIN 32
#define Y_MIN 28
#define X_MAX (X_MIN + 2 * WIDTH  - 1)
#define Y_MAX (Y_MIN + 2 * HEIGHT - 1)
#endif
static uint32_t lcd_id = 0;
uint16_t color = 0xFFFF;

#define ESC_REG(x)      0xFFFF, 0x00FF & (uint16_t)x
#define ESC_DELAY(x)    0xFFFF, 0x8000 | (x & 0x7FFF)
#define ESC_END         0xFFFF, 0x7FFF
#define ESC_FFFF        0xFFFF, 0xFFFF

void writeEscSequence(const uint16_t *sequence) {
  uint16_t data;
  for (;;) {
    data = *sequence++;
    if (data != 0xFFFF) {
      lcd.writeData(data);
      continue;
    }
    data = *sequence++;
    if (data == 0x7FFF) return;
    if (data == 0xFFFF) {
      lcd.writeData(data);
    } else if (data & 0x8000) {
      delay(data & 0x7FFF);
    } else if ((data & 0xFF00) == 0) {
      lcd.writeRegister(data);
    }
  }
}

static const uint16_t st7789v_init[] = {
  ESC_REG(0x10), ESC_DELAY(10), ESC_REG(0x01), ESC_DELAY(200), ESC_REG(0x11), ESC_DELAY(120),
  ESC_REG(0x36), 0x00A0,
  ESC_REG(0x3A), 0x0055,
  ESC_REG(0x2A), 0x0000, 0x0000, 0x0001, 0x003F,
  ESC_REG(0x2B), 0x0000, 0x0000, 0x0000, 0x00EF,
  ESC_REG(0xB2), 0x000C, 0x000C, 0x0000, 0x0033, 0x0033,
  ESC_REG(0xB7), 0x0035,
  ESC_REG(0xBB), 0x001F,
  ESC_REG(0xC0), 0x002C,
  ESC_REG(0xC2), 0x0001, 0x00C3,
  ESC_REG(0xC4), 0x0020,
  ESC_REG(0xC6), 0x000F,
  ESC_REG(0xD0), 0x00A4, 0x00A1,
  ESC_REG(0x29),
  ESC_REG(0x11),
  ESC_END
};
static const uint16_t ili9488_init[] = {
  ESC_REG(0x10), ESC_DELAY(10), ESC_REG(0x01), ESC_DELAY(200), ESC_REG(0x11), ESC_DELAY(120),
  ESC_REG(0x36), 0x0068,
  ESC_REG(0x3A), 0x0055,
  ESC_REG(0x2A), 0x0000, 0x0000, 0x0001, 0x00DF,
  ESC_REG(0x2B), 0x0000, 0x0000, 0x0001, 0x003F,
  ESC_REG(0xB0), 0x0000, 
  ESC_REG(0xB1), 0x00B0, 0x0011, 
  ESC_REG(0xB4), 0x0002, 
  ESC_REG(0xB6), 0x0002, 0x0042,
  ESC_REG(0xB7), 0x00C6,
  ESC_REG(0xC0), 0x0010, 0x0010,
  ESC_REG(0xC1), 0x0041,
  ESC_REG(0xC5), 0x0000,0x0022,0x0080,
  ESC_REG(0xD0), 0x00A4, 0x00A1,
  ESC_REG(0x29),
  ESC_REG(0x11),
  ESC_END
};
static const uint16_t ili9328_init[] = {
  ESC_REG(0x0001), 0x0100,
  ESC_REG(0x0002), 0x0400,
  ESC_REG(0x0003), 0x1038,
  ESC_REG(0x0004), 0x0000,
  ESC_REG(0x0008), 0x0202,
  ESC_REG(0x0009), 0x0000,
  ESC_REG(0x000A), 0x0000,
  ESC_REG(0x000C), 0x0000,
  ESC_REG(0x000D), 0x0000,
  ESC_REG(0x000F), 0x0000,
  ESC_REG(0x0010), 0x0000,
  ESC_REG(0x0011), 0x0007,
  ESC_REG(0x0012), 0x0000,
  ESC_REG(0x0013), 0x0000,
  ESC_REG(0x0007), 0x0001,
  ESC_DELAY(200),
  ESC_REG(0x0010), 0x1690,
  ESC_REG(0x0011), 0x0227,
  ESC_DELAY(50),
  ESC_REG(0x0012), 0x008C,
  ESC_DELAY(50),
  ESC_REG(0x0013), 0x1500,
  ESC_REG(0x0029), 0x0004,
  ESC_REG(0x002B), 0x000D,
  ESC_DELAY(50),
  ESC_REG(0x0050), 0x0000,
  ESC_REG(0x0051), 0x00EF,
  ESC_REG(0x0052), 0x0000,
  ESC_REG(0x0053), 0x013F,
  ESC_REG(0x0020), 0x0000,
  ESC_REG(0x0021), 0x0000,
  ESC_REG(0x0060), 0x2700,
  ESC_REG(0x0061), 0x0001,
  ESC_REG(0x006A), 0x0000,
  ESC_REG(0x0080), 0x0000,
  ESC_REG(0x0081), 0x0000,
  ESC_REG(0x0082), 0x0000,
  ESC_REG(0x0083), 0x0000,
  ESC_REG(0x0084), 0x0000,
  ESC_REG(0x0085), 0x0000,
  ESC_REG(0x0090), 0x0010,
  ESC_REG(0x0092), 0x0600,
  ESC_REG(0x0007), 0x0133,
  ESC_REG(0x0022),
  ESC_END
};

static const uint8_t button0[] = {
   B01111111,B11111111,B11111111,B11111111,B11111110,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00001000,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00111110,B00000000,B00000001,
   B10000000,B00000000,B01111111,B00000000,B00000001,
   B10000000,B00000000,B11111111,B10000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B01111111,B11111111,B11111111,B11111111,B11111110,
};

static const uint8_t button1[] = {
   B01111111,B11111111,B11111111,B11111111,B11111110,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B11111111,B10000000,B00000001,
   B10000000,B00000000,B01111111,B00000000,B00000001,
   B10000000,B00000000,B00111110,B00000000,B00000001,
   B10000000,B00000000,B00011100,B00000000,B00000001,
   B10000000,B00000000,B00001000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B01111111,B11111111,B11111111,B11111111,B11111110,
};

static const uint8_t button2[] = {
   B01111111,B11111111,B11111111,B11111111,B11111110,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B01000001,B11000000,B00000001,
   B10000000,B00000000,B11000001,B11000000,B00000001,
   B10000000,B00000001,B11111111,B11000000,B00000001,
   B10000000,B00000011,B11111111,B11000000,B00000001,
   B10000000,B00000001,B11111111,B11000000,B00000001,
   B10000000,B00000000,B11000000,B00000000,B00000001,
   B10000000,B00000000,B01000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B10000000,B00000000,B00000000,B00000000,B00000001,
   B01111111,B11111111,B11111111,B11111111,B11111110,
};

static void _setWindow_x20_x21_x22(uint16_t Xmin, uint16_t Ymin, uint16_t Xmax, uint16_t Ymax) {
  lcd.writeRegister(0x50);
  lcd.writeData(Ymin);
  lcd.writeRegister(0x51);
  lcd.writeData(Ymax);
  lcd.writeRegister(0x52);
  lcd.writeData(Xmin);
  lcd.writeRegister(0x53);
  lcd.writeData(Xmax);

  lcd.writeRegister(0x20);
  lcd.writeData(Ymin);
  lcd.writeRegister(0x21);
  lcd.writeData(Xmin);

  lcd.writeRegister(0x22);
}

static void _setWindow_x2a_x2b_x2c(uint16_t Xmin, uint16_t Ymin, uint16_t Xmax, uint16_t Ymax) {
  lcd.writeRegister(0x2A);
  lcd.writeData((Xmin >> 8) & 0xFF);
  lcd.writeData(Xmin & 0xFF);
  lcd.writeData((Xmax >> 8) & 0xFF);
  lcd.writeData(Xmax & 0xFF);

  lcd.writeRegister(0x2B);
  lcd.writeData((Ymin >> 8) & 0xFF);
  lcd.writeData(Ymin & 0xFF);
  lcd.writeData((Ymax >> 8) & 0xFF);
  lcd.writeData(Ymax & 0xFF);

  lcd.writeRegister(0x2C);
}

void drawImage(const uint8_t *data, uint16_t length, uint16_t height) {
  uint16_t i, j, k;
  uint16_t buffer[160];

  for (i = 0; i < height; i++) {
    k = 0;
    for (j = 0; j < length; j++) {
      if (*(data + (i * (length >> 3) + (j >> 3))) & (128 >> (j & 7))) {
        buffer[k++] = color;
        buffer[k++] = color;
      } else {
        buffer[k++] = 0x0000;
        buffer[k++] = 0x0000;
      }
    }
    lcd.writeSequence(buffer, length << 1);
    lcd.writeSequence(buffer, length << 1);
  }
}
#if ENABLED(MKS_ROBIN_TFT35)
void drawUI(void) {
  lcd.setWindow(10+80, 170+40, 309+80, 171+40);
  lcd.writeMultiple(color, 600);

  lcd.setWindow( 20+80, 185+40,  99+80, 224+40);
  drawImage(button0, 40, 20);
  lcd.setWindow(120+80, 185+40, 199+80, 224+40);
  drawImage(button1, 40, 20);
  lcd.setWindow(220+80, 185+40, 299+80, 224+40);
  drawImage(button2, 40, 20);
}
#else
void drawUI(void) {
  lcd.setWindow(10, 170, 309, 171);
  lcd.writeMultiple(color, 600);

  lcd.setWindow( 20, 185,  99, 224);
  drawImage(button0, 40, 20);
  lcd.setWindow(120, 185, 199, 224);
  drawImage(button1, 40, 20);
  lcd.setWindow(220, 185, 299, 224);
  drawImage(button2, 40, 20);
}
#endif

uint8_t u8g_dev_tft_320x240_upscale_from_128x64_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
#if HAS_COLOR_LEDS && ENABLED(PRINTER_EVENT_LEDS)
  uint16_t newColor;
#endif
  u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
  uint16_t buffer[256];
  uint32_t i, j, k;
  uint8_t byte;

  uint16_t reg00;

  switch(msg) {
    case U8G_DEV_MSG_INIT:
      dev->com_fn(u8g, U8G_COM_MSG_INIT, U8G_SPI_CLK_CYCLE_NONE, &lcd);
      if (lcd.writeRegister == NULL || lcd.writeData == NULL || lcd.readData == NULL || lcd.writeSequence == NULL) break;

//    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    !!! POC implementation. NOT compatible with 8-bit interface (SPI / FSMC 8-bit) !!!
//    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      lcd.writeRegister(0x0000);
      reg00 = lcd.readData();
      if (reg00 == 0 || reg00 == 0xFFFF) {
        lcd.writeRegister(0x0004);
        lcd.readData(); // dummy read
        lcd.id = ((uint32_t)(lcd.readData() & 0xff) << 16) | ((uint32_t)(lcd.readData() & 0xff) << 8) | (uint32_t)(lcd.readData() & 0xff);
        #if ENABLED(MKS_ROBIN_TFT35)
        if(lcd.id != 0x8552)
        {
            lcd.writeRegister(0x00d3);
            lcd.readData(); // dummy read
            lcd.id = ((uint32_t)(lcd.readData() & 0xff) << 16) | ((uint32_t)(lcd.readData() & 0xff) << 8) | (uint32_t)(lcd.readData() & 0xff);
        }
        #endif
        if (lcd_id == 0x040404) { // No connected display on FSMC
          lcd.id = 0;
          return 0;
        }
      } else {
        lcd.id = (uint32_t)reg00;
      }

      switch(lcd.id & 0xFFFF) {
        case 0x8552:   // ST7789V
          writeEscSequence(st7789v_init);
          lcd.setWindow = _setWindow_x2a_x2b_x2c;
          break;
        case 0x9328:  // ILI9328
          writeEscSequence(ili9328_init);
          lcd.setWindow = _setWindow_x20_x21_x22;
          break;
        #if ENABLED(MKS_ROBIN_TFT35)
        case 0x9488:
          writeEscSequence(ili9488_init);
          lcd.setWindow = _setWindow_x2a_x2b_x2c;            
          break;
          #endif
        case 0x0404:  // No connected display on FSMC
          lcd.id = 0;
          return 0;
        case 0xFFFF:  // No connected display on SPI
          lcd.id = 0;
          return 0;
        default:
          if (reg00 == 0)
            lcd.setWindow = _setWindow_x2a_x2b_x2c;
          else
            lcd.setWindow = _setWindow_x20_x21_x22;
          break;
      }
      #if ENABLED(MKS_ROBIN_TFT35)
       if(lcd.id==0x9488)
       {
            lcd.setWindow(0,0,479,319);
            lcd.writeMultiple(0x0000, 480 * 320);       
       }
       else
        #endif
       {
            lcd.setWindow(0,0,319,239);
            lcd.writeMultiple(0x0000, 320 * 240);
       }
      drawUI();
      break;

    case U8G_DEV_MSG_STOP:
      break;

    case U8G_DEV_MSG_PAGE_FIRST:
      if (lcd.id == 0) break;

#if HAS_COLOR_LEDS && ENABLED(PRINTER_EVENT_LEDS)
      newColor = (0xF800 & (((uint16_t)leds.color.r) << 8)) | (0x07E0 & (((uint16_t)leds.color.g) << 3)) | (0x001F & (((uint16_t)leds.color.b) >> 3));
      if ((newColor != 0) && (newColor != color)) {
        color = newColor;
        drawUI();
      }
#endif
      lcd.setWindow(X_MIN,Y_MIN,X_MAX,Y_MAX);
      break;

    case U8G_DEV_MSG_PAGE_NEXT:
      if (lcd.id == 0) break;

      for (j = 0; j < 8;  j++) {
        k = 0;
        for (i = 0; i < (uint32_t) pb->width;  i++) {
          byte = *(((uint8_t *)pb->buf) + i);
          if (byte & (1 << j)) {
            buffer[k++] = color;
            buffer[k++] = color;
          } else {
            buffer[k++] = 0x0000;
            buffer[k++] = 0x0000;
          }
        }
        for (k = 0; k < 2; k++) lcd.writeSequence(buffer, 256);
      }
      break;

    case U8G_DEV_MSG_SLEEP_ON:
      return 1;

    case U8G_DEV_MSG_SLEEP_OFF:
      return 1;
  }
  return u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
}

U8G_PB_DEV(u8g_dev_tft_320x240_upscale_from_128x64, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_tft_320x240_upscale_from_128x64_fn, U8G_COM_HAL_FSMC_FN);

#endif // HAS_GRAPHICAL_LCD

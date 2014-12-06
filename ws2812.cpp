// Implementation (would go to .cpp file once library is separated)
// ================================================================
#include "ws2812.h"

static const uint8_t pwmTable[32] = {0, 1, 1, 2, 3, 4, 6, 7, 9, 10, 13, 15, 18, 21, 24, 28, 33, 38, 44, 50, 58, 67, 77, 88, 101, 115, 132, 150, 172, 196, 224, 255};
const uint16_t maxBuffSize = 720; // max. Array size depending of free RAM

p44_ws2812::p44_ws2812(uint16_t aNumLeds)
{
  LEDBufferSize = (aNumLeds > maxBuffSize ? maxBuffSize : aNumLeds);
  // allocate the buffer
  if((pixelBufferP = new RGBPixel[LEDBufferSize])!=NULL) {
    memset(pixelBufferP, 0, sizeof(RGBPixel)*LEDBufferSize); // all LEDs off
  }
}

p44_ws2812::~p44_ws2812()
{
  // free the buffer
  if (pixelBufferP) delete pixelBufferP;
}

int p44_ws2812::getNumLeds()
{
  return LEDBufferSize;
}

void p44_ws2812::begin()
{
  // begin using the driver
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8); // System clock is 72MHz, we need 9MHz for SPI
  SPI.setBitOrder(MSBFIRST); // MSB first for easier scope reading :-)
  SPI.transfer(0); // make sure SPI line starts low (Note: SPI line remains at level of last sent bit, fortunately)
}

void p44_ws2812::disableInterrupts()
{
  __disable_irq();
}

void p44_ws2812::enableInterrupts()
{
  __enable_irq();
}

void p44_ws2812::transmitBuffer()
{
  // Note: on the spark core, system IRQs might happen which exceed 50uS
  // causing WS2812 chips to reset in midst of data stream.
  // Thus, until we can send via DMA, we need to disable IRQs while sending
  // transfer RGB values to LED chain
  for (uint16_t i=0; i<LEDBufferSize; i++) {
    RGBPixel *pixP = &(pixelBufferP[i]);
    byte b;
    // Order of PWM data for WS2812 LEDs is G-R-B
    // - green
    b = pwmTable[pixP->green];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - red
    b = pwmTable[pixP->red];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - blue
    b = pwmTable[pixP->blue];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
  }
}

void p44_ws2812::transmitBuffer(uint16_t aStart, uint16_t aEnd)
{
  // Note: on the spark core, system IRQs might happen which exceed 50uS
  // causing WS2812 chips to reset in midst of data stream.
  // Thus, until we can send via DMA, we need to disable IRQs while sending
  // transfer RGB values to LED chain
  if (aStart > maxBuffSize) {return;}
  if (aEnd > maxBuffSize) {return;}
  for (uint16_t i=aStart; i<aEnd; i++) {
    RGBPixel *pixP = &(pixelBufferP[i]);
    byte b;
    // Order of PWM data for WS2812 LEDs is G-R-B
    // - green
    b = pwmTable[pixP->green];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - red
    b = pwmTable[pixP->red];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - blue
    b = pwmTable[pixP->blue];
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
  }
}

void p44_ws2812::transmitColorLine(uint16_t aLength, byte aRed, byte aGreen, byte aBlue)
{
  if (aLength==0) return; // invalid amount of LEDs
  for (uint16_t i=0; i<aLength; i++) {
    byte b;
    // Order of PWM data for WS2812 LEDs is G-R-B
    // - green
    b = aGreen;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - red
    b = aRed;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - blue
    b = aBlue;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
  }
}

void p44_ws2812::setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue)
{
  if (aLedNumber>=LEDBufferSize) return; // invalid LED number
  RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
  // linear brightness is stored with 5bit precision only
  pixP->red = aRed>>3;
  pixP->green = aGreen>>3;
  pixP->blue = aBlue>>3;
}

void p44_ws2812::setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aBrightness)
{
  setColor(aLedNumber, (aRed*aBrightness)>>8, (aGreen*aBrightness)>>8, (aBlue*aBrightness)>>8);
}

void p44_ws2812::getColor(uint16_t aLedNumber, byte &aRed, byte &aGreen, byte &aBlue)
{
  if (aLedNumber>=LEDBufferSize) return; // invalid LED number
  RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
  // linear brightness is stored with 5bit precision only
  aRed = pixP->red<<3;
  aGreen = pixP->green<<3;
  aBlue = pixP->blue<<3;
}

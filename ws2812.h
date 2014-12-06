/*
 * Spark Core library to control WS2812 based RGB LED devices
 * using SPI to create bitstream.
 * Future plan is to use DMA to feed the SPI, so WS2812 bitstream
 * can be produced without CPU load and without blocking IRQs
 *
 * (c) 2014 by luz@plan44.ch (GPG: 1CC60B3A)
 * Licensed as open source under the terms of the MIT License
 * (see LICENSE.TXT)
 */


// Declaration (would go to .h file once library is separated)
// ===========================================================
#include "application.h"

class p44_ws2812 {

  typedef struct {
    unsigned int red:5;
    unsigned int green:5;
    unsigned int blue:5;
  } __attribute((packed)) RGBPixel;

  uint16_t LEDBufferSize; // number of buffered LEDs in array
  RGBPixel *pixelBufferP; // the pixel buffer

public:
  /// create driver for a WS2812 LED chain
  /// @param aNumLeds number of LEDs in the chain
  p44_ws2812(uint16_t aNumLeds);

  /// destructor
  ~p44_ws2812();

  /// begin using the driver
  void begin();

  /// disable interrupts during trasmition
  void disableInterrupts();

  /// enable interrupts
  void enableInterrupts();

  /// transfer RGB values to LED chain
  /// @note this must be called to send actual LEDs after modifying RGB values
  /// with setColor() and/or setColorDimmed()
  void transmitBuffer();

  /// transfer RGB values to LED chain
  /// @note this must be called to update the actual LEDs after modifying RGB values
  /// @param aStart startpositon of buffer to transmit
  /// @param aEnd endposition of buffer to transmit
  /// with setColor() and/or setColorDimmed()
  void transmitBuffer(uint16_t aStart, uint16_t aEnd);

  /// transmit a line of color to the LEDs
  /// @param aLength number of LEDs
  /// @param aRed intensity of red component, 0..255
  /// @param aGreen intensity of green component, 0..255
  /// @param aBlue intensity of blue component, 0..255
  void transmitColorLine(uint16_t aLength, byte aRed, byte aGreen, byte aBlue);

  /// set color of one LED in buffer
  /// @param aRed intensity of red component, 0..255
  /// @param aGreen intensity of green component, 0..255
  /// @param aBlue intensity of blue component, 0..255
  void setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue);

  /// set color of one LED, scaled by a visible brightness (non-linear) factor (in buffer)
  /// @param aRed intensity of red component, 0..255
  /// @param aGreen intensity of green component, 0..255
  /// @param aBlue intensity of blue component, 0..255
  /// @param aBrightness brightness, will be converted non-linear to PWM duty cycle for uniform brightness scale, 0..255
  void setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aBrightness);

  /// get current color of LED from buffer
  /// @param aRed set to intensity of red component, 0..255
  /// @param aGreen set to intensity of green component, 0..255
  /// @param aBlue set to intensity of blue component, 0..255
  /// @note for LEDs set with setColorDimmed(), this returns the scaled down RGB values,
  ///   not the original r,g,b parameters. Note also that internal brightness resolution is 5 bits only.
  void getColor(uint16_t aLedNumber, byte &aRed, byte &aGreen, byte &aBlue);

  /// @return number of LEDs from buffer
  int getNumLeds();

};

// Configuration
// =============

// Note: total number of LEDs determines amount of RAM needed. Unfortunately, with every Spark core FW update,
//       the amout of RAM consumed by the Spark FW itself grew and less RAM was available to the app.
//       So while the app ran fine with 240 LEDs on early spark FW, with current v0.2.2 everything above ~120
//       caused the app to crash (red or magenta flash of death) due to exhausted RAM.
//       Users disabled Cheerlights and Digitalstrom to get more RAM, but full 240 LEDs were still not
//       possible any more.
//
//       Hopefully, Spark will optimize their FW over time (they plan to, see forum) to make more RAM available
//       to apps like this one.
//
//       In the meantime, we have to live with very tight RAM, so I tried to squeeze bits out of the code
//       (mainly by storing 3*5 = 15 bits for RGB linear brightness rather than 24 bits for RGB PWM duty cycle).
//
//       If future firmware should again reduce RAM, try reducing the number of LEDs by making
//       levels or ledsPerLevel smaller.

#include "ws2812.h"
#include "font.h"
#include "utilities.h"
#include "fire.h"


// Number of LEDs around the tube. One too much looks better (italic text look)
// than one to few (backwards leaning text look)
// Higher number = diameter of the torch gets larger
// Torch Mode
const uint16_t ledsPerLevel = 26; // Original: 13, smaller tube 11

// Number of "windings" of the LED strip around (or within) the tube
// Higher number = torch gets taller
// Torch Mode
const uint16_t levels = 27;// original 18, smaller tube 21

// Main program, torch simulation
// ==============================
// moved defining constants for number of LEDs to top of file
const uint16_t numLeds = ledsPerLevel*levels; // total size of dynamic LEDs array

// Number of installed LEDs.
// Lamp and Colorcycle Mode
const uint16_t ledsInStrip = 1200; // 5 x 4m à 60 LEDs/m


p44_ws2812 leds(numLeds); // create WS2812 driver with dynamic LEDs array

// global parameters
enum {
  mode_off = 0,
  mode_torch = 1, // torch
  mode_colorcycle = 2, // moving color cycle
  mode_lamp = 3, // lamp
};

byte mode = mode_torch; // main operation mode
int brightness = 255; // overall brightness
byte fade_base = 140; // crossfading base brightness level
byte cyclecnt = 0; // counter for cycle colors

#include "textlayer.h"
#include "torch.h"
#include "cycle.h"
#include "lamp.h"
#include "cloudapi.h"

void myHandler(const char *event, const char *data)
{
  if (data == 0)
    mode = mode_off;
  else
    mode = mode_torch;
}


// Main program
// ============
void setup()
{
  resetEnergy();
  resetText();
  leds.begin();
  // remote control
  Spark.function("params", handleParams); // parameters
  Spark.function("message", newMessage); // text message display
  //Spark.subscribe("bee_is_daylight", myHandler);
}

void loop()
{
  // render the text
  renderText();
  textStart = text_base_line*ledsPerLevel;
  textEnd = textStart+rowsPerGlyph*ledsPerLevel;
  switch (mode) {
    case mode_off: {
      // off
      leds.disableInterrupts();
      leds.transmitColorLine(ledsInStrip, 0, 0, 0);
      leds.enableInterrupts();
      break;
    }
    case mode_lamp: {
      // static color + text display
      DoLamp();
      break;
    }
    case mode_torch: {
      // torch animation + text display
      DoTorch();
      break;
    }
    case mode_colorcycle: {
      DoCycle();
      break;
    }
  }
  // wait
  delay(cycle_wait); // latch & reset needs 50 microseconds pause, at least.
}

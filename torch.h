// torch mode
// ==========

// fire color
int red_energy = 180; //180 // 255
int green_energy = 145; //145 // 205
int blue_energy = 0;

// torch parameters
uint16_t cycle_wait = 1; // 0..255

byte flame_min = 100; // 0..255
byte flame_max = 220; // 0..255

byte random_spark_probability = 3; // 0..100
byte spark_min = 200; // 0..255
byte spark_max = 255; // 0..255

byte spark_tfr = 40; // (40) 0..256 how much energy is transferred up for a spark per cycle
uint16_t spark_cap = 200; // (200) 0..255: spark cells: how much energy is retained from previous cycle

uint16_t up_rad = 60; // (60) up radiation
uint16_t side_rad = 35; // (30) sidewards radiation
uint16_t heat_cap = 0; // (0) (0..255: passive cells: how much energy is retained from previous cycle

byte red_bg = 0;
byte green_bg = 0;
byte blue_bg = 0;
byte red_bias = 10;
byte green_bias = 0;
byte blue_bias = 0;

byte upside_down = 0; // if set, flame (or rather: drop) animation is upside down. Text remains as-is

byte currentEnergy[numLeds]; // current energy level
byte nextEnergy[numLeds]; // next energy level
byte energyMode[numLeds]; // mode how energy is calculated for this point

enum {
  torch_passive = 0, // just environment, glow from nearby radiation
  torch_nop = 1, // no processing
  torch_spark = 2, // slowly looses energy, moves up
  torch_spark_temp = 3, // a spark still getting energy from the level below
};

void resetEnergy()
{
  for (int i=0; i<numLeds; i++) {
    currentEnergy[i] = 0;
    nextEnergy[i] = 0;
    energyMode[i] = torch_passive;
  }
}


void calcNextEnergy()
{
  int i = 0;
  for (int y=0; y<levels; y++) {
    for (int x=0; x<ledsPerLevel; x++) {
      byte e = currentEnergy[i];
      byte m = energyMode[i];
      switch (m) {
        case torch_spark: {
          // loose transfer up energy as long as there is any
          reduce(e, spark_tfr);
          // cell above is temp spark, sucking up energy from this cell until empty
          if (y<levels-1) {
            energyMode[i+ledsPerLevel] = torch_spark_temp;
          }
          break;
        }
        case torch_spark_temp: {
          // just getting some energy from below
          byte e2 = currentEnergy[i-ledsPerLevel];
          if (e2<spark_tfr) {
            // cell below is exhausted, becomes passive
            energyMode[i-ledsPerLevel] = torch_passive;
            // gobble up rest of energy
            increase(e, e2);
            // loose some overall energy
            e = ((int)e*spark_cap)>>8;
            // this cell becomes active spark
            energyMode[i] = torch_spark;
          }
          else {
            increase(e, spark_tfr);
          }
          break;
        }
        case torch_passive: {
          e = ((int)e*heat_cap)>>8;
          increase(e, ((((int)currentEnergy[i-1]+(int)currentEnergy[i+1])*side_rad)>>9) + (((int)currentEnergy[i-ledsPerLevel]*up_rad)>>8));
        }
        default:
          break;
      }
      nextEnergy[i++] = e;
    }
  }
}


const uint8_t energymap[32] = {0, 64, 96, 112, 128, 144, 152, 160, 168, 176, 184, 184, 192, 200, 200, 208, 208, 216, 216, 224, 224, 224, 232, 232, 232, 240, 240, 240, 240, 248, 248, 248};

void calcNextColors()
{
  int textStart = text_base_line*ledsPerLevel;
  int textEnd = textStart+rowsPerGlyph*ledsPerLevel;
  for (int i=0; i<numLeds; i++) {
    if (i>=textStart && i<textEnd && textLayer[i-textStart]>0) {
      // overlay with text color
      leds.setColorDimmed(i, red_text, green_text, blue_text, (brightness*textLayer[i-textStart])>>8);
    }
    else {
      int ei; // index into energy calculation buffer
      if (upside_down)
        ei = numLeds-i;
      else
        ei = i;
      uint16_t e = nextEnergy[ei];
      currentEnergy[ei] = e;
      if (e>250)
        while(1==0) { // while implemented to disable next function because commenting out does not work easaly
            leds.setColorDimmed(i, 170, 170, e, brightness); // blueish extra-bright spark
        }
      else {
        if (e>0) {
          // energy to brightness is non-linear
          byte eb = energymap[e>>3];
          byte r = red_bias;
          byte g = green_bias;
          byte b = blue_bias;
          increase(r, (eb*red_energy)>>8);
          increase(g, (eb*green_energy)>>8);
          increase(b, (eb*blue_energy)>>8);
          leds.setColorDimmed(i, r, g, b, brightness);
        }
        else {
          // background, no energy
          leds.setColorDimmed(i, red_bg, green_bg, blue_bg, brightness);
        }
      }
    }
  }
}

void injectRandom()
{
  // random flame energy at bottom row
  for (int i=0; i<ledsPerLevel; i++) {
    currentEnergy[i] = random(flame_min, flame_max);
    energyMode[i] = torch_nop;
  }
  // random sparks at second row
  for (int i=ledsPerLevel; i<2*ledsPerLevel; i++) {
    if (energyMode[i]!=torch_spark && random(100)<random_spark_probability) {
      currentEnergy[i] = random(spark_min, spark_max);
      energyMode[i] = torch_spark;
    }
  }
}


void DoTorch()
{
  injectRandom();
  calcNextEnergy();
  calcNextColors();
  leds.disableInterrupts();
  leds.transmitBuffer();
  leds.transmitBuffer(200, 720);
  leds.enableInterrupts();    
}

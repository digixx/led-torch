// Utilities
// =========


uint16_t random(uint16_t aMinOrMax, uint16_t aMax = 0)
{
  if (aMax==0) {
    aMax = aMinOrMax;
    aMinOrMax = 0;
  }
  uint32_t r = aMinOrMax;
  aMax = aMax - aMinOrMax + 1;
  r += rand() % aMax;
  return r;
}


inline void reduce(byte &aByte, byte aAmount, byte aMin = 0)
{
  int r = aByte-aAmount;
  if (r<aMin)
    aByte = aMin;
  else
    aByte = (byte)r;
}


inline void increase(byte &aByte, byte aAmount, byte aMax = 255)
{
  int r = aByte+aAmount;
  if (r>aMax)
    aByte = aMax;
  else
    aByte = (byte)r;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void wheel(byte WheelPos, byte &red, byte &green, byte &blue) {
  if(WheelPos < 85) {
    red = WheelPos * 3;
    green = 255 - WheelPos * 3;
    blue = 0;
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    red = 255 - WheelPos * 3;
    green = 0;
    blue = WheelPos * 3;
  } else {
    WheelPos -= 170;
    red = 0;
    green = WheelPos * 3;
    blue = 255 - WheelPos * 3;
  }
}


int hexToInt(char aHex)
{
  if (aHex<'0') return 0;
  aHex -= '0';
  if (aHex>9) aHex -= 7;
  if (aHex>15) return 0;
  return aHex;
}

// lamp mode params
byte lamp_red = 10;
byte lamp_green = 10;
byte lamp_blue = 10;

void DoLamp()
{
  leds.disableInterrupts();
  leds.transmitColorLine(ledsInStrip, lamp_red, lamp_green, lamp_blue);
  leds.enableInterrupts();
  /*
  // just single color lamp + text display
  for (int i=0; i<ledsInStrip; i++) {
    if (i>=textStart && i<textEnd && textLayer[i-textStart]>0) {
      leds.setColorDimmed(i, red_text, green_text, blue_text, (textLayer[i-textStart]*brightness)>>8);
    } else {
      leds.setColorDimmed(i, lamp_red, lamp_green, lamp_blue, brightness);
    }
  }
  */
}

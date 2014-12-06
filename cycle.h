void DoCycle()
{
    // simple color wheel animation
    byte r,g,b;
    byte wheeldivider = ledsInStrip / 128;
    cyclecnt += 2;
    leds.disableInterrupts();
    for(int i=0; i<ledsInStrip; i++) {
        wheel(((i / wheeldivider) - cyclecnt) , r, g, b);
        leds.transmitColorLine(1, r, g, b);
    }
    leds.enableInterrupts();

}

// text layer
// ==========
// text params
int text_intensity = 255; // intensity of last column of text (where text appears)
int cycles_per_px = 5;
int text_repeats = 15; // text displays until faded down to almost zero
int fade_per_repeat = 15; // how much to fade down per repeat
int text_base_line = 10;
int textStart;
int textEnd;
byte red_text = 0;
byte green_text = 255;
byte blue_text = 180;

// text layer, but only one strip around the tube (ledsPerLevel) with the height of the characters (rowsPerGlyph)
const int textPixels = ledsPerLevel*rowsPerGlyph;
byte textLayer[textPixels];
String text;

int textPixelOffset;
int textCycleCount;
int repeatCount;


// this function automagically gets called upon a matching POST request
int newMessage(String aText)
{
  // URL decode
  text = "";
  int i = 0;
  char c;
  while (i<(int)aText.length()) {
    if (aText[i]=='%') {
      if ((int)aText.length()<=i+2) break; // end of text
      // get hex
      c = (hexToInt(aText[i+1])<<4) + hexToInt(aText[i+2]);
      i += 2;
    }
    // Ä = C3 84
    // Ö = C3 96
    // Ü = C3 9C
    // ä = C3 A4
    // ö = C3 B6
    // ü = C3 BC
    else if (aText[i]==0xC3) {
      if ((int)aText.length()<=i+1) break; // end of text
      switch (aText[i+1]) {
        case 0x84: c = 0x80; break; // Ä
        case 0x96: c = 0x81; break; // Ö
        case 0x9C: c = 0x82; break; // Ü
        case 0xA4: c = 0x83; break; // ä
        case 0xB6: c = 0x84; break; // ö
        case 0xBC: c = 0x85; break; // ü
        default: c = 0x7F; break; // unknown
      }
      i += 1;
    }
    else {
      c = aText[i];
    }
    // put to output string
    text += String(c);
    i++;
  }
  // initiate display of new text
  textPixelOffset = -ledsPerLevel;
  textCycleCount = 0;
  repeatCount = 0;
  return 1;
}


void resetText()
{
  for(int i=0; i<textPixels; i++) {
    textLayer[i] = 0;
  }
}


void crossFade(byte aFader, byte aValue, byte &aOutputA, byte &aOutputB)
{
  byte baseBrightness = (aValue*fade_base)>>8;
  byte varBrightness = aValue-baseBrightness;
  byte fade = (varBrightness*aFader)>>8;
  aOutputB = baseBrightness+fade;
  aOutputA = baseBrightness+(varBrightness-fade);
}


void renderText()
{
  // fade between rows
  byte maxBright = text_intensity-repeatCount*fade_per_repeat;
  byte thisBright, nextBright;
  crossFade(255*textCycleCount/cycles_per_px, maxBright, thisBright, nextBright);
  // generate vertical rows
  int pixelsPerChar = bytesPerGlyph+glyphSpacing;
  int activeCols = ledsPerLevel-2;
  int totalTextPixels = (int)text.length()*pixelsPerChar;
  for (int x=0; x<ledsPerLevel; x++) {
    uint8_t column = 0;
    // determine font row
    if (x<activeCols) {
      int rowPixelOffset = textPixelOffset + x;
      if (rowPixelOffset>=0) {
        // visible row
        int charIndex = rowPixelOffset/pixelsPerChar;
        if ((int)text.length()>charIndex) {
          // visible char
          char c = text[charIndex];
          int glyphOffset = rowPixelOffset % pixelsPerChar;
          if (glyphOffset<bytesPerGlyph) {
            // fetch glyph column
            c -= 0x20;
            if (c>=numGlyphs) c = 95; // ASCII 0x7F-0x20
            column = fontBytes[c*bytesPerGlyph+glyphOffset];
          }
        }
      }
    }
    // now render columns
    for (int glyphRow=0; glyphRow<rowsPerGlyph; glyphRow++) {
      int i = glyphRow*ledsPerLevel + x; // LED index
      if (glyphRow < rowsPerGlyph) {
        if (column & (0x40>>glyphRow)) {
          textLayer[i] = thisBright;
          // also adjust pixel left to this one
          if (x>0) {
            increase(textLayer[i-1], nextBright, maxBright);
          }
          continue;
        }
      }
      textLayer[i] = 0; // no text
    }
  }
  // increment
  textCycleCount++;
  if (textCycleCount>=cycles_per_px) {
    textCycleCount = 0;
    textPixelOffset++;
    if (textPixelOffset>totalTextPixels) {
      // text shown, check for repeats
      repeatCount++;
      if (text_repeats!=0 && repeatCount>=text_repeats) {
        // done
        text = ""; // remove text
      }
      else {
        // show again
        textPixelOffset = -ledsPerLevel;
        textCycleCount = 0;
      }
    }
  }
}

#include "Adafruit_WS2801.h"

////////////////////////////////////////////////////////////////////////////////
// Globals & Constants

// These are just named quantities so we don't just see numbers all over the
// place. It's much easier to know what's going on when you read (e.g.)
// "numberofLEDs" instead of the number 25.

////////////////////////////////////////////////////////////////////////////////

// The number of LEDs in the string of WS2801s.
// This determines the maximum number of addressable lights which can be programmed.
const int numberOfLEDs = 25;

// The frame counter increases by one for each update of the lighting values.
// This value can be used to (e.g.) animate lighting colors over time.
static uint32_t frameCounter = 0;
const uint32_t frameDelay = 10;

// A selection of colors to cycle.
const uint32_t colorBlack = 0x000000;
const uint32_t colorRed = 0xff0000;
const uint32_t colorGreen = 0x00ff00;
const uint32_t colorBlue = 0x0000ff;
const uint32_t colorWhite = 0xffffff;

////////////////////////////////////////////////////////////////////////////////
// Helper Functions

// These math functions make it easier to assemble parts of an effect.

////////////////////////////////////////////////////////////////////////////////

// Take a red, green, and blue value and turn it into a color value.
uint32_t createColor(uint8_t r, uint8_t g, uint8_t b) {
  return (uint32_t(r) << 16) | (uint32_t(g) << 8) | (uint32_t(b));
}

// This function oscillates from 0-255, then 255-0, then it repeats.
// This simple sawtooth can be used to build other effects.
uint8_t fnSaw(uint16_t c) {
  c = c & 511;
  return c < 256 ? c : 255 - (c - 256);
}

// This function "clamps" values below 240 to zero.
// This creates a narrow band of lit values for strobe effects.
uint8_t fnBand(uint16_t c) {
  c = fnSaw(c);
  return c < 240 ? 0 : c;
}

////////////////////////////////////////////////////////////////////////////////
// Animation Functions

// This part defines the strip of LEDs as well as the effects to be applied.

// The strip itself; this object represents the connected LEDs.
// Since we don't specify the pins here the WS2801 library will default to use
// MOSI (Yellow/Data) & CLK (Green/Clock).
Adafruit_WS2801 strip = Adafruit_WS2801(numberOfLEDs);

// Set all bulbs to a uniform color.
void doFill(uint32_t color) {
  for (int i = 0; i < numberOfLEDs; ++i) {
    strip.setPixelColor(i, color);
  }
}

// Walk one bulb along the strip each frame.
void doMarchingBulb(uint32_t colorOn, uint32_t colorOff) {
  uint32_t which = frameCounter % numberOfLEDs;
  for (int i = 0; i < numberOfLEDs; ++i) {
    strip.setPixelColor(i, i == which ? colorOn : colorOff);
  }
}

// Make a horrible mess of strobes.
void doMultiStrobe() {
  for (int i = 0; i < numberOfLEDs; ++i) {
    strip.setPixelColor(i, createColor(
      fnBand(frameCounter + 150 + i * 4),
      fnBand(-frameCounter * 2 + 290 + i * 5),
      fnBand(frameCounter * 5 + 377 + i * 6)));
  }  
}

// Carnival style Red, Green, Blue alternating.
void doRedGreenBlue() {
  const uint32_t colorCount = 3;
  const uint32_t colorValues[colorCount] = { colorRed, colorGreen, colorBlue };
  for (int i = 0; i < numberOfLEDs; ++i) {
    strip.setPixelColor(i, colorValues[(frameCounter + i) % colorCount]);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Entry Point Functions

// The setup is the first function to run when the program starts. This will run
// only once each time the board is first powered up.
void setup() {
  strip.begin();
}

// The loop function runs continuously as fast at the microprocessor will allow
// It is advisable to add a delay in here so it doesn't run crazy fast.
void loop() {
  ////////////////////////////////////////
  // ENABLE ONLY ONE OF THESE:
  ////////////////////////////////////////
  //doMarchingBulb(colorRed, colorBlack);
  doMultiStrobe();
  //doRedGreenBlue();
  //doFill(colorWhite);
  ////////////////////////////////////////
  
  // Update all the LEDs with the last values we set.
  strip.show();
  // Advance to the next frame and delay.  
  ++frameCounter;
  delay(frameDelay);
}

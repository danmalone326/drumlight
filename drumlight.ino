#include <FastLED.h>
#include "DrumSensor.h"
#include "DrumLight.h"
#include "ChristmasLights.h"

boolean debug = false;

#define NUM_LEDS 22
CRGB leds[NUM_LEDS];
const int ledPin = 7;

const int drumSensorPin = 0;

unsigned char drumBrightness = 128;
unsigned char twinkleBrightness = 60;
unsigned long millisLastTap = 0;

// after X milli-seconds of no drum taps, switch to twinkle
unsigned long millisToIdle = 5000;

// states
// 0 = drum
// 1 = twinkle
unsigned char state = 0;

// AGHS Colors
//CRGB colors[] = { 0x0000FF, 0xFFD700 };

// Christmas Colors
CRGB colors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00 };

// Valentines Colors
//CRGB colors[] = { 0xCE4444, 0x65015C, 0xFF7BD2, 0x65015C };

// Cal Poly Colors
//CRGB calpolyColors[] = { 0x00FF00, 0xFFFF00 };

unsigned char numColors = sizeof(colors) / sizeof(CRGB);
unsigned char currentColorIndex = numColors - 1;

DrumSensor myDrumSensor(drumSensorPin);
DrumLight  myDrumLight(&leds[0], NUM_LEDS, 1);
ChristmasLights  myChristmasLights(&leds[0], NUM_LEDS,
                                   &colors[0], numColors);

void myTapBegin() {
  if (debug) {
    Serial.println("Tap Begin");
  }
  myDrumLight.setNextColor(nextColor());
  myDrumLight.tapBegin();
  millisLastTap = millis();
}

void myTapEnd() {
  if (debug) {
    Serial.println("Tap End");
  }
  myDrumLight.tapEnd();
}

CRGB nextColor() {
  currentColorIndex = (currentColorIndex + 1) % numColors;
  return colors[currentColorIndex];
}


void setup()
{
  if (debug) {
    Serial.begin(57600);  // Initializing the serial port at 9600 baud
    Serial.println("DEBUG ON");
  }

  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.setBrightness(drumBrightness);
  FastLED.show();

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  myDrumSensor.attachTapBegin(myTapBegin);
  myDrumSensor.attachTapEnd(myTapEnd);

  FastLED.show();
}

void loop()
{
  myDrumSensor.loop();

  if (state == 0) {
    myDrumLight.loop();
    if (millis() - millisLastTap > millisToIdle) {
      state = 1;
      myChristmasLights.restart();
      FastLED.setBrightness(twinkleBrightness);
    }
  } else if (state == 1) {
    random16_add_entropy(analogRead(drumSensorPin));
    myChristmasLights.loop();
    if (millis() - millisLastTap < millisToIdle) {
      state = 0;
      myChristmasLights.clear();
      FastLED.setBrightness(drumBrightness);
    }
  }

  FastLED.show();
}

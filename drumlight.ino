#include <FastLED.h>
#include "DrumSensor.h"
#include "DrumLight.h"

boolean debug = true;

#define NUM_LEDS 22
CRGB leds[NUM_LEDS];
const int ledPin = 7;

const int drumSensorPin = 0;

unsigned char brightness = 255;

CRGB agColors[] = { 0x0000FF, 0xFFD700 };
CRGB christmasColors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00 };
CRGB valentinesColors[] = { 0xCE4444, 0x65015C, 0xFF7BD2, 0x65015C };
CRGB calpolyColors[] = { 0x00FF00, 0xFFFF00 };

DrumSensor myDrumSensor(drumSensorPin);
DrumLight  myDrumLight(&leds[0], NUM_LEDS,
                       &christmasColors[0],
                       sizeof(christmasColors) / sizeof(CRGB),
                       1);

void myTapBegin() {
  if (debug) {
    Serial.println("Tap Begin");
  }
  myDrumLight.tapBegin();
}

void myTapEnd() {
  if (debug) {
    Serial.println("Tap End");
  }
  myDrumLight.tapEnd();
}

void setup()
{
  if (debug) {
    Serial.begin(57600);  // Initializing the serial port at 9600 baud
    Serial.println("DEBUG ON");
  }

  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.setBrightness(brightness);
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
  myDrumLight.loop();

  FastLED.show();
}

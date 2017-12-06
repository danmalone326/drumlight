#include <FastLED.h>
#include "DrumSensor.h"
#include "DrumLight.h"
#include "ChristmasLights.h"

boolean debug = false;
unsigned long debugCounter = 0;


// for Snare
//#define NUM_LEDS 22

// for Tenor
#define NUM_LEDS 51

CRGB leds[NUM_LEDS];
const int ledPin = 7;

// Analog Pin for the first drum sensor
#define drumSensorPin0 0
#define drumStartLed0 0
#define drumNumLeds0 15

// Analog Pin for the second drum sensor
#define drumSensorPin1 1
#define drumStartLed1 15
#define drumNumLeds1 8

// Analog Pin for the third drum sensor
#define drumSensorPin2 2
#define drumStartLed2 23
#define drumNumLeds2 12

// Analog Pin for the fourth drum sensor
#define drumSensorPin3 3
#define drumStartLed3 35
#define drumNumLeds3 16



unsigned char drumBrightness = 90;
unsigned char twinkleBrightness = 90;
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

#ifdef drumSensorPin0
DrumSensor drumSensor0(drumSensorPin0);
DrumLight  drumLight0(&leds[drumStartLed0], drumNumLeds0, 1);

void tapBegin0() {
  drumLight0.setNextColor(nextColor());
  drumLight0.tapBegin();
  millisLastTap = millis();
}

void tapEnd0() {
  drumLight0.tapEnd();
}
#endif

#ifdef drumSensorPin1
DrumSensor drumSensor1(drumSensorPin1);
DrumLight  drumLight1(&leds[drumStartLed1], drumNumLeds1, 1);

void tapBegin1() {
  drumLight1.setNextColor(nextColor());
  drumLight1.tapBegin();
  millisLastTap = millis();
}

void tapEnd1() {
  drumLight1.tapEnd();
}
#endif

#ifdef drumSensorPin2
DrumSensor drumSensor2(drumSensorPin2);
DrumLight  drumLight2(&leds[drumStartLed2], drumNumLeds2, 1);

void tapBegin2() {
  drumLight2.setNextColor(nextColor());
  drumLight2.tapBegin();
  millisLastTap = millis();
}

void tapEnd2() {
  drumLight2.tapEnd();
}
#endif

#ifdef drumSensorPin3
DrumSensor drumSensor3(drumSensorPin3);
DrumLight  drumLight3(&leds[drumStartLed3], drumNumLeds3, 1);

void tapBegin3() {
  drumLight3.setNextColor(nextColor());
  drumLight3.tapBegin();
  millisLastTap = millis();
}

void tapEnd3() {
  drumLight3.tapEnd();
}
#endif

ChristmasLights  myChristmasLights(&leds[0], NUM_LEDS,
                                   &colors[0], numColors);

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

#ifdef drumSensorPin0
  drumSensor0.attachTapBegin(tapBegin0);
  drumSensor0.attachTapEnd(tapEnd0);
#endif

#ifdef drumSensorPin1
  drumSensor1.attachTapBegin(tapBegin1);
  drumSensor1.attachTapEnd(tapEnd1);
#endif

#ifdef drumSensorPin2
  drumSensor2.attachTapBegin(tapBegin2);
  drumSensor2.attachTapEnd(tapEnd2);
#endif

#ifdef drumSensorPin3
  drumSensor3.attachTapBegin(tapBegin3);
  drumSensor3.attachTapEnd(tapEnd3);
#endif

  FastLED.show();
}

void loop()
{

#ifdef drumSensorPin0
  drumSensor0.loop();
#endif

#ifdef drumSensorPin1
  drumSensor1.loop();
#endif

#ifdef drumSensorPin2
  drumSensor2.loop();
#endif

#ifdef drumSensorPin3
  drumSensor3.loop();
#endif

  if (state == 0) {
#ifdef drumSensorPin0
    drumLight0.loop();
#endif

#ifdef drumSensorPin1
    drumLight1.loop();
#endif

#ifdef drumSensorPin2
    drumLight2.loop();
#endif

#ifdef drumSensorPin3
    drumLight3.loop();
#endif

    if (millis() - millisLastTap > millisToIdle) {
      state = 1;
      myChristmasLights.restart();
      FastLED.setBrightness(twinkleBrightness);
    }
  } else if (state == 1) {
    myChristmasLights.loop();
    if (millis() - millisLastTap < millisToIdle) {
      state = 0;
      myChristmasLights.clear();
      FastLED.setBrightness(drumBrightness);
    }
  }

  FastLED.show();
}

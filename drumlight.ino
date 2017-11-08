#include <FastLED.h>
boolean debug = false;

#define NUM_LEDS 8
CRGB leds[NUM_LEDS];
const int ledPin = 7;

#define DRUM1 0    // Our analog pin

unsigned char brightness = 60;

unsigned int lastColor = 1;
boolean lightOn = false;
CRGB color1 = 0x0000FF;
CRGB color2 = 0xFFD700;
unsigned int drumThreshold = 75;
unsigned long drumMinBetween = 60;

int val = 0;      // Initializing the variable for the voltage value
unsigned int count = 0;
unsigned long lastDrum = 0;


class lightBar {
  public:
    CRGB *leds;
    const unsigned int numLEDs = 8;
    char relativeBrightness = 255;

    const unsigned int numBuffers = 8;
    CRGB *ledBuffer[8];

    void copyToDisplayBuffer(unsigned int buffer) {
      memcpy(leds, ledBuffer[buffer], sizeof(CRGB)*numLEDs);
    }

    void copyFromDisplayBuffer(unsigned int buffer) {
      memcpy(ledBuffer[buffer], leds, sizeof(CRGB)*numLEDs);
    }

    void copyBuffer(unsigned int fromBuffer, unsigned int toBuffer) {
      memcpy(ledBuffer[fromBuffer], ledBuffer[toBuffer], sizeof(CRGB)*numLEDs);
    }

    void setBuffer(unsigned int buffer, CRGB color) {
      CRGB *ledPtr = ledBuffer[buffer];
      int pos;

      for (pos = 0; pos < numLEDs; pos++) {
        *(ledPtr + pos) = color;
      }
    }

    void initBuffers() {
      unsigned int counter;

      for (counter = 0; counter < numBuffers; counter++) {
        ledBuffer[counter] = (CRGB*)malloc(sizeof(CRGB) * numLEDs);
        setBuffer(counter, 0x000000);
      }
    }

    void setLED(unsigned int buffer, unsigned int pos, CRGB color) {
      if (pos < numLEDs) {
        CRGB *ledPtr = ledBuffer[buffer];
        *(ledPtr + pos) = color;
      }
      else {
        setBuffer(buffer, color);
      }
    }

    void setAll(CRGB color) {
      int counter;
      for (counter = 0; counter < numLEDs; counter++) {
        leds[counter] = color;
      }
    }

    //    void standardFlash(unsigned int onBuffer, unsigned int offBuffer, unsigned int onDuration) {
    //      copyToDisplayBuffer(onBuffer);
    //      show();
    //      delay(onDuration);
    //
    //      copyToDisplayBuffer(offBuffer);
    //      show();
    //    }
    //
    //    void multiFlash(unsigned int onBuffer, unsigned int offBuffer, unsigned int numFlashes, unsigned int onDuration, unsigned int betweenDuration) {
    //      int counter;
    //
    //      for (counter = 0; counter < numFlashes; counter++) {
    //        standardFlash(onBuffer, offBuffer, onDuration);
    //        delay(betweenDuration);
    //      }
    //    }

    void off () {
      setAll(0x000000);
    }

    lightBar (struct CRGB *ledArray) {
      leds = ledArray;
      initBuffers();
    }

};

class drumLightbar {
  public:
    unsigned int numLeds = 8;
    CRGB *leds;
    CRGB color = 0x000000;

    const unsigned long millisOn = 60;
    const unsigned long millisFastOff = 60;
    const unsigned long millisSlowOff = 250;

    const float dimSlowStartPercent = 0.25;

    unsigned long millisLastOn = 0;
    unsigned long millisLastOff = 0;
    boolean isOn = false;

    boolean allOff = true;

    unsigned long timer = 0;
    boolean state = false;
    unsigned long mod = 0;

    drumLightbar (struct CRGB *leds, unsigned int numLeds) {
      this->leds = leds;
      this->numLeds = numLeds;
    }

    void setColor(CRGB color) {
      this->color = color;
    }

    void setAll(float percentOn) {
      int counter;
      CRGB currentDim;

      for (counter = 0; counter < numLeds; counter++) {
        currentDim = color;
        currentDim.fadeLightBy(255 - int(255 * percentOn));
        leds[counter] = currentDim;
      }
      FastLED.show();
    }

    void setLEDs(float percentOn) {
      int counter;
      float currentPosition = (percentOn * numLeds) - 1.0;
      float currentDimPercent;
      CRGB currentDim;

      for (counter = 0; counter < numLeds; counter++) {
        currentDim = color;
        if (counter > currentPosition) {
          //      currentDim = 0xFF0000;
          currentDimPercent = 1 / pow(4, counter - currentPosition);
          if (! isOn && (currentDimPercent < dimSlowStartPercent)) {
            currentDimPercent = dimSlowStartPercent;
          }
          currentDim.fadeLightBy(255 - int(255 * currentDimPercent));
        }
        leds[counter] = currentDim;
      }
      FastLED.show();
    }

    void on() {
      unsigned long millisCurrent = millis();
      float percentOn = 0.0;

      if (! isOn) {
        isOn = true;
        allOff = false;
        if (millisCurrent > (millisLastOff + millisFastOff)) {
          millisLastOn = millisCurrent;
        }
        else {
          percentOn = 1.0 - ( (float)(millisCurrent - millisLastOff) / (float)millisFastOff );
          millisLastOn = millisCurrent - (percentOn * millisOn );
        }
      }

      if ((millisCurrent - millisLastOn) > millisOn) {
        percentOn = (float)(millisCurrent - millisLastOn) / (float)millisOn;
        setLEDs(percentOn);
      }
    }

    void off() {
      unsigned long millisCurrent = millis();
      float percentOn = 0.0;

      if (isOn) {
        isOn = false;
        if (millisCurrent > (millisLastOn + millisOn)) {
          millisLastOff = millisCurrent;
        }
        else {
          percentOn = (float)(millisCurrent - millisLastOn) / (float)millisFastOff;
          millisLastOff = millisCurrent - ((1.0 - percentOn) * millisFastOff );
        }
      }

      if ((millisCurrent - millisLastOff) >= (millisFastOff + millisSlowOff)) {
        if (!allOff) {
          setAll(0);
          allOff = true;
        }
      }
      else if ((millisCurrent - millisLastOff) <= millisFastOff) {
        percentOn = 1.0 - ((float)(millisCurrent - millisLastOff) / (float)millisFastOff);
        setLEDs(percentOn);
      }
      else {
        percentOn = dimSlowStartPercent * (1.0 - ((float)(millisCurrent - (millisLastOff + millisFastOff)) / (float)millisSlowOff));
        setAll(percentOn);
      }
    }
};

lightBar myLightBar(&leds[0]);
drumLightbar myDrumLightbar(&leds[0],NUM_LEDS);

void setup()
{
  if (debug) {
    Serial.println("DEBUG ON");
  }

  Serial.begin(9600);  // Initializing the serial port at 9600 baud

  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.setBrightness(brightness);
  FastLED.show();

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  myLightBar.setAll(0x00FF00);
  FastLED.show();
}

void loop()
{
  CRGB currentColor = 0x000000;
  val = analogRead(DRUM1);  // Read the voltage

  if ((val > drumThreshold) && (!lightOn)) {
    lightOn = true;
    if (lastColor == 2) {
      currentColor = color1;
      myDrumLightbar.setColor(currentColor);
      lastColor = 1;
    }
    else if (lastColor == 1) {
      currentColor = color2;
      myDrumLightbar.setColor(currentColor);
      lastColor = 2;
    }
//    myLightBar.setAll(currentColor);
//    FastLED.show();
    if (debug) {
      Serial.print("O");
    }
    lastDrum = millis();
  }

  if ((val <= drumThreshold) && (lightOn) && (lastDrum + drumMinBetween < millis())) {
    lightOn = false;
//    myLightBar.setAll(0x000000);
//    FastLED.show();
    if (debug) {
      Serial.print("X");
    }
  }

  if (lightOn) {
    myDrumLightbar.on();
  } else {
    myDrumLightbar.off();
  }

  if (val > 1) {
    Serial.print(val);        // Print the voltage to the terminal
    Serial.print("  ");
    if (count++ > 10) {
      Serial.println("");
      count = 0;
    }
    if (debug) {
      Serial.print("o");
    }
  } else if (lastDrum + 2000 < millis()) {
    Serial.println(" ");
    lastDrum = millis() + 10000;
    if (debug) {
      Serial.print("x");
    }
  }
}

#include <FastLED.h>
boolean debug = false;

#define NUM_LEDS 22
CRGB leds[NUM_LEDS];
const int ledPin = 7;

#define DRUM1 0    // Our analog pin

unsigned char brightness = 255;

boolean lightOn = false;

unsigned int drumThreshold = 75;
unsigned long drumMinBetween = 60;

int val = 0;      // Initializing the variable for the drum sensor
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
    CRGB *leds;
    unsigned int numLeds = 0;
    CRGB *colors;
    unsigned int numColors = 0;

    unsigned char currentColorIndex = 0;
    CRGB currentColor;

    //    CRGB color = 0x000000;

    const unsigned long millisOn = 60;
    const unsigned long millisFastOff = 160;
    const unsigned long millisSlowOff = 1000;

    const float dimSlowStartPercent = 0.25;

    unsigned long millisLastOn = 0;
    unsigned long millisLastOff = 0;
    boolean isOn = false;

    boolean allOff = true;

    drumLightbar (struct CRGB *leds, unsigned int numLeds,
                  struct CRGB *colors, unsigned int numColors) {
      this->leds = leds;
      this->numLeds = numLeds;
      this->colors = colors;
      this->numColors = numColors;
      this->currentColorIndex = numColors - 1;
    }

    void nextColor() {
      currentColorIndex = (currentColorIndex + 1) % numColors;
      currentColor = colors[currentColorIndex];
    }

    void setAll(float percentOn) {
      int counter;
      CRGB currentDim;

      for (counter = 0; counter < numLeds; counter++) {
        currentDim = currentColor;
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
        currentDim = currentColor;
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
        nextColor();
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

//CRGB color1 = 0x0000FF;
//CRGB color2 = 0xFFD700;
CRGB agColors[] = { 0x0000FF, 0xFFD700 };
CRGB christmasColors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF };
CRGB valentinesColors[] = { 0xCE4444, 0x65015C, 0xFF7BD2, 0x65015C };
drumLightbar myDrumLightbar(&leds[0], NUM_LEDS,
                            &agColors[0],
                            sizeof(agColors) / sizeof(CRGB));

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
  val = analogRead(DRUM1);  // Read the voltage

  if ((val > drumThreshold) && (!lightOn)) {
    lightOn = true;
    if (debug) {
      Serial.print("O");
    }
    lastDrum = millis();
  }

  if ((val <= drumThreshold) && (lightOn) && (lastDrum + drumMinBetween < millis())) {
    lightOn = false;
    if (debug) {
      Serial.print("X");
    }
  }

  if (lightOn) {
    myDrumLightbar.on();
  } else {
    myDrumLightbar.off();
  }

  if (debug) {
    if (val > 5) {
      Serial.print(val);        // Print the voltage to the terminal
      Serial.print("  ");
      if (count++ > 10) {
        Serial.println("");
        count = 0;
      }
    } else if (lastDrum + 2000 < millis()) {
      Serial.println(" ");
      lastDrum = millis() + 10000;
    }
  }
}

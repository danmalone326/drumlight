// -----
// DrumLight.h - Library for controlling lights around a drum
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/08/2017 transformed into a library
// -----

#include "DrumLight.h"


DrumLight::DrumLight (struct CRGB *_leds, unsigned int _numLeds,
                      struct CRGB *_colors, unsigned int _numColors) {
  leds = _leds;
  numLeds = _numLeds;
  colors = _colors;
  numColors = _numColors;
  currentColorIndex = _numColors - 1;
}

void DrumLight::nextColor() {
  currentColorIndex = (currentColorIndex + 1) % numColors;
  currentColor = colors[currentColorIndex];
}

void DrumLight::setAll(float percentOn) {
  int counter;
  CRGB currentDim;

  for (counter = 0; counter < numLeds; counter++) {
    currentDim = currentColor;
    currentDim.fadeLightBy(255 - int(255 * percentOn));
    leds[counter] = currentDim;
  }
}

void DrumLight::setLEDs(float percentOn) {
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
}

void DrumLight::on() {
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

void DrumLight::off() {
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
      state = 0;
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

void DrumLight::tapBegin () {
  state = 1;
}

void DrumLight::tapEnd () {
  state = 2;
}

void DrumLight::loop() {
  if (state == 1) {
    on();
  } else if (state == 2) {
    off();
  }
}



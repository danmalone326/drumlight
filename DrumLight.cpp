// -----
// DrumLight.cpp - Library for controlling lights around a drum
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/08/2017 transformed into a library
// -----

#include "DrumLight.h"


DrumLight::DrumLight (struct CRGB *_leds, unsigned int _numLeds,
                      unsigned int _everyX = 1) {
  leds = _leds;
  numLeds = _numLeds;
  everyX = _everyX;

  state = (unsigned int *)malloc(sizeof(unsigned int) * _everyX);
  millisLastOn = (unsigned long *)malloc(sizeof(unsigned long) * _everyX);
  millisLastOff = (unsigned long *)malloc(sizeof(unsigned long) * _everyX);
  currentColor = (CRGB *)malloc(sizeof(CRGB) * _everyX);

  unsigned char xIndex;
  for (xIndex = 0; xIndex < everyX; xIndex++) {
    state[xIndex] = 0;
    millisLastOn[xIndex] = 0;
    millisLastOff[xIndex] = 0;
    currentColor[xIndex] = 0x000000;
  }
}

void DrumLight::setNextColor(CRGB _color) {
  nextColor = _color;
}

void DrumLight::setAll(float percentOn, unsigned char xIndex = 0) {
  int counter;
  CRGB currentDim;

  currentDim = currentColor[xIndex];
  currentDim.fadeLightBy(255 - int(255 * percentOn));
  for (counter = xIndex; counter < numLeds; counter += everyX) {
    leds[counter] = currentDim;
  }
}

void DrumLight::wipeLEDs(float percentOn, unsigned char xIndex = 0) {
  int counter;
  float currentPosition = (percentOn * numLeds) - 1.0;
  float currentDimPercent;
  CRGB currentDim;

  for (counter = xIndex; counter < numLeds; counter += everyX) {
    currentDim = currentColor[xIndex];
    if (counter > currentPosition) {
      currentDimPercent = 1 / pow(4, counter - currentPosition);
      if ((state[xIndex] == 4) && (currentDimPercent < dimSlowStartPercent)) {
        currentDimPercent = dimSlowStartPercent;
      }
      currentDim.fadeLightBy(255 - int(255 * currentDimPercent));
    }
    leds[counter] = currentDim;
  }
}

void DrumLight::wipeLoop(unsigned char xIndex = 0) {
  unsigned long millisCurrent = millis();
  float percentOn = 0.0;

  if (state[xIndex] == 1) {
    state[xIndex] = 2;
    currentColor[xIndex] = nextColor;

    if (millisCurrent > (millisLastOff[xIndex] + (millisFastOff + millisSlowOff))) {  // last flash is already off
      millisLastOn[xIndex] = millisCurrent;
    }
    else if (millisCurrent > (millisLastOff[xIndex] + millisFastOff )) {  // in the slow off part
      percentOn = dimSlowStartPercent - (dimSlowStartPercent * (float)(millisCurrent - millisLastOff[xIndex] - millisFastOff) / (float) millisSlowOff );
      millisLastOn[xIndex] = millisCurrent - (percentOn * millisOn );

    } else {  // in the fast off part
      percentOn = 1.0 - ((1.0 - dimSlowStartPercent) * (float)(millisCurrent - millisLastOff[xIndex]) / (float)millisFastOff );
      millisLastOn[xIndex] = millisCurrent - (percentOn * millisOn );
    }
  }

  if (state[xIndex] == 2) {

    if ((millisCurrent - millisLastOn[xIndex]) < millisOn) {
      percentOn = (float)(millisCurrent - millisLastOn[xIndex]) / (float)millisOn;
    } else {
      percentOn = 1.0;
      //      state[xIndex] = 3;
    }
    wipeLEDs(percentOn, xIndex);
  }

  if (state[xIndex] == 3) {
    state[xIndex] = 4;
    if (millisCurrent > (millisLastOn[xIndex] + millisOn)) {
      millisLastOff[xIndex] = millisCurrent;
    }
    else {
      percentOn = (float)(millisCurrent - millisLastOn[xIndex]) / (float)millisOn;
      if (percentOn <= dimSlowStartPercent) { // in the slow off part
        millisLastOff[xIndex] = millisCurrent - millisFastOff - (((dimSlowStartPercent - percentOn) / dimSlowStartPercent) * millisSlowOff );
      } else {  // in the fast part
        millisLastOff[xIndex] = millisCurrent - (millisFastOff - ((percentOn - dimSlowStartPercent) / (1.0 - dimSlowStartPercent) * millisFastOff ));
      }
    }
  }

  if (state[xIndex] == 4) {
    if ((millisCurrent - millisLastOff[xIndex]) >= (millisFastOff + millisSlowOff)) {
      if (state != 0) {
        setAll(0, xIndex);
        state[xIndex] = 0;
      }
    }
    else if ((millisCurrent - millisLastOff[xIndex]) <= millisFastOff) {
      percentOn = 1.0 - ((1.0 - dimSlowStartPercent) * (millisCurrent - millisLastOff[xIndex]) / (float)millisFastOff);
      wipeLEDs(percentOn, xIndex);
    }
    else {
      percentOn = dimSlowStartPercent * (1.0 - ((float)(millisCurrent - (millisLastOff[xIndex] + millisFastOff)) / (float)millisSlowOff));
      setAll(percentOn, xIndex);
    }
  }
}

void DrumLight::tapBegin () {
  currentXIndex = (currentXIndex + 1) % everyX;
  state[currentXIndex] = 1;
}

void DrumLight::tapEnd () {
  state[currentXIndex] = 3;
}

void DrumLight::loop() {
  unsigned char xIndex;
  for (xIndex = 0; xIndex < everyX; xIndex++) {
    wipeLoop(xIndex);
  }
}



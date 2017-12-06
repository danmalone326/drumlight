// -----
// ChristmasLights.cpp - Library for controlling Christmas lights
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/15/2017 Initial code
// -----

#include "ChristmasLights.h"

ChristmasLights::ChristmasLights(struct CRGB *_leds, unsigned int _numLeds,
                                 struct CRGB *_colors, unsigned int _numColors) {
  leds = _leds;
  numLeds = _numLeds;
  colors = _colors;
  numColors = _numColors;
  state =  (unsigned char *)malloc(sizeof(unsigned char) * _numLeds);
  millisLastChange = (unsigned long *)malloc(sizeof(unsigned long) * _numLeds);
  millisUntilChange = (unsigned int *)malloc(sizeof(unsigned int) * _numLeds);

  restart();
}

void ChristmasLights::setBlinkThreshold(uint16_t _threshold) {
  blinkThreshold = _threshold;
}

void ChristmasLights::restart() {
  unsigned char counter;
  unsigned long currentMillis = millis();
  for (counter = 0; counter < numLeds; counter++) {
    state[counter] = 2;
    millisLastChange[counter] = currentMillis;
    millisUntilChange[counter] = millisRestart;
  }
}

void ChristmasLights::clear() {
  unsigned char counter;
  for (counter = 0; counter < numLeds; counter++) {
    leds[counter] = 0x000000;
  }
}

void ChristmasLights::loop() {
  unsigned char counter;
  float percentOn;
  CRGB tempColor;
  unsigned long currentMillis = millis();
  unsigned long delta;

//  if (scrollState == 0) {
//    if (currentMillis > millisStateStart + millisStateChange) {
//      scrollState = 1;
//      millisStateStart = currentMillis;
//      millisStateChange = 15000;
//    }
//  } else if (scrollState == 1) {
//    scrollState = 2;
//  } else if (scrollState == 2) {
//    if (currentMillis > millisStateStart + millisStateChange) {
//      scrollState = 0;
//      millisStateStart = currentMillis;
//      millisStateChange = 2000;
//    }
//    if (currentMillis > (millisScrollChange + (millisScrollRate / numColors))) {
//      scrollState = 1;
//      millisScrollChange = currentMillis;
//      scrollCounter = (scrollCounter + 1) % numColors;
//    }
//  }

  for (counter = 0; counter < numLeds; counter++) {
    if (scrollState == 0) {
      if (state[counter] == 0) {
        percentOn = 1.0;
        if (random16() < blinkThreshold) {
          state[counter] = 1;
          millisLastChange[counter] = currentMillis;
          millisUntilChange[counter] = millisOff + random16(millisDiff);
        }
      }
    } else if (scrollState == 1) {
      if ((counter % numColors) == scrollCounter) {
        state[counter] = 1;
        millisLastChange[counter] = currentMillis;
        millisUntilChange[counter] = millisScrollRate / 2;
      }
    }

    delta = currentMillis - millisLastChange[counter];
    if (state[counter] == 1) {
      if (delta > millisUntilChange[counter] ) {
        state[counter] = 2;
        percentOn = minBright;
        millisLastChange[counter] = currentMillis;
        millisUntilChange[counter] = millisOn + random16(millisDiff);
      } else {
        percentOn = 1.0 - ((1.0 - minBright) * (float) delta / (float) millisUntilChange[counter]);
      }
    } else if (state[counter] == 2) {
      if (delta > millisUntilChange[counter] ) {
        state[counter] = 0;
        percentOn = 1.0;
        millisLastChange[counter] = currentMillis;
      } else {
        percentOn = minBright + ((1.0 - minBright) * (float) delta / (float) millisUntilChange[counter]);
      }
    }
    if (percentOn > 1.0) {
      percentOn = 1.0;
    }
    tempColor = colors[counter % numColors];
    tempColor.fadeLightBy(255 - int(255 * percentOn));
    leds[counter] = tempColor;
  }
}


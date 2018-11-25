// -----
// ChristmasLights.h - Library for controlling Christmas lights
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/15/2017 Initial code
// -----

#ifndef ChristmasLights_h
#define ChristmasLights_h

#include "Arduino.h"

// This supresses the pragma version warning FASTLED produces
#define FASTLED_INTERNAL
#include <FastLED.h>

class ChristmasLights
{
  public:
    // ----- Constructor -----
    ChristmasLights(struct CRGB *_leds, unsigned int _numLeds,
              struct CRGB *_colors, unsigned int _numColors);

    // ----- Set runtime parameters -----
    void setBlinkThreshold(uint16_t _threshold);

    // ----- State functions -----
    void restart();
    void clear();

    // call this function every some milliseconds for handling color changes/dims.
    void loop(void);

  private:
    CRGB *leds;
    unsigned int numLeds = 0;
    CRGB *colors;
    unsigned int numColors = 0;

    const unsigned long millisRestart = 1500;
    const unsigned long millisOn = 100;
    const unsigned long millisOff = 100;
    const unsigned long millisDiff = 750;
    const float minBright = 0.05;
    uint16_t blinkThreshold = 40;

    unsigned long millisStateStart = 0;
    unsigned long millisStateChange = 5000;
    unsigned long millisScrollChange = 0;
    unsigned long millisScrollRate = 1500;
    unsigned char scrollCounter = numColors;
    unsigned char scrollState = 0;

 
    // States
    // 0 = LED is on full
    // 1 = LED fading down
    // 2 = LED returning to full
    unsigned char *state;
    unsigned long *millisLastChange;
    unsigned int *millisUntilChange;

};

#endif

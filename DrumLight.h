// -----
// DrumLight.h - Library for controlling lights around a drum
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/08/2017 transformed into a library
// -----

#ifndef DrumLight_h
#define DrumLight_h

#include "Arduino.h"
#include <FastLED.h>

class DrumLight
{
  public:
    // ----- Constructor -----
    DrumLight(struct CRGB *_leds, unsigned int _numLeds,
              unsigned int _everyX);

    // ----- Set runtime parameters -----

    void setNextColor(CRGB _color);

    // set # minimum value on the analog input for the start of a stroke/tap.
    // void setDetectLevel(int level);

    // ----- State functions -----

    void tapBegin();
    void tapEnd();

    // call this function every some milliseconds for handling color changes/dims.
    void loop(void);

  private:
    CRGB *leds;
    unsigned int numLeds = 0;

    const unsigned long millisOn = 60;
    const unsigned long millisFastOff = 160;
    const unsigned long millisSlowOff = 1000;
    const float dimSlowStartPercent = 0.25;

    // States
    // 0 = All LEDs are off
    // 1 = tap just started
    // 2 = tap started, has not finished yet, LEDs getting brighter
    // 3 = tap just finished
    // 4 = tap finished, LEDs still fading
    unsigned int *state;
    unsigned long *millisLastOn;
    unsigned long *millisLastOff;
    CRGB *currentColor;

    unsigned int everyX = 1;
    unsigned char currentXIndex = 0;
    CRGB nextColor = 0x000000;

    void setAll(float percentOn, unsigned char xIndex = 0);
    void wipeLEDs(float percentOn, unsigned char xIndex = 0);
    void wipeLoop(unsigned char xIndex = 0);

};

#endif

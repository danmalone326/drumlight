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
              struct CRGB *_colors, unsigned int _numColors);

    // ----- Set runtime parameters -----

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
    CRGB *colors;
    unsigned int numColors = 0;

    unsigned char currentColorIndex = 0;
    CRGB currentColor;

    int state = 0;
    
    const unsigned long millisOn = 60;
    const unsigned long millisFastOff = 160;
    const unsigned long millisSlowOff = 1000;

    const float dimSlowStartPercent = 0.25;

    unsigned long millisLastOn = 0;
    unsigned long millisLastOff = 0;
    boolean isOn = false;

    boolean allOff = true;

    void nextColor();
    void setAll(float percentOn);
    void setLEDs(float percentOn);
    void on();
    void off();

};

#endif

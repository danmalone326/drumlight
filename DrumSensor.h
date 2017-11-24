// -----
// DrumSensor.h - Library for detecting a drum stroke/tap using a piezo sensor
// Took my original code and used the model provided by OneButton to make it a module
// OneButton by Matthias Hertel, https://github.com/mathertel/OneButton
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// 11/08/2017 transformed into a library
// -----

#ifndef DrumSensor_h
#define DrumSensor_h

#include "Arduino.h"

// ----- Callback function types -----

extern "C" {
  typedef void (*tapCallbackFunction)(void);
}


class DrumSensor
{
public:
  // ----- Constructor -----
  DrumSensor(int pin);
  
  // ----- Set runtime parameters -----

  // set # minimum value on the analog input for the start of a stroke/tap.
  void setDetectLevel(int level);

  // set # millisec after safe tap is assumed.
  void setDebounceTicks(int ticks);

  // set # millisec required between stroke/taps
  void setBetweenTicks(int ticks);

  // attach functions that will be called at the begin and end of the tap.
  void attachTapBegin(tapCallbackFunction newFunction);
  void attachTapEnd(tapCallbackFunction newFunction);

  // ----- State machine functions -----

  // call this function every some milliseconds for handling button events.
  void loop(void);

private:
  int _pin;              // hardware pin number
  int _detectLevel;      // minimum value on the analog input for the start of a stroke/tap
  int _debounceTicks;    // number of ticks for debounce times.
  int _betweenTicks; // millisec required between stroke/taps
  
  // These variables will hold functions for the events
  tapCallbackFunction _tapBeginFunc;
  tapCallbackFunction _tapEndFunc;

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the tick function is called.
  int _state;
  unsigned long _startTime; // will be set in state 1
  unsigned long _stopTime; // will be set in state 2
};

#endif

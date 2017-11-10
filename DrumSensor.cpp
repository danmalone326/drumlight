// -----
// DrumSensor.h - Library for detecting a drum stroke/tap using a piezo sensor
// Took my original code and used the model provided by OneButton to make it a module
// OneButton by Matthias Hertel, https://github.com/mathertel/OneButton
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dan Malone
// This work is licensed under a BSD style license.
// More information on:
// -----
// Changelog: see OneButton.h
// -----

#include "DrumSensor.h"

// ----- Initialization and Default Values -----

DrumSensor::DrumSensor(int pin)
{
  _pin = pin;

  _detectLevel = 75;        // minimum value of the analog pin indicating a drum tap.
  _debounceTicks = 20;      // number of millisec that have to pass by before a tap is assumed as safe.
  _betweenTicks = 60;   // number of millisec that have to pass between successive drum taps.
 
  _state = 0; // starting with state 0: waiting for button to be pressed

  // no functions attached yet: clear all function pointers.
  _tapBeginFunc = NULL;
  _tapEndFunc = NULL;
} // DrumSensor


// explicitly set the minimum value on the analog input for the start of a stroke/tap.
void DrumSensor::setDetectLevel(int level) { 
  _detectLevel = level;
} // setDetectLevel

// explicitly set the number of millisec that have to pass by before a tap is assumed as safe.
void DrumSensor::setDebounceTicks(int ticks) { 
  _debounceTicks = ticks;
} // setDebounceTicks

// explicitly set the number of millisec required between stroke/taps.
void DrumSensor::setBetweenTicks(int ticks) { 
  _betweenTicks = ticks;
} // setBetweenThreshold


// save function for tap events
void DrumSensor::attachTapBegin(callbackFunction newFunction)
{
  _tapBeginFunc = newFunction;
} // attachTapBegin

void DrumSensor::attachTapEnd(callbackFunction newFunction)
{
  _tapEndFunc = newFunction;
} // attachTapEnd

void DrumSensor::loop(void)
{
  // Detect the input information 
  int sensorLevel = analogRead(_pin); // current button signal.
  unsigned long now = millis(); // current (relative) time in msecs.

  // Implementation of the state machine
  if (_state == 0) { // waiting for tap to start.
    if (sensorLevel >= _detectLevel) {
      _state = 1; // step to state 1
      _startTime = now; // remember starting time
    } // if

  } else if (_state == 1) { // waiting for tap to end.
    if ((sensorLevel < _detectLevel) && ((unsigned long)(now - _startTime) < _debounceTicks)) {
      // button was released to quickly so assume some debouncing.
	  // go back to state 0 without calling a function.
      _state = 0;

    } else if ((unsigned long)(now - _startTime) >= _debounceTicks) {
      if (_tapBeginFunc) _tapBeginFunc();
      _state = 2; // go wait till this tap is done

    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state == 2) { // waiting for level to drop indicating end of tap
    if ((sensorLevel < _detectLevel)) {
      if (_tapEndFunc) _tapEndFunc();
      _state = 3;      // this tap is done
      _stopTime = now; // remember stopping time
    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state == 3) { // waiting for minimum ticks between taps.
    if ((unsigned long)(now - _stopTime) > _betweenTicks) {
      _state = 0; // restart.
    } // if 

  } // if  
} // DrumSensor.loop()


// end.

#include "DFR_LCDKeypad.h"
/*
 * File: DFR_LCDKeypad.cpp
 * Hardware: DFRobot LCD Keypad Shield for Arduino
 * Author: Paul R. Nelson III
 * Version: 1.0
 * License: BSD 2-clause
 * Description:  Library for easy interaction with 'DFRobot LCD Keypad Shield V1.0'
 *   This library works conjunction with the LiquidCrystal library, and adds DFRobot 
 *   specific features, such as backlight control on pin 10.  Readability, functionality
 *   and byte count are all concerns.
 *   Features - buttonUp, buttonDown, buttonHeld callbacks
 *            - setBrightness(byte brightness) function
 */

#define ISRIGHT(value)  (value <   77)
#define ISUP(value)     (value >=  78 && value < 120)
#define ISDOWN(value)   (value >= 236 && value < 300)
#define ISLEFT(value)   (value >= 400 && value < 599)
#define ISSEL(value)    (value >= 600 && value < 887)

DFR_LCDKeypad::DFR_LCDKeypad(byte brightness)
		: _buttonDown(0), _buttonUp(0), _buttonHeld(0), _prevMs(0),
		  _buttonState(eNone), _heldInterval(cDefaultHeldInterval),
		  _brightness(brightness)
{
	// initialize backlight default to full brightness, if no parameter
	pinMode(cBacklightPin, OUTPUT);
	analogWrite(cBacklightPin, _brightness);
}

// Called from within loop() to update button state and dispatch calls to button handler 
// callbacks as necessary.   Conditions checked for are button down, button up and button held
// longer than setable interval.  I believe debounce code is unnecessary because
// the A/D conversion appears to be soaking up any transients.  
// (not entirely true, seeing some bounce if rapidly pressing button like a frantic monkey.)
// Typical use case seemes to be fine.
// 
// _buttonUp() resets _heldInterval to cDefaultHeldInterval. - my usage, saves doing it in
// each client's buttonUp routine(s) using a function call.
// 
// Note: the magic occuring in the first line turns an analog value to a key enumeration,
//   using a degree=1 least squares fit of the midpoint values between the analog key values.
//   this allows creating a reverse mapping of value to key without using a series of 
//   if/then/else statements.  The code squeezes and additional 70 bytes out of the 
//   equivalent if/then/else code at the cost of code clarity.  
//   For full discussion, see ReadMe.txt
void
DFR_LCDKeypad::update() 
{	
	// int kv = ((analogRead(cKeyPin) + 71) * 6) >> 10;   // MAGIC NUMBERS see ReadMe.txt
	// eDFRKey keyRead = (kv < eNone) ? eDFRKey(kv) : eNone;
	int value = analogRead(cKeyPin);
	// Serial.println(value);
	eDFRKey keyRead = eNone;
	if (     ISRIGHT(value) ) keyRead = eRight;
	else if ( ISLEFT(value) ) keyRead = eLeft;
	else if ( ISUP  (value) ) keyRead = eUp;
	else if ( ISDOWN(value) ) keyRead = eDown;
	else if ( ISSEL (value) ) keyRead = eSelect;

    // if button state differs from previous state, call appropriate callback function
	if( keyRead != _buttonState) {
		// accept the extra overhead of a temporary for previous state so that 
		// client code can safely call getState() and have current button status.
		eDFRKey prevKey = _buttonState;
		_buttonState = keyRead;
		if( prevKey == eNone ) {
			if(_buttonDown)
				(*_buttonDown)(keyRead);	// call button down callback
		} else {
			if(_buttonUp) {
				_heldInterval = cDefaultHeldInterval;
				(*_buttonUp)(prevKey);		// call button up callback
			}
		}

	} else {
		if(_buttonState != eNone && _buttonHeld) {
			// button held down - check update interval
			unsigned long current = millis();
			if(current - _prevMs > _heldInterval) {
				_prevMs = current;
				(*_buttonHeld)(keyRead);	// call button held callback
			}
		}
		return;
	}
}







#if !defined( DFR_LCDKEYPAD_H )
#define DFR_LCDKEYPAD_H
#include "Arduino.h"
/*
 * File: DFR_LCDKeypad.h
 * Hardware: DFRobot LCD Keypad Shield for Arduino
 * Author: Paul R. Nelson III
 * Version: 1.0
 * License: BSD 2-clause
 * Description:  Library for easy interaction with 'DFRobot LCD Keypad Shield V1.0'
 *   This library works conjunction with the LiquidCrystal library, and adds DFRobot 
 *   specific features, such as backlight control on pin 10.
 *   Features - buttonUp, buttonDown, buttonHeld callbacks
 *            - setBrightness(byte brightness) function
 */
// Order is specific, since it matches Analog Key value order
typedef enum {
	eRight = 0,
	eUp,
	eDown,
	eLeft,
	eSelect,
	eNone
} eDFRKey;

const int cKeyPin(0);
const int cBacklightPin(10);
const int cDebounceDelay(50);
const unsigned int cDefaultHeldInterval(250);

class DFR_LCDKeypad
{
	void (*_buttonDown)(eDFRKey button);
	void   (*_buttonUp)(eDFRKey button);
	void (*_buttonHeld)(eDFRKey button);
	unsigned long _prevMs;
	unsigned int  _heldInterval;
	eDFRKey       _buttonState;
	byte          _brightness;
	
public:
	DFR_LCDKeypad(byte brightness=255);

	void update();
	inline eDFRKey getState() {return _buttonState;}
	inline void setHeldInterval( unsigned int interval) { _heldInterval = interval; }
	inline void setButtonUpHandler( void (*func)(eDFRKey) )   { _buttonUp = func; }
	inline void setButtonDownHandler( void (*func)(eDFRKey) ) { _buttonDown = func; }
	inline void setButtonHeldHandler( void (*func)(eDFRKey) ) { _buttonHeld = func; }
	inline byte getBrightness() {return _brightness;}
	void setBrightness(byte brightness) {
		_brightness = brightness;
		analogWrite(cBacklightPin, _brightness); 
	}
};

#endif

ReadMe.txt

Support for DFRobotics LCD Keypad Shield V1.0
Author: Paul R. Nelson III
Version: 1.0
License: BSD 2-clause

None of the sample code I found exercised the display the way I 
wanted.  I have added backlight control, and event driven callbacks on
button presses.  The user program callback functions are triggered
when a button is pressed, released or held for longer than a settable
interval.  The update handlers can be changed at run time, which is
how the demo program changes modes on the select button.  Would be
good for different menus.

Significant optimization has been done to squeeze out as many bytes as
I could find, to make the library as low overhead as possible.  In the
case of mapping analog key value to key press enumeration, it
significantly impacts code clarity.  To squeeze an additional 70 bytes
out of the code, the following if/then/else code was replaced with an
equation.

// Values are midpoints between analog value of key press.
#define ISRIGHT(value)  (value <   77)
#define ISUP(value)     (value >=  78 && value < 236)
#define ISDOWN(value)   (value >= 236 && value < 441)
#define ISLEFT(value)   (value >= 441 && value < 622)
#define ISSEL(value)    (value >= 622 && value < 887)

::update() {
	int value = analogRead(cKeyPin);
	eDFRKey keyRead = eNone;
	if (     ISRIGHT(value) ) keyRead = eRight;
	else if ( ISLEFT(value) ) keyRead = eLeft;
	else if ( ISUP  (value) ) keyRead = eUp;
	else if ( ISDOWN(value) ) keyRead = eDown;
	else if ( ISSEL (value) ) keyRead = eSelect;
...
}

All the above code is replaced with the equivalent:

::update() {
	int kv = ((analogRead(cKeyPin) + 71) * 6) >> 10;   
	eDFRKey keyRead = (kv < eNone) ? eDFRKey(kv) : eNone;
...
}

The Original Analog key down values:
    order = eRight, eUp, eDown, eLeft, eSelect, eNone
    values = { 0, 144, 329, 505, 742, 1023 }

Using a degree=1 least squares fit of midpoint value of analog key down values.
     xs = [ 0, 1, 2, 3, 4, 5 ]
     ys = [ 0, 77, 236, 441, 622, 887 ]
     p = numpy.poly1d(numpy.polyfit(xs,ys,deg=1))

Creates a reverse mapping of value to key without repeated if/then/else statements.

      key = value + 71
            ----------
               179

 int kv = (analogRead(cKeyPin) + 71) / 179; 

This gives a mapping that works robustly across the range [eRight,eSelect].  
Clamp to eNone if key value is eNone or larger.

Turns out that the division is almost as expensive as the series of if/then/else 
since the AVR instruction set does not have division. Only library supported
software routine for division.  So to accomplish the same without using
the division operator.

 		1/179 ~= 6 / 1024  == * 6  >> 10

yeilding the final 

	int kv = ((analogRead(cKeyPin) + 71) * 6) >> 10;

Demo progrogram: 
	 displays initial banner in setup
	 downloads user defined characters to LCD
	 Two Display modes chosen by the selecte button
	 Display Character mode:  (default mode)

	    Left and Right buttons scroll through character set displaying 
	    character and value on the LCD as shown below.
            Display Char: !
            Value: 33
	
		Up and Down buttons raise and lower the backlight brightness

		Holding down a button will cause it to advance automaticially,
		holding longer will cause the advance to occur faster.
 
	Mouse Mode:
		Up/Down/Left/Right moves a blinking block cursor around
		display


Class Usage:

DFR_LCDKeypad keypad;

...

void myButtonUp(eDFRKey key) {
 // do something on key
}

void myButtonHeld(eDFRKey key) {
// do something on key held down
}

...

void setup() {
...
// note: without any handlers it will be a really dull program...
keypad.setButtonUpHandler(myButtonUp);		  // optional 
keypad.setButtonHeldHandler(myButtonHeld);	  // optional
keypad.setButtonHeldHandler(myButtonDown);	  // optional

keypad.setBacklight( some_brightness_value ); // optional


...
}

void loop() {
	 // update current keypress value and call button callbacks if necessary
	 keypad.update();
}

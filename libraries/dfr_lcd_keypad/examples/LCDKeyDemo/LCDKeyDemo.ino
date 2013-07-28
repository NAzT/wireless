#include <LiquidCrystal.h>
#include <DFR_LCDKeypad.h>
/*
 * File: LCDKeyDemo.ino
 * Hardware: DFRobot LCD Keypad Shield for Arduino
 * Author: Paul R. Nelson III
 * Version: 1.0
 * License:  BSD 2-clause
 * Descritpion:  Full-on demo of of 'DFRobot LCD Keypad Shield V1.0'
 *   Features - user defined display characters
 *            - blink
 *            - program controlled backlight brightness
 *            - single press increment/decrement
 *            - key auto-repeat at fixed interval when held down
 *            - display of characters and their values
 *            - mouse-mode - select toggles display vs mouse by changing button callbacks
 *              - moves character block using up/down, left/right
 * 
 */
/* interesting display chars 
 * 126  ->
 * 127  <-
 * 161  small box lower left 3x3 center open
 * 162  left upper box corner 
 * 163  rt   lower box corner
 * 164  lower left corner \
 * 165  center 4 pixel box
 * 186  Left open box
 * 219  Full open box
 * 223  Upper left open 3x3 (degree symbol)
 * 242 omega
 * 243 interestign horizontla box
 * 244 ohm
 * 246 capital Sigma
 * 247 pi
 * 248 x bar - bar over x
 * 253 divide 
 * 255 full box
 */

const byte cBrightness(128);
//Pin assignments for DFRobot LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DFR_LCDKeypad keypad(cBrightness);
byte displayChar(0);
byte brightness(cBrightness);

unsigned long prevTick(0);
unsigned int keyInterval(700);
bool isSolid(true);


byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

byte fwd[8] = {
   B00000,
   B01000,
   B01100,
   B01110,
   B01111,
   B01110,
   B01100,
   B01000
};

byte ch_fwd[8] = {
   B00000,
   B10001,
   B11001,
   B11101,
   B11111,
   B11101,
   B11001,
   B10001
};

byte rev[8] = {
   B00000,
   B00010,
   B00110,
   B01110,
   B11110,
   B01110,
   B00110,
   B00010
};

byte ch_rev[8] = {
   B00000,
   B10001,
   B10011,
   B10111,
   B11111,
   B10111,
   B10011,
   B10001
};

byte pause[8] = {
   B00000,
   B11011,
   B11011,
   B11011,
   B11011,
   B11011,
   B11011,
   B11011
};

byte Stop[8] = {
   B00000,
   B00000,
   B11111,
   B11111,
   B11111,
   B11111,
   B11111,
   B00000
  }; 
       
static int sHeldCount(0);
static byte xPos(0);
static byte yPos(0);
static bool isMouseMode(false);
void mouseBtnUp(eDFRKey);

// Character display is occuring once in setup, then only again in the button routines
// no need to refresh in update loop unless there is a change.  For now just redisplay all 
// text instead of additional logic to update only chagned value and display character.
// it is by far fast enough.
void charDisplay() 
{
    // display characters mode
    lcd.setCursor(0, 0);
    lcd.print("Display Char: ");
    lcd.setCursor(14,0);
    lcd.write(displayChar);
      
    lcd.setCursor(0,1);
    lcd.print("Value: ");
    lcd.print(displayChar);
    lcd.print("  ");  // make sure to cleanup after numerical wrap around
 }

void cursorDraw() 
{
//  lcd.write(254);
  prevTick = millis();
  isSolid = true;
  lcd.setCursor(xPos,yPos);
  lcd.write(255);
}

void buttonUp(eDFRKey key) 
{
  sHeldCount = 0;

  switch(key) {
    case eUp:
      if(brightness < 255) 
        keypad.setBrightness(++brightness);
      break;
      
    case eDown:
      if(brightness > 0) 
        keypad.setBrightness(--brightness);
      break;
      
    case eLeft:
      --displayChar;
      break;
      
    case eRight:
      ++displayChar;
      break;
      
    case eSelect:
      // switch to mouse mode
      isMouseMode = true;
      lcd.clear();
      cursorDraw();
      keypad.setButtonUpHandler(mouseBtnUp);
      keypad.setButtonHeldHandler(0);
      return;
  }
  charDisplay();
}

/*
void buttonDown(eDFRKey key) {
 // lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pressed = ");
  switch(key) {
    case eUp:
      lcd.print("Up");
      break;
      
    case eDown:
      lcd.print("Down");
      break;
      
    case eLeft:
      lcd.print("Left");
      break;
      
    case eRight:
      lcd.print("Right");
      break;
      
    case eSelect:
      lcd.print("Sel");
      break;
  }
}
*/

void buttonHeld(eDFRKey key) {
  sHeldCount++;
  if(sHeldCount > 1) {
    switch(key) {
      case eUp:
        if(brightness < 255)
          keypad.setBrightness(++brightness);
        break;
      
      case eDown:
        if(brightness > 0)
          keypad.setBrightness(--brightness);
        break;
      
      case eLeft:
         --displayChar;
        break;
      
      case eRight:
          ++displayChar;
        break;
      
      case eSelect:
        break;
    }
  }
  // speed things up
  if(sHeldCount > 30) {
    keypad.setHeldInterval(30);
  } else if(sHeldCount > 10) {
    keypad.setHeldInterval(100);
  }
  charDisplay();
}

// clamps to edges of display, rather than wrap
void mouseBtnUp(eDFRKey key) 
{
  lcd.clear();    // fewer bytes than lcd.setCursor(,); lcd.write()

  switch(key) {

    case eUp:
    case eDown:
      yPos = (yPos) ? 0 : 1;   // display is only 2 lines tall
      break;
      
    case eLeft:
      if(xPos > 0) --xPos;
      break;
      
    case eRight:
      if(xPos < 15) ++xPos;
      break;
      
    case eSelect:
      // switch back to display mode
      charDisplay();
      isMouseMode = false;
      keypad.setButtonUpHandler(buttonUp);
      keypad.setButtonHeldHandler(buttonHeld);
      return;
  }
  cursorDraw(); 
}

void setup() 
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCDKeypad v1.0");
  keypad.setButtonUpHandler(buttonUp);
//  keypad.setButtonDownHandler(buttonDown);
  keypad.setButtonHeldHandler(buttonHeld);
   
  lcd.createChar(0, smiley);
  lcd.createChar(1, fwd);
  lcd.createChar(2, rev);
  lcd.createChar(3, pause);
  lcd.createChar(4, Stop);
  lcd.createChar(5, ch_fwd);
  lcd.createChar(6, ch_rev);
//  lcd.createChar(7, );
  delay(2500);
  
  lcd.clear();
  charDisplay();
}

void loop() 
{
  // updates current key press value and calls button callbacks if necessary  
  keypad.update();
  
  unsigned long current = millis();
  
  if(isMouseMode) {  
    // blink full display element at key interval  
    if(current - prevTick > keyInterval) {
      prevTick = current;
      lcd.setCursor(xPos,yPos);
      isSolid = !isSolid;
      lcd.write( isSolid ? 255 : 254 );
    }
  } 
}

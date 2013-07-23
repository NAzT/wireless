#include <LiquidCrystal.h>
#include <DFR_LCDKeypad.h>
#include <VirtualWire.h>
#include <stdio.h>
#include <string.h>
#include "pt.h"
 
static struct pt pt1, pt2;

#define MAX_WORDS 100
#ifndef NULL
#define NULL 0
#endif


// static int thread1( struct pt *pt, long timeout, DFR_LCDKeypad *keypad ) {
//   static long t1 = 0;
//   PT_BEGIN( pt );
//   while(1) {
//     PT_WAIT_UNTIL( pt, (millis() - t1) > timeout );
//         Serial.println("IN THREAD1");
//         (*keypad).update();
//        t1 = millis();
//   }
//   PT_END( pt );
// }
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
// Values are midpoints between analog value of key press.
#define SENSOR_NUM 3
#define SENSOR_TYPE 3
#define MAX_NODE 8

char* sensor_data[SENSOR_TYPE][MAX_NODE] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
}; 

struct SENSOR {
  byte type;
  byte node;
  char* s;
};
const byte cBrightness(128);
//Pin assignments for DFRobot LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DFR_LCDKeypad keypad(cBrightness);
byte displayChar(1);
int vertChar;
// byte brightness(cBrightness);

// unsigned long prevTick(0);
// unsigned int keyInterval(700);
// bool isSolid(true);

static byte xPos(0);
static byte yPos(0);

char *buffer, *words[MAX_WORDS], *aPtr, *bPtr;
int count = 0, i;
// char* printStr;

char* data[10] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };

int fromBinary(char *s) {
  return (int) strtol(s, NULL, 2);
}


// Character display is occuring once in setup, then only again in the button routines
// no need to refresh in update loop unless there is a change.  For now just redisplay all 
// text instead of additional logic to update only chagned value and display character.
// it is by far fast enough.
void charDisplay() {
    // display characters mode
    lcd.setCursor(0, 0);
    // lcd.print("Sensor Type: ");
    lcd.print("Type: ");
    lcd.print(displayChar%SENSOR_NUM);
    lcd.print(" ");
    lcd.print("Node: ");
    lcd.print(vertChar%MAX_NODE);
    // lcd.setCursor(14,0);
    // lcd.write(displayChar);
      
    lcd.setCursor(0,1);
    // lcd.print("Vert: ");
    // lcd.print(vertChar);
    // lcd.print(printStr);
    if (sensor_data[displayChar%SENSOR_NUM][vertChar] == 0) {
      lcd.print("NULL            ");
    }
    else {
      lcd.print(sensor_data[displayChar%SENSOR_NUM][vertChar%MAX_NODE]);
      lcd.print("      ");
    }
    // lcd.print("  ");  // make sure to cleanup after numerical wrap around
 }

void buttonUp(eDFRKey key) {
  Serial.print("KEY: ");
  Serial.print(key);
  // Serial.println(brightness);

  switch(key) {
    case eUp:
      Serial.println("eUp");
      vertChar = abs(--vertChar);
      // if(brightness < 255) 
      //   keypad.setBrightness(++brightness);
      break;
    case eDown:
      ++vertChar;
      Serial.println("eDown");    
      // if(brightness > 0) 
      //   keypad.setBrightness(--brightness);
      break;
      
    case eLeft:
      Serial.println("eLeft");        
      --displayChar;
      vertChar = 0;
      break;
      
    case eRight:
      Serial.println("eRight");        
      vertChar = 0;
      ++displayChar;
      break;
      
    case eSelect:
      Serial.println("eSelect");        
      // switch to mouse mode
      // keypad.setButtonHeldHandler(0);
      return;
  }
  charDisplay();
}


void setup() {
  vertChar = 0;
  Serial.begin(9600);
  // Serial.  println("setup");

  // Initialise the IO and ISR
  vw_set_rx_pin(13);    
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running  
  
  //// LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCDKeypad v1.0");
  keypad.setButtonUpHandler(buttonUp);
//  keypad.setButtonDownHandler(buttonDown);
  // keypad.setButtonHeldHandler(buttonHeld);
   
  delay(250);
  
  lcd.clear();
  charDisplay();
}

void loop() {
    keypad.update();
    // thread1( &pt1, 2000, &keypad);
    count = 0;
  // updates current key press value and calls button callbacks if necessary  
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    // Serial.println("1. WAITING...");
    // vw_wait_rx();
    // Serial.println("2. ### GOT IT ###");
    if (vw_get_message(buf, &buflen)) // Non-blocking    
    {
        // digitalWrite(13, true); // Flash a light to show received good message
        Serial.println((char*) buf);
        // memcpy(buffer, (char*)buf, buflen);
        // buffer = strdup((char*) buf);
        char *a = (char*) buf;
        while((aPtr = strsep(&a, ","))) {
            words[count++] = aPtr;
        }

        Serial.print("node: ");
        Serial.print(fromBinary(words[0]));
        Serial.print(" type: ");
        Serial.print(words[1]);
        Serial.print(" value: ");
        Serial.println(words[2]);
        // if (fromBinary(words[1]) == (1+displayChar%SENSOR_NUM)) {
        //   printStr = strdup(words[2]);
        // }
        free(sensor_data[atoi(words[1])][fromBinary(words[0])]);
        sensor_data[atoi(words[1])][fromBinary(words[0])] = strdup(words[2]); 

        Serial.println("==============================");
        for (int i = 0; i < SENSOR_TYPE; ++i)
        {
          for (int j = 0; j < MAX_NODE; ++j)
          {
            if (sensor_data[i][j] == 0) {
              Serial.print("NULL");
            }
            else {
              Serial.print(sensor_data[i][j]);
            }

            if (atoi(words[1]) == 2) {
              Serial.print(",\t");
            }
            else {
              Serial.print("\t,\t");
            }

          }
          Serial.println();
        }
        Serial.println("==============================");

    }
    // Serial.println("3. === ENDED ===");

  charDisplay();  
  unsigned long current = millis();
  // Serial.println(current);
  
}

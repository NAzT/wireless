#include <LiquidCrystal.h>
#include <DFR_LCDKeypad.h>
#include <VirtualWire.h>
#include <stdio.h>
#include <string.h>
 
static struct pt pt1, pt2;

#define MAX_WORDS 100
#ifndef NULL
#define NULL 0
#endif

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


typedef struct {
  byte type;
  byte node;
  char *str;
} sensor_t;

sensor_t* sensor_data[SENSOR_TYPE][MAX_NODE] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
}; 
const byte cBrightness(128);
//Pin assignments for DFRobot LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DFR_LCDKeypad keypad(cBrightness);
byte displayChar(1);
int vertChar;

static byte xPos(0);
static byte yPos(0);

char *buffer, *words[MAX_WORDS], *aPtr, *bPtr;
int count = 0, i;

int fromBinary(char *s) {
  return (int) strtol(s, NULL, 2);
}


// Character display is occuring once in setup, then only again in the button routines
// no need to refresh in update loop unless there is a change.  For now just redisplay all 
// text instead of additional logic to update only chagned value and display character.
// it is by far fast enough.
void charDisplay() {
    sensor_t *sensor_ptr; 
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
    sensor_ptr = sensor_data[displayChar%SENSOR_NUM][vertChar%MAX_NODE];
    if (sensor_ptr == 0) {
      lcd.print("NULL            ");
    }
    else {
      lcd.print((*sensor_ptr).str);
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
   
  delay(250);
  
  lcd.clear();
  charDisplay();
}

void loop() {
  // updates current key press value and calls button callbacks if necessary  
  keypad.update();
  count = 0;
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  // vw_wait_rx();
  if (vw_get_message(buf, &buflen)) // Non-blocking    
  {
      int l_type;
      int l_node;
      char* l_value;
      digitalWrite(13, true); // Flash a light to show received good message
      Serial.println((char*) buf);
      char *a = (char*) buf;
      while((aPtr = strsep(&a, ","))) {
          words[count++] = aPtr;
      }

      l_type = atoi(words[1]);
      l_node = fromBinary(words[0]);


      sensor_t *tmpSensor;
      sensor_t *sensor_temp;
      sensor_temp = sensor_data[l_type][l_node];
      if (sensor_temp != 0) {
        free(sensor_data[l_type][l_node]->str);
      }
      free(sensor_data[l_type][l_node]);


      Serial.print("node: ");
      Serial.print(l_node);
      Serial.print(" type: ");
      Serial.print(l_type);
      Serial.print(" value: ");
      Serial.println(words[2]);

      tmpSensor = (sensor_t*)malloc(sizeof(sensor_t));
      tmpSensor->node = l_node;
      tmpSensor->type = l_type;
      tmpSensor->str = strdup(words[2]);
      sensor_data[l_type][l_node] = tmpSensor;

      Serial.println("==============================");
      for (int i = 0; i < SENSOR_TYPE; ++i)
      {
        for (int j = 0; j < MAX_NODE; ++j)
        {
          if (sensor_data[i][j] == 0) {
            Serial.print("null");
          }
          else {
            Serial.print((*sensor_data[i][j]).str);
          }

          if (l_type == 2) {
            Serial.print(",\t");
          }
          else {
            Serial.print(",\t");
          }

        }
        Serial.println();
      }
      Serial.println("==============================");
      digitalWrite(13, false); // Flash a light to show received good message
  }

  charDisplay();  
  unsigned long current = millis();
  // Serial.println(current);
  
}

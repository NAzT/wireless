#include <LiquidCrystal.h>
#include <DFR_LCDKeypad.h>
#include <VirtualWire.h>
#include <stdio.h>
#include <string.h>
 
#define MAX_WORDS 100
#ifndef NULL
#define NULL 0
#endif

// Values are midpoints between analog value of key press.

#define NUM_SENSOR 8
#define MAX_NODE 8

#define DEBUG 0
#define SHOW_TABLE 0

const byte cBrightness(128);
//Pin assignments for DFRobot LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DFR_LCDKeypad keypad(cBrightness);

byte sensor_page(1);
byte select_pressed(0);
int sensor_nid;
int sensor_link_num;

char *words[MAX_WORDS], *aPtr, *bPtr;
int count = 0;

// user defined functions
int fromBinary(char *s) {
  return (int) strtol(s, NULL, 2);
}

// user defined types
typedef struct {
  byte type;
  byte node;
  char *str;
} sensor_t;

// initialization
sensor_t* sensor_data[MAX_NODE][NUM_SENSOR] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0} 
}; 

sensor_t* sensor_link[MAX_NODE];


// Character display is occuring once in setup, then only again in the button routines
// no need to refresh in update loop unless there is a change.  For now just redisplay all 
// text instead of additional logic to update only chagned value and display character.
// it is by far fast enough.
void charDisplay() {
    sensor_t *sensor_ptr; 
    int _sensor_type = sensor_page%NUM_SENSOR;
    int _sensor_nid = sensor_nid%MAX_NODE;

    // display characters mode
    lcd.setCursor(0, 0);
    lcd.print("Type: "); lcd.print(_sensor_type); lcd.print(" "); lcd.print("Node: "); lcd.print(_sensor_nid);
      
    lcd.setCursor(0,1);
    sensor_ptr = sensor_data[_sensor_type][_sensor_nid];

    if (sensor_ptr == NULL) {
      lcd.print("NULL            ");
    }
    else {
      lcd.print((*sensor_ptr).str);
      lcd.print("      ");
    }
    // lcd.print("  ");  // make sure to cleanup after numerical wrap around
 }

void buttonUp(eDFRKey key) {
  switch(key) {
    case eUp:
      if (DEBUG) {
        Serial.println("eUp");
      }
      sensor_nid = abs(--sensor_nid);
      break;
    case eDown:
      ++sensor_nid;
      if (DEBUG) {
        Serial.println("eDown");
      }      
      break;
      
    case eLeft:
      //Serial.println("eLeft");        
      if (DEBUG) {
        Serial.println("eLeft");
      }      

      // the previous page 
      --sensor_page;

      // reset sensor nid to zero
      sensor_nid = 0;
      break;
      
    case eRight:
      if (DEBUG) {
        Serial.println("eRight");
      }    
      // the next page
      ++sensor_page;
      // reset sensor id to zero
      sensor_nid = 0;
      break;
      
    case eSelect:
      if (DEBUG) {
        Serial.println("eSelect");
      }    

      // toggle active sensors
      ++select_pressed;
      sensor_t* tmp = sensor_link[select_pressed%sensor_link_num];
      sensor_page = tmp->type;
      sensor_nid = tmp->node;

      charDisplay();  
      // switch to mouse mode
      // keypad.setButtonHeldHandler(0);
      return;
  }
  charDisplay();
}


void setup() {
  sensor_nid = 0;

  Serial.begin(9600);
  Serial.println("SETUP");
  // Initialise the IO and ISR
  vw_set_rx_pin(2);    
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running  
  
  //// LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCDKeypad v1.0");
  keypad.setButtonDownHandler(buttonUp);
  // keypad.setButtonUpHandler(buttonUp);
   
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
  //vw_wait_rx();

  if (vw_get_message(buf, &buflen)) // Non-blocking    
  {
      digitalWrite(13, true); // Flash a light to show received good message

      int l_type;
      int l_node;
      char* l_value;

      //SPLIT DATA WITH ,
      char *a = (char*) buf;
      while((aPtr = strsep(&a, ","))) {
          words[count++] = aPtr;
      }

      // grab a node & type of sensors
      l_type = atoi(words[1]);
      l_node = fromBinary(words[0]);

      // free string memory
      if (sensor_data[l_type][l_node] != NULL) {
        free(sensor_data[l_type][l_node]->str);
      }

      free(sensor_data[l_type][l_node]);

      if (DEBUG) {
        Serial.print("node: ");
        Serial.print(l_node);
        Serial.print(" type: ");
        Serial.print(l_type);
        Serial.print(" value: ");
        Serial.println(words[2]);
      }

      sensor_t *temp_sensor;

      temp_sensor = (sensor_t*)malloc(sizeof(sensor_t));
      temp_sensor->node = l_node;
      temp_sensor->type = l_type;
      temp_sensor->str = strdup(words[2]);

      sensor_data[l_type][l_node] = temp_sensor;
       
      if (DEBUG) {
        Serial.println("==============================");
      }
      
      
      int sensor_link_idx = 0;
      sensor_link_num =0;
      
      
      for (int i = 0; i < NUM_SENSOR; ++i)
      {
        for (int j = 0; j < MAX_NODE; ++j)
        {
          if (sensor_data[i][j] == NULL) {
            if (SHOW_TABLE) {
              Serial.print("null");
            }
          }
          else {
            if (SHOW_TABLE) {
              Serial.print((*sensor_data[i][j]).str);
            }
            sensor_link[sensor_link_idx++] = sensor_data[i][j];
            sensor_link_num++;
          }

          if (SHOW_TABLE) {
            if (l_type == 2) {
              Serial.print(",\t");
            }
            else {
              Serial.print(",\t");
            }
          }
        }
        if (SHOW_TABLE) {
          Serial.println();
        }
      }
      
      if (SHOW_TABLE) {
        Serial.println("===========================");
      }
      
      // Send data to serial port
      for (int i=0; i < sensor_link_num; i++) {
        Serial.print("NODE: ");
        Serial.print(sensor_link[i]->node);
        Serial.print(" TYPE: ");
        Serial.print(sensor_link[i]->type);
        Serial.print(" VALUE: [");
        Serial.print(sensor_link[i]->str);
        Serial.println("]");        
      }

      if (DEBUG) {
        Serial.println("===========================");
      }

      digitalWrite(13, false); // Flash a light to show received good message
  }

  charDisplay();  

  unsigned long current = millis();
  // Serial.println(current);
  
}

// 433Mhz transmitter by Jan Leander Jensen 2011
// TX pin12, PTT pin 10

// include library
#include <VirtualWire.h> // ver. 1.5
#include <BH1750.h>
#include <Wire.h>

// end include library

// define things
/*
Connection:
 VCC-5v
 GND-GND
 SCL-SCL(analog pin 5)
 SDA-SDA(analog pin 4)
 ADD-NC or GND

*/

#define MSG_SIZE 19
// end define
//global variable
BH1750 lightMeter;
char buffer[MSG_SIZE];
// end global variable

// setup
void setup() {
  Serial.begin(9600);
  vw_set_tx_pin(13);      
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  pinMode(13, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);  
  randomSeed(analogRead(0));  
  lightMeter.begin();  
}
// end setup


// main loop
void loop()
{
    digitalWrite(13, true); // Flash a light to show transmitting
   
    buffer[0] = '0'; //digitalRead(8)+48;
    buffer[1] = digitalRead(7)+48;
    buffer[2] = digitalRead(8)+48;   
    // &buffer[3]
    memcpy(buffer+3, ",4,", 3);
    uint16_t lux = lightMeter.readLightLevel();
    char nat[20];
    
    itoa(lux, nat, 10);

//    memcpy(buffer+6, nat, strlen(nat)); 
    strcpy(&buffer[6], nat);
//    Serial.println(strlen(itoa(lux, nat, 10))); 

//    dtostrf(dht.readTemperature(), 2, 2, &buffer[6]);
//    buffer[11] = ':';
    
//    dtostrf(dht.readHumidity(), 2, 2, &buffer[12]);
    buffer[17] = '\0';
Serial.println(buffer);    
//    Serial.println((char*) buffer);
    for (int jj=0; jj<=3; jj++) {
      vw_send((uint8_t *)buffer, MSG_SIZE); // transmit msg
      vw_wait_tx(); // Wait for message to finish
    }

//    digitalWrite(13, false);
    delay(random(90, 3300));  
}
// end main loop

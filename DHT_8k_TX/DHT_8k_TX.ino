// 433Mhz transmitter by Jan Leander Jensen 2011
// TX pin12, PTT pin 10

// include library
#include <VirtualWire.h> // ver. 1.5
//#include "pt.h"
#include "DHT.h"
// end include library

// define things

#define DHTPIN 9     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define MSG_SIZE 19
// end define
//global variable
DHT dht(DHTPIN, DHTTYPE);
char buffer[MSG_SIZE];
// end global variable

// setup
void setup() {
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  pinMode(13, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);  

  dht.begin();  
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
    memcpy(buffer+3, ",2,", 3);
    
    dtostrf(dht.readTemperature(), 2, 2, &buffer[6]);
    buffer[11] = ':';
    
    dtostrf(dht.readHumidity(), 2, 2, &buffer[12]);
    buffer[17] = '\0';
    
    vw_send((uint8_t *)buffer, MSG_SIZE); // transmit msg
    vw_wait_tx(); // Wait for message to finish

    digitalWrite(13, false);
    delay(450);
}
// end main loop

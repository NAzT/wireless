// 433Mhz transmitter by Jan Leander Jensen 2011
// TX pin12, PTT pin 10

// include library
#include <VirtualWire.h> // ver. 1.5
// end define

/* Select the input pin for the flame detectors analogue output. */
#define FLAME_DETECT_ANA A0     
                                  
/* Select the input pin for the flame detectors digital output. */
#define FLAME_DETC_DIO 10

//global variable
char buffer[16];
int isFire = 0;
// end global variable

// setup
void setup() {
  Serial.begin(9600);
  Serial.println('a');
  vw_set_tx_pin(13);
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  pinMode(FLAME_DETC_DIO, INPUT); 
}
// end setup


// main loop
void loop()
{
//    digitalWrite(13, true); // Flash a light to show transmitting
   if (digitalRead(FLAME_DETC_DIO)) {
     int i = 0;
     for (i = 0; i < 30; i++) {
      buffer[0] = '@'; //digitalRead(8)+48;
      buffer[1] = '@';
      buffer[2] = '@';
      buffer[3] = '1';
      buffer[4] = '1'; 
      buffer[5] = '1';    
      buffer[6] = '\0';
      vw_send((uint8_t *)buffer, 16); // transmit msg
      vw_wait_tx(); // Wait for message to finish
     }
     
      isFire = 0;
    }

//    buffer[0] = '1'; //digitalRead(8)+48;
//    buffer[1] = '1';
//    buffer[2] = '1';
//    buffer[3] = '1';
//    buffer[4] = '1'; 
//    buffer[5] = '1';    
//    buffer[6] = '\0';   
    
    
////    dtostrf(dht.readTemperature(), 2, 2, &buffer[4]);
//    buffer[9] = ',';
////    dtostrf(dht.readHumidity(), 2, 2, &buffer[10]);
//    buffer[15] = '\0';    
    

//    digitalWrite(13, false);
    delay(100);
}
// end main loop

// 433Mhz transmitter by Jan Leander Jensen 2011
// TX pin12, PTT pin 10

// include library
#include <VirtualWire.h> // ver. 1.5
// end define

//global variable
char buffer[16];
// end global variable

// setup
void setup() {
  vw_set_tx_pin(6);
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
}
// end setup


// main loop
void loop()
{
    digitalWrite(13, true); // Flash a light to show transmitting
   
    buffer[0] = '0'; //digitalRead(8)+48;
    buffer[1] = '1';
    buffer[2] = '2';
    buffer[3] = ',';
    
//    dtostrf(dht.readTemperature(), 2, 2, &buffer[4]);
    buffer[9] = ',';
//    dtostrf(dht.readHumidity(), 2, 2, &buffer[10]);
    buffer[15] = '\0';    
    
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish

    digitalWrite(13, false);
    delay(150);
}
// end main loop

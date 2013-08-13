#include <VirtualWire.h>

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
char buffer[16];

void setup() {
  Serial.begin(9600);	// Debugging only

  // Initialise the IO and ISR
  vw_set_tx_pin(13);
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  pinMode(pirPin, INPUT);
}

void loop() {
   if(digitalRead(pirPin) == HIGH) {
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
}


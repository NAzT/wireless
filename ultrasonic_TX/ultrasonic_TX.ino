#include <VirtualWire.h> // ver. 1.5

#define trigPin 2
#define echoPin 3

char buffer[16];

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
//    vw_set_tx_pin(6);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);  
    vw_set_tx_pin(13);
    vw_set_ptt_inverted(true); // Required for PTT
    vw_setup(2000); // Bits per sec    
}
void loop() {
    long duration;
    int distance;
    digitalWrite(trigPin, LOW); // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    // delayMicroseconds(1000); - Removed this line
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) / 29.1;
//    memcpy(buffer, "999:", 4);

//    buffer[0] = digitalRead(6)+48;
//    buffer[1] = digitalRead(7)+48;
//    buffer[2] = digitalRead(8)+48;   
    memcpy(buffer, "$$$", 3);
    buffer[3] = ',';    
    if (distance >= 200 || distance < -5) {
        strcpy(buffer+4, "$$");           
    } else {
        sprintf (buffer+4, "%d", distance);
    }
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish
    delay(550);
}

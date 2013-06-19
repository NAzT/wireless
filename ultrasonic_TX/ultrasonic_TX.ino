#include <VirtualWire.h> // ver. 1.5
#define trigPin 9
#define echoPin 8
//#define led 13
char buffer[16];

void setup() {
//    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
//    pinMode(led, OUTPUT);
//    vw_set_tx_pin(6);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);  
    vw_set_tx_pin(13);
    vw_set_ptt_inverted(true); // Required for PTT
    vw_setup(2000); // Bits per sec
    
}
void loop() {
    memcpy(buffer, "999:", 4);
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
//    if (distance < 4) { // This is where the LED On/Off happens
////        digitalWrite(led, HIGH); // When the Red condition is met, the Green LED should turn off
//    }
//    else {
////        digitalWrite(led, LOW); // When the Red condition is met, the Green LED should turn off
//    }
//    
    if (distance >= 200 || distance <= 0) {
//        Serial.print("-1:");
        strcpy(buffer+4, "-1");        
//        Serial.println(distance);     
    } else {
        sprintf (buffer+4, "%d", distance);
    }
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish
    delay(200);
}

#include <VirtualWire.h>

void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_rx_pin(13);    
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    char msg[30];
    memcpy(msg, "GET /sensor/", 12);
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    uint8_t msg_size;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;
        digitalWrite(13, true); // Flash a light to show received good message
//        msg_size = strlen((char*) buf);
//        memcpy(msg+12, buf, msg_size);
//        strcpy(msg+12+msg_size, " HTTP/1.1");
        Serial.println((char*) buf);
//        Serial.print(" : ");
//        Serial.println(msg);
//                Serial.print(" : ");  
//        Serial.print(msg_size);



    }
}

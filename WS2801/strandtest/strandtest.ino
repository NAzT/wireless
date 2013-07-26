#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <VirtualWire.h>
#include "pt.h"

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(200, dataPin, clockPin);

//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

static struct pt pt1, pt2;
static int thread1( struct pt *pt, long timeout ) {
  static long t1 = 0;
  PT_BEGIN( pt );
  while(1) {
    PT_WAIT_UNTIL( pt, (millis() - t1) > timeout );
    Serial.print("IN THREAD 1");
    colorWipe(Color(255, 0, 0), 1);    
    t1 = millis();
  }
  PT_END( pt );
}

void setup() {
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_rx_pin(13);    
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running    
    strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
}


void loop() {
  // Some example procedures showing how to display to the pixels

//  colorWipe(Color(255, 0, 0), 1);
//  colorWipe(Color(0, 255, 0), 1);
//  colorWipe(Color(0, 0, 255), 1);
//  rainbow(20);
//  rainbowCycle(20);
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
        char* tmp = (char*)buf;
//        Serial.print((char*) buf);
//        Serial.print("---:---");
//        Serial.println(buflen);
        Serial.println(strcmp(tmp, "111111"));
        if (strcmp(tmp, "@@@111") == 0) {
          Serial.println("RED");
          thread1( &pt1, 200);
        }
        else {

        }
//        else if (strcmp((char*) buf, "000000") == 0) {
//          Serial.println("000000 GOT JA");
//          colorWipe(Color(0, 255, 0), 1);          
//        }
//        else {
//          Serial.print("Should Wipe Out With: ");
//          Serial.println((char*) buf);
//          colorWipe(Color(0, 0, 0), 1);
////            strip.show();
//        }
//    }
//    else {
////        Serial.println("ALL OFF");
//        strip.show();
//    }
    }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

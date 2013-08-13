#include <Wire.h>
#include <VirtualWire.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>

char buffer[16];

BH1750 lightMeter;

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(9600);
  vw_set_tx_pin(13);    
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  lightMeter.begin();
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
 
    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = '0';   
    buffer[3] = ',';
    buffer[4] = '0';
    buffer[5] = ',';
 
    dtostrf(bmp.readPressure(), 2, 2, &buffer[6]);
    buffer[15] = '\0';
    
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish
    Serial.println(buffer);

    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = '1';   
    buffer[3] = ',';
    buffer[4] = '1';
    buffer[5] = ',';
    
    dtostrf(bmp.readTemperature(), 2, 2, &buffer[6]);
    buffer[15] = '\0';
    
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish
    Serial.println(buffer);

    delay(300);
    
    uint16_t lux = lightMeter.readLightLevel();

    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = '0';   
    buffer[3] = ',';
    buffer[4] = '3';
    buffer[5] = ','; 
    
    dtostrf(lux, 2, 2, &buffer[6]);
    buffer[15] = '\0';
    
    vw_send((uint8_t *)buffer, 16); // transmit msg
    vw_wait_tx(); // Wait for message to finish
    Serial.println(buffer);
    delay(300);
    
//  
//    Serial.print("Temperature = ");
//    Serial.print(bmp.readTemperature());
//    Serial.println(" *C");
//    
//    Serial.print("Pressure = ");
//    Serial.print(bmp.readPressure());
//    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
//    Serial.print("Altitude = ");
//    Serial.print(bmp.readAltitude());
//    Serial.println(" meters");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
//    Serial.print("Real altitude = ");
//    Serial.print(bmp.readAltitude(101500));
//    Serial.println(" meters");
//    
//    Serial.println();
//    
//    uint16_t lux = lightMeter.readLightLevel();
//    Serial.print("Light: ");
//    Serial.print(lux);
//    Serial.println(" lx");
    
    delay(300);
}

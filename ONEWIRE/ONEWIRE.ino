#include <OneWire.h>
#include <VirtualWire.h> // ver. 1.5

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)
char buffer[12];

void setup(void) {
//  vw_set_tx_pin(13);
  vw_set_ptt_inverted(true); // Required for PTT
  vw_setup(2000); // Bits per sec
  pinMode(13, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);  
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  digitalWrite(13, true);
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }

  type_s = 0;  //("  Chip = DS18B20");

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000+random(300));     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  
    buffer[0] = digitalRead(6)+48;
    buffer[1] = digitalRead(7)+48;
    buffer[2] = digitalRead(8)+48;   
    buffer[3] = ',';
    buffer[4] = '1';
    buffer[5] = ',';    
    buffer[11] = '\0'; 
    
  dtostrf(celsius, 2, 2, &buffer[6]);  
  vw_send((uint8_t *)buffer, 16); // transmit msg
  vw_wait_tx(); // Wait for message to finish  
  digitalWrite(13, false);
  delay(50+random(300));
}

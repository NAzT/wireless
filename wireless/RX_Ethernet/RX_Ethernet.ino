/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include "pt.h"




// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "10.0.1.57";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(10,0,1,213);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void httpRequest() {
    Serial.println("connecting...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 3000)) {
    Serial.print("connected: ");
    Serial.println(client.connected());
    // Make a HTTP request:
    client.println("GET /sensor/311,3333 HTTP/1.1");
    client.println("Host: 10.0.1.57");
    client.println("Connection: close");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}


void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("SETUP");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("STARTED");
  // give the Ethernet shield a second to initialize:
  delay(1000);
  httpRequest();


}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  else {
    if (!client.connected()) {
        Serial.println("LOOPING CONNECTED");
      Serial.println("disconnecting.");
      client.stop();
    }    
//    httpRequest();    
    delay(1000);
  }
//
//  // if the server's disconnected, stop the client:

//
//    // do nothing forevermore:
//    while(true);
//  }

//  sleep(1000);
}


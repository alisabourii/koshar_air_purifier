#include "MQ135.h"
#include <WiFi.h>
#include <Wire.h>

int air_quality;

//TODO: ESP32 MQTT user config
/*const char* ssid = "TTNET_ZyXEL_RVAN";
const char* password = "3DaE1B2443497";*/

const char* ssid = "G301";
const char* password = "alialiali";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(32, INPUT);        //Gas sensor will be an input to the ESP32
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}


void loop() {

  MQ135 gasSensor = MQ135(33);
  float air_quality = gasSensor.getPPM();
  
  delay(100);
  Serial.print(air_quality);      
  Serial.println(" PPM");
  delay(200);

}
#include "DHTesp.h"
#include "MQ135.h"
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESPAsyncWebServer.h"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int DHT_PIN = 32;
DHTesp dhtSensor;

int air_quality;
const char* ssid = "Celal";
const char* password = "Celal5577";
// Set web server port number to 80
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

int trigPin = 13;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ŞSY MTAL</h2>
  <p>
   <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Kalan Pil</span>
    <span id="KalanPilID">%KalanPil%</span>
    <sup class="units">&KalanPil;</sup>
  </p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Sıcaklık</span> 
    <span id="temperature">%SICAKLIK%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Nem</span>
    <span id="humidity">%NEM%</span>
    <sup class="units">&percnt;</sup>
  </p>

  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">CO2</span>
    <span id="karbonID">%Co2%</span>
    <sup class="units">&percnt;</sup>
  </p>

   <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Yerden Mesafe</span>
    <span id="mesafeID">%YerdenMesafe%</span>
    <sup class="units">&percnt;</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}


void setup() {
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hos geldiniz");
  display.display(); 

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  // Start server
  server.begin();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  // Start server
  server.begin();
}

String msfDurum = "";
String gazDurum = "";
String sckDurum  ="";
int durum = 0;

void loop() {
  if(ultra() > 10 && ultra() < 100){
    msfDurum = "Uygun yer"; durum ++;
  }
  else{
    msfDurum = "Uygun degil";
  }

  

  
  if(dhtSicaklik() > 18 && dhtSicaklik() < 23){
    sckDurum = "Ideal";durum ++;
  }
  else{
    sckDurum = "Sicak/Soguk"; 
  }
  Serial.println(gazSensor());
   if(gazSensor() > 50000 && gazSensor() < 150000){
    sckDurum = "Normal";durum ++;
  }
  else{
    sckDurum = "Y/D";
  }

  if(durum == 3){
    digitalWrite(16, 0);  digitalWrite(17, 1);  digitalWrite(18, 0);
    
  }
  else if(durum == 2){
    digitalWrite(16, 0);  digitalWrite(17, 0);  digitalWrite(18, 1);
    
  }
  else if(durum <= 1){
    digitalWrite(16, 1);  digitalWrite(17, 0);  digitalWrite(18, 0);
  }


  if(gazSensor() > 150000){
    digitalWrite(19, 1);
  }
  else{digitalWrite(19, 0);}

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println(msfDurum);
  display.display(); 
  delay(1000);
  display.clearDisplay(); 

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Derece ");

  display.setTextSize(2);
  display.setCursor(10, 30);
  display.println(dhtSicaklik());
  display.display(); 
  delay(1000);
  display.clearDisplay(); 

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println(gazDurum);
  display.display(); 
  delay(1000);
  display.clearDisplay(); 
  
  durum = 0;
}
int ultra(){
   // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  /*Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();*/
  
  return cm;
}
int dhtSicaklik(){
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  /*Serial.println("Temp: " + String(data.temperature, 2) + "°C");
  Serial.println("Humidity: " + String(data.humidity, 1) + "%");
  Serial.println("---");*/
  return data.temperature;
}
int dhtNem(){
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  /*Serial.println("Temp: " + String(data.temperature, 2) + "°C");
  Serial.println("Humidity: " + String(data.humidity, 1) + "%");
  Serial.println("---");*/
  return data.humidity;
}


int gazSensor(){
  MQ135 gasSensor = MQ135(33);
  float air_quality = gasSensor.getPPM();
  delay(100);
  //Serial.print(air_quality);      
  //Serial.println(" PPM");
  return air_quality;
}

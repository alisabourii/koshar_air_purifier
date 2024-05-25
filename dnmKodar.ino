/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
int trig = 34;
int echo = 35;

int DOUTpin = 13;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  pinMode(DOUTpin, INPUT);  
  Serial.begin(115200);

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
  display.println("Hello, world!");
  display.display(); 
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  // Display static text
  display.println("Hocam deneme");
  display.display(); 
  delay(1000);
  display.clearDisplay(); 



}
int mesafe(){
  digitalWrite(trig,1);delay(10);digitalWrite(trig,0);
  int sure = pulseIn(echo,1);
  int msf = (sure/2)/28.97;
  return msf;
}

int gazSensor(){
  int gazSensorValue = digitalRead(13);
  return gazSensorValue;
}

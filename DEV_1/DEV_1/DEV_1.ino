#include "RTClib.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h">

// DHT
const int DHT1_PIN = 15;
const int DHT2_PIN = 4;
#define DHTTYPE DHT11   // DHT 11
DHT dht1(DHT1_PIN, DHTTYPE);
DHT dht2(DHT2_PIN, DHTTYPE);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String text;

// RTC
RTC_DS3231 rtc;

// SOIL MOISTURE
const int SOIL1_PIN = 34;
const int SOIL2_PIN = 35;

// RELAY
const int MOTOR1_PIN = 5;
const int MOTOR2_PIN = 18;
const int LAMP_PIN = 19;
//const int FAN_PIN = ;


// Setpoint
int maxSoilMoisture = 2000;

// Variables
unsigned long current_time = 0;
unsigned long prev_motor1_on = 0;
unsigned long prev_motor2_on = 0;
int onMotorTime = 500;

void oledDisplayCenter(String text) {
  int16_t x1, y1;
  uint16_t width, height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(text);
  display.display();
}

void setup () {
  Serial.begin(115200);

  dht1.begin();
  dht2.begin();

  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  delay(100);

#ifndef ESP8266
  while (!Serial);
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  current_time = millis();
  DateTime now = rtc.now();

//  DHT
  float humi1 = dht1.readHumidity();
  float temp1 = dht1.readTemperature();
  float humi2 = dht2.readHumidity();
  float temp2 = dht2.readTemperature();
  float humiAverage = (humi1 + humi2) / 2;
  float tempAverage = (temp1 + temp2) / 2;
//  
//  if (isnan(humi1) || isnan(temp1)) {
//    Serial.println(F("Failed to read from DHT1 sensor!"));
//    return;
//  }
//
//  if (isnan(humi2) || isnan(temp2)) {
//    Serial.println(F("Failed to read from DHT2 sensor!"));
//    return;
//  }

//  Soil Moisture
  int soil1 = analogRead(SOIL1_PIN);
  int soil2 = analogRead(SOIL2_PIN);
  
//  OLED
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);        // Draw white text
//  Row 1
  text = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + String(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(text);  
  display.println();
  display.println();
//  Row 2
  text = "H: " + String(humiAverage) + " %  T: " + String(tempAverage) + " C";
  display.print(text);
  display.println();
  display.println();
//  Row 3
  text = "Soil Moisture 1: " + String(soil1);
  display.print(text);
  display.println();
  display.println();
//  Row 4
  text = "Soil Moisture 2: " + String(soil2);
  display.print(text);
  display.display();
  
//  Control Soil Moisture 1
  Serial.print(soil1);
  Serial.print(" (");
  Serial.print((current_time - prev_motor1_on));
  Serial.print(")");
  if (soil1 > maxSoilMoisture && (current_time - prev_motor1_on) >= 10000) {
    digitalWrite(MOTOR1_PIN, LOW);
    delay(onMotorTime);
    digitalWrite(MOTOR1_PIN, HIGH);
    prev_motor1_on = millis();
  } 

//  Control Soil Moisture 2
  Serial.print(" | ");
  Serial.print(soil2);
  Serial.print(" (");
  Serial.print((current_time - prev_motor2_on));
  Serial.print(")");
  if (soil2 > maxSoilMoisture && (current_time - prev_motor2_on) >= 10000) {
    digitalWrite(MOTOR2_PIN, LOW);
    delay(onMotorTime);
    digitalWrite(MOTOR2_PIN, HIGH);
    prev_motor2_on = millis();
  }

  Serial.println();
  delay(500);
}

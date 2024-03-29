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
const int FAN_PIN = 23;

// Setpoint
int maxSoilMoisture = 2000;

// Variables
unsigned long current_time = 0;
unsigned long prev_motor1_on = 0;
unsigned long prev_motor2_on = 0;
int onMotorTime = 500;
int buf[100];
int temp;
int avgValue;
bool morningWatering = false;
bool eveningWatering = false;

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
  pinMode(LAMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, HIGH);
  digitalWrite(LAMP_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);

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

//  Soil Moisture
  int soil1 = getSoilMoistureVal(SOIL1_PIN);  // send to ThingSpeak
  int soil2 = getSoilMoistureVal(SOIL2_PIN);  // send to ThingSpeak

//  DHT
  float humi1 = dht1.readHumidity();
  float temp1 = dht1.readTemperature();
  float humi2 = dht2.readHumidity();
  float temp2 = dht2.readTemperature();
  float humiAverage = (humi1 + humi2) / 2;  // send to ThingSpeak
  float tempAverage = (temp1 + temp2) / 2;  // send to ThingSpeak
  
  if (isnan(humi1) || isnan(temp1)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    oledDisplayCenter("DHT #1 disconnected!");
    Serial.println(F("Failed to read from DHT1 sensor!"));
    return;
  }

  if (isnan(humi2) || isnan(temp2)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    oledDisplayCenter("DHT #2 disconnected!");
    Serial.println(F("Failed to read from DHT2 sensor!"));
    return;
  }
  
//  OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);
//  Row 1
  text = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + String(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(text);  
  display.println();
  display.println();
//  Row 2
  text = "H: " + String(humiAverage) + " % T: " + String(tempAverage) + " C";
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

//  Fan ON
  if (28 < tempAverage) {
    digitalWrite(FAN_PIN, LOW);
  }

//  Lamp ON
  if (tempAverage < 26) {
    digitalWrite(LAMP_PIN, LOW);
  }

//  Fan OFF
  if (tempAverage < 27.5) {
    digitalWrite(FAN_PIN, HIGH);
  }

//  Lamp OFF
  if (26.5 < tempAverage) {
    digitalWrite(LAMP_PIN, HIGH);
  }

//  Daily reset watering
  if (now.hour() == 0 && !morningWatering && !eveningWatering) {
    morningWatering = true;
    eveningWatering = true;
  }

  if ((morningWatering && (now.hour() == 7)) || (eveningWatering && (now.hour() == 17))) {
//  Control Soil Moisture Motor 1
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

//  Control Soil Moisture Motor 2
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

    if (morningWatering) {
      morningWatering = false; 
    } else if (eveningWatering) {
      eveningWatering = false;
    }
  }

  Serial.println();
  delay(100);
}

int getSoilMoistureVal(int pin) {
  for (int i = 0; i < 100; i++) {
    temp = analogRead(pin);
    buf[i] = temp;
  }

  for (int i = 0; i < 100-1; i++) {
    for (int j = i + 1; j < 100; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for (int i = 0; i < 50; i++) {
    avgValue += buf[i+24];
  }

  avgValue /= 50;

  return avgValue;
}


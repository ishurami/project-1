#include "RTClib.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String text;

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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  delay(2000);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

//  display.startscrollright(0x00, 0x0F);
  
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
  DateTime now = rtc.now();
  
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  text = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " " + String(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(text);  
  
  display.println();
  display.println();

  text = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  display.setTextSize(2);
  display.print(text);

  display.display();
  
  delay(1000);
}

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS3231 rtc;

const int resetTime[2] = {0, 50};
const int alarmTime[2][2] = {{0, 48}, {0, 49}};
bool isWatered[] = {false, false};

void setup () {
  Serial.begin(115200);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {
    DateTime now = rtc.now();
//
//    if (now.hour() == resetTime[0] && now.minute() == resetTime[1]) {
//      isWatered[0] = false;
//      isWatered[1] = false;
//    }
//    
//    if ((now.hour() == alarmTime[0][0]) && (now.minute() == alarmTime[0][1]) && !(isWatered[0])) {
//        Serial.print("Siram pagi ");
//        isWatered[0] = true;
//    }
//
//    if ((now.hour() == alarmTime[0][0]) && (now.minute() == alarmTime[1][1]) && !(isWatered[1])) {
//        Serial.print("Siram sore ");
//        isWatered[1] = true;
//    }
//    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" - ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
//    Serial.print(" - ");
//    Serial.print(isWatered[0]);
//    Serial.print(" - ");
//    Serial.print(isWatered[1]);
    Serial.println();
//
//    Serial.println();
    delay(1000);
}

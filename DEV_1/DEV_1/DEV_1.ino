// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "DHT.h"

// DS3231 RTC pin
// SDA   21
// SCL   22
// GND   GND
// VCC   VIN (5V)

// Soil moisture sensor pin
#define soilPin 34
// GND   GND
// VCC   3.3V

// Relay pin
#define relayWater 4
#define relayLamp 5
#define relayFan 18
#define relay_4 19
// GND   GND
// VCC   VIN (5V)

// DHT 11
#define DHTTYPE DHT11
#define DHTPIN 15
// GND   GND
// VCC   3.3V
DHT dht(DHTPIN, DHTTYPE);

// Setting time to reset isWatered flag to become false
const int resetTime[2] = {10, 12};

// Setting time to set isWatered flag to become true
const int alarmTime[2][2] = {{10, 11}, {9, 27}};

const int soilSetPoint = 10;

// Define isWatered
bool isMorningTime = false;
bool isAfternoonTime = false;
bool isWatered[] = {false, false};

float temperature = 0;
float humidity = 0;
const float temperatureLimit[] = {25, 32};
const float humidityLimit[] = {60, 73};

float soilMoisture = 0;


RTC_DS3231 rtc;

void setup () {
  Serial.begin(115200);
  pinMode(relayWater, OUTPUT);
  pinMode(relayLamp, OUTPUT);
  pinMode(relayFan, OUTPUT);
  pinMode(relay_4, OUTPUT);

  digitalWrite(relayWater, HIGH);
  digitalWrite(relayLamp, HIGH);
  digitalWrite(relayFan, HIGH);
  digitalWrite(relay_4, HIGH);

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

  dht.begin();
}

void loop () {
    DateTime now = rtc.now();
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    soilMoisture = analogRead(soilPin);
    
//    alarmTime[0][x] : Morning
//    alarmTime[1][x] : Afternoon
    isMorningTime = (now.hour() == alarmTime[0][0]) && (now.minute() == alarmTime[0][1]);
    isAfternoonTime = (now.hour() == alarmTime[0][0]) && (now.minute() == alarmTime[1][1]);

//    Next day at 0 AM, reset isWatered flag
    if (now.hour() == resetTime[0] && now.minute() == resetTime[1]) {
      isWatered[0] = false;
      isWatered[1] = false;
    } else if (isMorningTime && !(isWatered[0])) {
      watering(now.hour(), now.minute(), 0);
    } else if (isAfternoonTime && !(isWatered[1])) {
      watering(now.hour(), now.minute(), 1);
    }

    if (temperature < temperatureLimit[0]) {
      digitalWrite(relayLamp, LOW);
      delay(3000);
//      digitalWrite(relayLamp, HIGH);
    } else if (temperature > temperatureLimit[1]) {
      digitalWrite(relayFan, LOW);
      delay(3000);
//      digitalWrite(relayFan, HIGH);
    }

    if (humidity < humidityLimit[0]) {
      digitalWrite(relayFan, HIGH);
      delay(3000);
      digitalWrite(relayFan, LOW);
    } else if (humidity > humidityLimit[1]) {
      digitalWrite(relayLamp, HIGH);
      delay(3000);
      digitalWrite(relayLamp, LOW);
    }

    printSerial(now);
    
    delay(1000);
}

void watering(int aHour, int aMinute, int aTime) {
  if (soilMoisture > soilSetPoint) {
//    Turn motor ON when normally open
    digitalWrite(relayWater, HIGH);

//    Turn motor ON when normally close
//    digitalWrite(relayWater, LOW);

    if (aTime) {
      Serial.print("Siram sore ");
    } else {
      Serial.print("Siram pagi "); 
    }
    
    isWatered[aTime] = true;

    delay(3000);

//    Turn motor OFF when normally open    
    digitalWrite(relayWater, LOW);
    
//    Turn motor OFF when normally close
//    digitalWrite(relayWater, HIGH);
  } else {
    if (aTime) {
      Serial.print("Below set point, no afternoon watering ");
    } else {
      Serial.print("Below set point, no morning watering ");
    }

    isWatered[aTime] = true;

    delay(3000);
  }
}

void printSerial(DateTime now) {  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" | ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" | ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" | ");
  Serial.print(soilMoisture);
  Serial.print(" | ");
  Serial.print(isWatered[0]);
  Serial.print(" - ");
  Serial.print(isWatered[1]);
  Serial.println();
}


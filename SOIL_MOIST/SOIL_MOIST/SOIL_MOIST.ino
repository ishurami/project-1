//#define SensorPin A0  // used for Arduino and ESP8266
#define SensorPin 34  // used for ESP32

void setup() { 
  Serial.begin(115200);
}

void loop() {
  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);
  delay(2000);
} 

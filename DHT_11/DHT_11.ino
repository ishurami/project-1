#include "DHT.h"

const int DHT1_PIN = 15;
const int DHT2_PIN = 4;

#define DHTTYPE DHT11   // DHT 11

DHT dht1(DHT1_PIN, DHTTYPE);
DHT dht2(DHT2_PIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht1.begin();
  dht2.begin();
}

void loop() {
  float humi1 = dht1.readHumidity();
  float temp1 = dht1.readTemperature();

  float humi2 = dht2.readHumidity();
  float temp2 = dht2.readTemperature();

  if (isnan(humi1) || isnan(temp1)) {
    Serial.println(F("Failed to read from DHT1 sensor!"));
    return;
  }

  if (isnan(humi2) || isnan(temp2)) {
    Serial.println(F("Failed to read from DHT2 sensor!"));
    return;
  }

  Serial.print(F("Humi1: "));
  Serial.print(humi1);
  Serial.print(F("%  Temp1: "));
  Serial.print(temp1);
  Serial.print(F("°C"));

  Serial.print(F(" | Humi2: "));
  Serial.print(humi2);
  Serial.print(F("%  Temp2: "));
  Serial.print(temp2);
  Serial.print(F("°C"));
  
  Serial.println();

  delay(1000);
}

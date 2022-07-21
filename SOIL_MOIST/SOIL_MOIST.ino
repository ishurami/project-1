const int SOIL1_PIN = 34;
const int SOIL2_PIN = 35;

void setup() { 
  Serial.begin(115200);
}

void loop() {
  Serial.print("SOIL MOISTURE 1: ");
  Serial.print(analogRead(SOIL1_PIN));
  Serial.print(" | SOIL MOISTURE 2: ");
  Serial.println(analogRead(SOIL2_PIN));
  delay(1000);
} 

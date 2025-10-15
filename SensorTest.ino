int sensorPin = A0;
int sensorValue = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println("Iniciando sensor de humedad...");
}

void loop() {
  sensorValue = analogRead(sensorPin);
  Serial.print("Lectura cruda: ");
  Serial.print(sensorValue);

  int humedad = map(sensorValue, 3400, 1200, 0, 100);
  humedad = constrain(humedad, 0, 100);
  Serial.print("  -> Humedad: ");
  Serial.print(humedad);
  Serial.println("%");

  delay(1000);
}

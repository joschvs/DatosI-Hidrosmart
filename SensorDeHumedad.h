#ifndef SENSORDEHUMEDAD_H
#define SENSORDEHUMEDAD_H

#include <Arduino.h>

// Clase para manejar el sensor de humedad del suelo
class SensorDeHumedad {
private:
  int pinA;         
public:
    SensorDeHumedad(int analogPin)
     {
    pinA = analogPin;
    analogReadResolution(12);
  }
  int leerPorcentaje() const {
    int sensorValue = analogRead(pinA);
    int humedad = map(sensorValue, 3400, 1200, 0, 100);
    humedad = constrain(humedad, 0, 100);
    return humedad;
  }
};
#endif
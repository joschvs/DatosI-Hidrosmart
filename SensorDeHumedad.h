#ifndef SENSORDEHUMEDAD_H
#define SENSORDEHUMEDAD_H

#include <Arduino.h>

// Clase para manejar el sensor de humedad del suelo
class SensorDeHumedad {
  private:
    uint8_t pinA;           // Pin analógico donde está conectado el sensor
    uint8_t pinD;           // Pin digital del módulo (opcional)
    uint8_t nSamples;       // Número de muestras para promediar la lectura
    int     adcDry;         // Valor de calibración para suelo seco
    int     adcWet;         // Valor de calibración para suelo húmedo

    // Lee el valor analógico una vez
    int readOnce() const {
      return analogRead(pinA);
    }

    // Limita el valor entre un mínimo y máximo
    static int clamp(int v, int lo, int hi) {
      if (v < lo) return lo;
      if (v > hi) return hi;
      return v;
    }

  public:
    // Constructor: define los pines y valores de calibración por defecto
    explicit SensorDeHumedad(uint8_t analogPin, uint8_t digitalPin = 255)
      : pinA(analogPin), pinD(digitalPin), nSamples(10),
        adcDry(3400), adcWet(1200) {
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
      #if defined(analogReadResolution)
        analogReadResolution(12);
      #endif
#endif
      if (pinD != 255) pinMode(pinD, INPUT);
    }

    // Cambia el número de muestras para promediar
    void setSamples(uint8_t samples) { nSamples = samples == 0 ? 1 : samples; }

    // Calibra el sensor con valores de suelo seco y húmedo
    void calibrar(int valorSeco, int valorMojado) {
      adcDry = max(valorSeco, valorMojado);
      adcWet = min(valorSeco, valorMojado);
    }

    // Lee el valor crudo promediado del sensor
    int leerCrudo() const {
      long acc = 0;
      for (uint8_t i = 0; i < nSamples; ++i) acc += readOnce();
      return (int)(acc / nSamples);
    }

    // Devuelve el porcentaje de humedad (0 = seco, 100 = húmedo)
    int leerPorcentaje() const {
      int raw = leerCrudo();
      raw = clamp(raw, adcWet, adcDry);
      long num   = (long)(adcDry - raw) * 100L;
      long denom = (long)(adcDry - adcWet);
      int pct = denom > 0 ? (int)(num / denom) : 0;
      return clamp(pct, 0, 100);
    }

    // Si usas el pin digital, devuelve si el suelo está húmedo según el comparador
    bool digitalHumedo() const {
      if (pinD == 255) return false;
      return digitalRead(pinD) == HIGH;
    }
};

#endif
#ifndef VALVULA_H
#define VALVULA_H

#include <Arduino.h>
// Control de bomba 3–6 V vía PWM (MOSFET low-side).
// Compatible con uso simple mediante activarValvula(int).
class Valvula {
  int _pinPWM;
public:
  explicit Valvula(int pinPWM)
    : _pinPWM(pinPWM){}

  void begin() {
    pinMode(_pinPWM, OUTPUT);
  }
  void activarValvula(int estado) {
    if (estado == 1) {
      digitalWrite(_pinPWM, HIGH);
    } else {
      digitalWrite(_pinPWM, LOW);
    }
  }
};
#endif
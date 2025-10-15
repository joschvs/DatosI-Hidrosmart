#ifndef VALVULA_H
#define VALVULA_H

#include <Arduino.h>

// Control de bomba 3–6 V vía PWM (MOSFET low-side).
// Compatible con uso simple mediante activarValvula(int).
class Valvula {
  uint8_t  _pinPWM;
  bool     _isOn;
  uint8_t  _speedTarget;   // 0–255
  uint8_t  _speedNow;      // rampa simple
  uint32_t _lastChangeMs;
  uint32_t _minOnMs;
  uint32_t _minOffMs;
  bool     _pendingPulse;
  uint32_t _pulseEndMs;
  uint16_t _rampPeriodMs;  // ms entre pasos de rampa
  uint8_t  _rampUp;        // incremento por paso
  uint8_t  _rampDown;      // decremento por paso

  static bool reached(uint32_t t) { return (int32_t)(millis() - t) >= 0; }

  void applyPWM(uint8_t duty) { analogWrite(_pinPWM, duty); }

  bool canOn()  const { return (millis() - _lastChangeMs) >= _minOffMs; }
  bool canOff() const { return (millis() - _lastChangeMs) >= _minOnMs;  }

public:
  // pinPWM debe ser un pin con PWM
  explicit Valvula(uint8_t pinPWM)
  : _pinPWM(pinPWM), _isOn(false), _speedTarget(0), _speedNow(0),
    _lastChangeMs(0), _minOnMs(0), _minOffMs(0),
    _pendingPulse(false), _pulseEndMs(0),
    _rampPeriodMs(30), _rampUp(6), _rampDown(8) {}

  void begin() {
    pinMode(_pinPWM, OUTPUT);
    _isOn = false;
    _speedTarget = 0;
    _speedNow = 0;
    applyPWM(0);
    _lastChangeMs = millis();
  }

  // API "simple" compatible: 1=encender, 0=apagar. Devuelve 1 si encendida, 0 si apagada.
  int activarValvula(int estado) {
    if (estado) { start(255); } else { stop(); }
    return _isOn ? 1 : 0;
  }

  // Encender con velocidad (0–255). Respeta minOff.
  bool start(uint8_t speed = 255) {
    speed = constrain(speed, (uint8_t)0, (uint8_t)255);
    if (!_isOn) {
      if (!canOn()) return false;
      _isOn = true;
      _lastChangeMs = millis();
    }
    _speedTarget = speed;
    _pendingPulse = false;
    return true;
  }

  // Apagar con rampa a 0. Respeta minOn.
  bool stop() {
    if (!_isOn) return true;
    if (!canOff()) return false;
    _speedTarget = 0;
    _pendingPulse = false;
    return true;
  }

  // Fija duty objetivo (0–255). Si estaba apagada y speed>0 intenta encender.
  bool setVelocidad(uint8_t speed) {
    speed = constrain(speed, (uint8_t)0, (uint8_t)255);
    if (speed == 0) return stop();
    return start(speed);
  }

  // Pulso no bloqueante: enciende 'ms' a 'speed' y luego apaga.
  bool pulso(uint32_t ms, uint8_t speed = 255) {
    if (ms == 0) return false;
    if (!start(speed)) return false;
    _pendingPulse = true;
    _pulseEndMs = millis() + ms;
    return true;
  }

  // Llamar en loop()
  void actualizar() {
    const uint32_t now = millis();

    if (_pendingPulse && reached(_pulseEndMs)) {
      if (canOff()) { _speedTarget = 0; _pendingPulse = false; }
    }

    static uint32_t lastStep = 0;
    if (_speedNow != _speedTarget && (now - lastStep) >= _rampPeriodMs) {
      lastStep = now;
      if (_speedTarget > _speedNow) {
        uint16_t next = _speedNow + _rampUp;
        _speedNow = (next > _speedTarget) ? _speedTarget : next;
      } else {
        int16_t next = (int16_t)_speedNow - (int16_t)_rampDown;
        _speedNow = (next < _speedTarget) ? _speedTarget : (uint8_t)next;
      }
      applyPWM(_speedNow);
    }

    if (_isOn && _speedNow == 0 && _speedTarget == 0) {
      _isOn = false;
      _lastChangeMs = now;
    }
    if (!_isOn && _speedNow > 0) {
      _isOn = true;
      _lastChangeMs = now;
    }
  }

  void setTiemposMinimos(uint32_t minOnMs, uint32_t minOffMs) {
    _minOnMs = minOnMs; _minOffMs = minOffMs;
  }

  void setRampa(uint16_t periodoMs, uint8_t subida, uint8_t bajada) {
    _rampPeriodMs = periodoMs; _rampUp = subida; _rampDown = bajada;
  }

  bool estaEncendida() const { return _isOn; }
  uint8_t velocidadActual() const { return _speedNow; }
  uint8_t velocidadObjetivo() const { return _speedTarget; }
  uint8_t pin() const { return _pinPWM; }
};

#endif

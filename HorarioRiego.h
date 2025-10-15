#ifndef HORARIORIEGO_H
#define HORARIORIEGO_H

#include <Arduino.h>

// Clase para manejar el horario y frecuencia de riego
class HorarioRiego {
  private:
    bool     habilitado;      // Indica si el horario está activo
    uint16_t startMin;        // Minutos desde medianoche donde inicia el riego
    uint16_t endMin;          // Minutos desde medianoche donde termina el riego
    uint16_t cadaNDias;       // Cada cuántos días se debe regar
    uint32_t lastStartDay;    // Último día en que se inició el riego

    // Convierte horas y minutos a minutos totales desde medianoche
    static uint16_t toMinutes(uint8_t h, uint8_t m) { return (uint16_t)h * 60 + m; }

    // Verifica si el momento actual está dentro de la ventana de riego
    static bool inWindow(uint16_t nowMin, uint16_t a, uint16_t b) {
      if (a == b) return false;           // Si la ventana está vacía
      if (a < b)  return (nowMin >= a) && (nowMin < b);      // Si la ventana no cruza medianoche
      // Si la ventana cruza medianoche (ejemplo: 22:00–02:00)
      return (nowMin >= a) || (nowMin < b);
    }

  public:
    // Constructor: define la ventana de riego y el intervalo de días
    HorarioRiego(uint8_t hIni, uint8_t mIni, uint8_t hFin, uint8_t mFin,
                 uint16_t intervaloDias = 1, bool on = true)
      : habilitado(on),
        startMin(toMinutes(hIni, mIni)),
        endMin(toMinutes(hFin, mFin)),
        cadaNDias(intervaloDias == 0 ? 1 : intervaloDias),
        lastStartDay(0) {}

    // Activa o desactiva el horario de riego
    void setHabilitado(bool on) { habilitado = on; }

    // Cambia la ventana de tiempo para el riego
    void setVentana(uint8_t hIni, uint8_t mIni, uint8_t hFin, uint8_t mFin) {
      startMin = toMinutes(hIni, mIni);
      endMin   = toMinutes(hFin, mFin);
    }

    // Cambia el intervalo de días entre riegos
    void setIntervalo(uint16_t nDias) { cadaNDias = nDias == 0 ? 1 : nDias; }

    // Determina si hoy y ahora corresponde regar
    bool debeRegar(uint32_t dayCount, uint16_t minutesNow) {
      if (!habilitado) return false;

      bool tocaHoy;
      if (lastStartDay == 0) {
        tocaHoy = (dayCount % cadaNDias) == 0;   // Primer ciclo
      } else {
        tocaHoy = ((dayCount - lastStartDay) % cadaNDias) == 0;
      }
      if (!tocaHoy) return false;

      return inWindow(minutesNow, startMin, endMin);
    }

    // Guarda el día en que se inició el riego para el próximo ciclo
    void registrarInicioRiego(uint32_t dayCount) { lastStartDay = dayCount; }
};

#endif

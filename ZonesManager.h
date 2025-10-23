#ifndef ZONAMANAGER_H
#define ZONAMANAGER_H

#include <Arduino.h>
#include "SensorDeHumedad.h"
#include "Valvula.h"
#include "HorarioRiego.h"
#include "RainGuard.h"

class ZonaManager {
private:
    SensorDeHumedad sensor;
    Valvula valvula;
    int humedadMin;
    int humedadMax;
    bool estadoRiego;
    HorarioRiego horario;
    RainGuard rainGuar;

public:
    ZonaManager(int pinSensorAnalog, int pinValvulaPWM, int min, int max)
  : sensor(pinSensorAnalog),
    valvula(pinValvulaPWM),
    humedadMin(min),
    humedadMax(max),
    estadoRiego(false),
    horario(6, 0, 7, 0, 2, true),
    rainGuar(10.0, -84.2, 70)
{
    valvula.begin();
}

    void verificarYRegar(uint32_t dayCount, uint16_t minutesNow) {
        int humedad = sensor.leerPorcentaje();
        //Serial.println(humedad);
        bool esMomentoDeRiego = horario.debeRegar(dayCount, minutesNow);
        bool bloquearPorLluvia = rainGuar.shouldBlockToday();
        //Serial.print("¿Es momento de riego? ");
        //Serial.println(esMomentoDeRiego ? "Sí" : "No");

        if (!bloquearPorLluvia) {
            apagarRiego();
            return;
        }

        if (/*humedad < humedadMin*/false) {
            encenderRiego();
            if (esMomentoDeRiego) {
                horario.registrarInicioRiego(dayCount);
            }
        }
        else if (esMomentoDeRiego && humedad < humedadMax) {
            encenderRiego();
            horario.registrarInicioRiego(dayCount);
        }
        else {
            apagarRiego();
        }


    }
    bool refreshRainGuard() {
    return rainGuar.refresh();
}

uint8_t obtenerProbabilidadLluvia() const {
    return rainGuar.cachedProbability();
}

    void encenderRiego() {
        if (!estadoRiego) {
            valvula.activarValvula(1);
            estadoRiego = true;
            Serial.println("Riego encendido.");
        }
    }

    void apagarRiego() {
        if (estadoRiego) {
            valvula.activarValvula(0);
            estadoRiego = false;
            Serial.println("Riego apagado.");
        }
    }

    void configurarHumedad(int min, int max) {
        humedadMin = min;
        humedadMax = max;
        Serial.println("Rango de humedad actualizado.");
    }

    void configurarHorario(uint16_t hIni, uint16_t mIni, uint16_t hFin, uint16_t mFin, uint16_t intervalo) {
        horario.setVentana(hIni, mIni, hFin, mFin);
        horario.setIntervalo(intervalo);
        horario.setHabilitado(true); 
    }

    int obtenerHumedad() {
        return sensor.leerPorcentaje();
    }

    bool estaRegando() {
        return estadoRiego;
    }
};

#endif


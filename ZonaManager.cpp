//Aqui se implementa la logica de la clase
#include "ZonaDeRiego.h"
#include <Arduino.h>
#include "ZonesManager.h"

class Zonemanager{
    //Constructor de la clase
    ZonaDeRiego::ZonaDeRiego(int pinSensor, int pinRiego, int min, int max, string tipo, int pinValvula)
    {
        pinMode(pinRiego, OUTPUT);
        pinMode(valvula, OUTPUT);
    }

    //Metodo 
    void ZonaDeRiego::verificarYRegar()
    {
        int humedad = sensor.leerHumedad();
        if (humedad < humedadMin) {
            digitalWrite(pinRiego, HIGH);
        } else if (humedad > humedadMax) {
            digitalWrite(pinRiego, LOW);
        }
    }
}
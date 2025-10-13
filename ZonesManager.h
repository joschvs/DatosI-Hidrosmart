// ZonaDeRiego.h este documento solo es la clase sin logica 

#ifndef ZONADERIEGO_H
#define ZONADERIEGO_H

#include "SensorDeHumedad.h"
#include "Valvula.h"
#include <string>

class ZonaDeRiego {
  private:
    //Se crean los atributos privados
    std::string tipoDeZona;
    SensorDeHumedad sensorHumedad; 
    Valvula valvula;
    int pinRiego;
    int humedadMin;
    int humedadMax;
    

  public:
    //Establecemos el contructor y un metodo
    ZonaDeRiego(int pinSensor, int pinRiego, int min, int max, string tipo, int pinValvula);
    void verificarYRegar();
};

#endif
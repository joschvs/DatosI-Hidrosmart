#ifndef SENSORDEHUMEDAD_H
#define SENSORDEHUMEDAD_H

class SensorDeHumedad {
  private:
    int pin;

  public:
    SensorDeHumedad(int p);
    int leerHumedad();
};

#endif
#ifndef VALVULA_H
#define VALVULA_H_H

class Valvula {
  private:
    int pin;

  public:
    Valvula(int p);
    int activarValvula();
};

#endif
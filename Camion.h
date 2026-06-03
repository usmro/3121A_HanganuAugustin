#ifndef CAMION_H
#define CAMION_H
#include "Vehicul.h"
class Camion : public Vehicul {
private:
    string culoare;
    string numarInmatriculare;
public:
    Camion(string culoare, string numar);
    string getCuloare();
    string getNumar();
    void afiseaza();
    char getSimbol();
    ~Camion();
};
#endif

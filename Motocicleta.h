#ifndef MOTOCICLETA_H
#define MOTOCICLETA_H
#include "Vehicul.h"
class Motocicleta : public Vehicul {
private:
    string culoare;
    string numarInmatriculare;
public:
    Motocicleta(string culoare, string numar);
    string getCuloare();
    string getNumar();
    void afiseaza();
    char getSimbol();
    ~Motocicleta();
};
#endif

#ifndef MASINA_H
#define MASINA_H

#include "Vehicul.h"

class Masina : public Vehicul {
private:
    string culoare;
    string numarInmatriculare;

public:
    Masina(string culoare, string numar);

    string getCuloare();
    string getNumar();

    void afiseaza();
    char getSimbol();

    ~Masina();
};

#endif

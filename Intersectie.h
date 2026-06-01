#ifndef INTERSECTIE_H
#define INTERSECTIE_H

#include "Banda.h"
#include "Masina.h"
#include <vector>
#include <string>
using namespace std;

class Intersectie {
private:
    string nume;
    vector<Banda*> benzi;
    vector<Semafor*> semafoare;
    int ciclu;

public:
    Intersectie(string nume);

    void initializeaza();
    void update();
    void afiseaza();
    void adaugaVehiculeRandom();
    void ruleaza(int nrCicluri);

    ~Intersectie();
};

#endif

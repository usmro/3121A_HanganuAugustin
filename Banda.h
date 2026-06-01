#ifndef BANDA_H
#define BANDA_H
#include "Vehicul.h"
#include "Semafor.h"
#include <queue>
#include <string>
using namespace std;
class Banda {
public:
    string directie;
    queue<Vehicul*> coada;
    Semafor* semafor;
    int maxMasini;
public:
    Banda(string directie, Semafor* sem);
    void adaugaVehicul(Vehicul* v);
    void avanseaza();
    void afiseaza();
    int getNrVehicule();
    string getDirectie();
    bool esteGoala();
    ~Banda();
};
#endif

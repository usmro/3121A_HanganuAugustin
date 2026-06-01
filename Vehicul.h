#ifndef VEHICUL_H
#define VEHICUL_H

#include <string>
using namespace std;

class Vehicul {
protected:
    string tip;
    int viteza;
    int pozitie;
    bool esteOprit;

public:
    Vehicul(string tip, int viteza);

    string getTip();
    int getViteza();
    int getPozitie();
    bool getEsteOprit();

    void setViteza(int v);
    void setPozitie(int p);
    void opreste();
    void porneste();

    virtual void afiseaza();
    virtual char getSimbol();

    virtual ~Vehicul();
};

#endif


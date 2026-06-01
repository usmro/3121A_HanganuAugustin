#include "Vehicul.h"
#include <iostream>
using namespace std;

Vehicul::Vehicul(string tip, int viteza) {
    this->tip = tip;
    this->viteza = viteza;
    this->pozitie = 0;
    this->esteOprit = false;
}

string Vehicul::getTip() { return tip; }
int Vehicul::getViteza() { return viteza; }
int Vehicul::getPozitie() { return pozitie; }
bool Vehicul::getEsteOprit() { return esteOprit; }

void Vehicul::setViteza(int v) { viteza = v; }
void Vehicul::setPozitie(int p) { pozitie = p; }

void Vehicul::opreste() {
    esteOprit = true;
    viteza = 0;
}

void Vehicul::porneste() {
    esteOprit = false;
}

void Vehicul::afiseaza() {
    cout << "Vehicul: " << tip << " | Viteza: " << viteza << " km/h" << endl;
}

char Vehicul::getSimbol() { return 'V'; }

Vehicul::~Vehicul() {}

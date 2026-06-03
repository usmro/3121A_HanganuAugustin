#include "Camion.h"
#include <iostream>
using namespace std;
Camion::Camion(string culoare, string numar)
    : Vehicul("Camion", 30) {
    this->culoare = culoare;
    this->numarInmatriculare = numar;
}
string Camion::getCuloare() { return culoare; }
string Camion::getNumar() { return numarInmatriculare; }
void Camion::afiseaza() {
    cout << "Camion: " << numarInmatriculare
         << " | Culoare: " << culoare
         << " | Viteza: " << viteza << " km/h" << endl;
}
char Camion::getSimbol() { return 'C'; }
Camion::~Camion() {}

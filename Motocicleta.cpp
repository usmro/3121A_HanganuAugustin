#include "Motocicleta.h"
#include <iostream>
using namespace std;
Motocicleta::Motocicleta(string culoare, string numar)
    : Vehicul("Motocicleta", 80) {
    this->culoare = culoare;
    this->numarInmatriculare = numar;
}
string Motocicleta::getCuloare() { return culoare; }
string Motocicleta::getNumar() { return numarInmatriculare; }
void Motocicleta::afiseaza() {
    cout << "Motocicleta: " << numarInmatriculare
         << " | Culoare: " << culoare
         << " | Viteza: " << viteza << " km/h" << endl;
}
char Motocicleta::getSimbol() { return 'M'; }
Motocicleta::~Motocicleta() {}

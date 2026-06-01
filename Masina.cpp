#include "Masina.h"
#include <iostream>
using namespace std;

Masina::Masina(string culoare, string numar)
    : Vehicul("Masina", 50) {
    this->culoare = culoare;
    this->numarInmatriculare = numar;
}

string Masina::getCuloare() { return culoare; }
string Masina::getNumar() { return numarInmatriculare; }

void Masina::afiseaza() {
    cout << "Masina: " << numarInmatriculare
         << " | Culoare: " << culoare
         << " | Viteza: " << viteza << " km/h" << endl;
}

char Masina::getSimbol() { return 'M'; }

Masina::~Masina() {}

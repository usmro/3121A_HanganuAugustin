#include "Banda.h"
#include <iostream>
using namespace std;

Banda::Banda(string directie, Semafor* sem) {
    this->directie = directie;
    this->semafor = sem;
    this->maxMasini = 10;
}

void Banda::adaugaVehicul(Vehicul* v) {
    if ((int)coada.size() < maxMasini) {
        coada.push(v);
    }
}

void Banda::avanseaza() {
    if (!coada.empty() && semafor->esteVerde()) {
        Vehicul* v = coada.front();
        cout << "Trece: ";
        v->afiseaza();
        coada.pop();
        delete v;
    }
}

void Banda::afiseaza() {
    cout << "Banda " << directie
         << " | Vehicule: " << (int)coada.size()
         << " | Semafor: " << semafor->getCuloareString() << endl;
}

int Banda::getNrVehicule() { return (int)coada.size(); }
string Banda::getDirectie() { return directie; }
bool Banda::esteGoala() { return coada.empty(); }

Banda::~Banda() {
    while (!coada.empty()) {
        delete coada.front();
        coada.pop();
    }
}

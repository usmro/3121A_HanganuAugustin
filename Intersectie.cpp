#include "Intersectie.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
using namespace std;

Intersectie::Intersectie(string nume) {
    this->nume = nume;
    this->ciclu = 0;
    srand(time(0));
}

void Intersectie::initializeaza() {
    // Cream 4 semafoare pentru 4 directii
    semafoare.push_back(new Semafor(5, 2, 4)); // Nord
    semafoare.push_back(new Semafor(5, 2, 4)); // Sud
    semafoare.push_back(new Semafor(5, 2, 4)); // Est
    semafoare.push_back(new Semafor(5, 2, 4)); // Vest

    // Cream 4 benzi
    benzi.push_back(new Banda("Nord", semafoare[0]));
    benzi.push_back(new Banda("Sud",  semafoare[1]));
    benzi.push_back(new Banda("Est",  semafoare[2]));
    benzi.push_back(new Banda("Vest", semafoare[3]));
}

void Intersectie::adaugaVehiculeRandom() {
    string culori[] = {"Rosu", "Albastru", "Alb", "Negru", "Gri"};
    string numere[] = {"B-01-ABC", "CJ-22-XYZ", "TM-33-DEF", "IS-44-GHI"};

    for (int i = 0; i < 4; i++) {
        if (rand() % 2 == 0) {
            string culoare = culori[rand() % 5];
            string numar = numere[rand() % 4];
            benzi[i]->adaugaVehicul(new Masina(culoare, numar));
        }
    }
}

void Intersectie::update() {
    ciclu++;

    // Actualizam semafoarele
    for (int i = 0; i < semafoare.size(); i++) {
        semafoare[i]->setMasiniAsteptare(benzi[i]->getNrVehicule());
        semafoare[i]->ajusteazaTimpi();
        semafoare[i]->update();
    }

    // Avanseaza vehiculele
    for (int i = 0; i < benzi.size(); i++) {
        benzi[i]->avanseaza();
    }

    // Adaugam vehicule noi random
    adaugaVehiculeRandom();
}

void Intersectie::afiseaza() {
    system("cls");
    cout << "========================================" << endl;
    cout << "   SIMULATOR INTERSECTIE: " << nume << endl;
    cout << "   Ciclu: " << ciclu << endl;
    cout << "========================================" << endl;

    for (int i = 0; i < benzi.size(); i++) {
        benzi[i]->afiseaza();
    }
    cout << "========================================" << endl;
}

void Intersectie::ruleaza(int nrCicluri) {
    initializeaza();

    for (int i = 0; i < nrCicluri; i++) {
        adaugaVehiculeRandom();
        update();
        afiseaza();
        Sleep(1000); // pauza 1 secunda intre cicluri
    }

    cout << "\nSimularea s-a incheiat!" << endl;
}

Intersectie::~Intersectie() {
    for (auto b : benzi) delete b;
    for (auto s : semafoare) delete s;
}

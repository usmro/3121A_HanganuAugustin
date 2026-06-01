#include "Semafor.h"
using namespace std;

Semafor::Semafor(int tRosu, int tGalben, int tVerde) {
    culoareCurenta = ROSU;
    timpRosu = tRosu;
    timpGalben = tGalben;
    timpVerde = tVerde;
    contor = 0;
    nrMasiniAsteptare = 0;
}

void Semafor::update() {
    contor++;
    if (culoareCurenta == ROSU && contor >= timpRosu) {
        culoareCurenta = VERDE;
        contor = 0;
    } else if (culoareCurenta == VERDE && contor >= timpVerde) {
        culoareCurenta = GALBEN;
        contor = 0;
    } else if (culoareCurenta == GALBEN && contor >= timpGalben) {
        culoareCurenta = ROSU;
        contor = 0;
    }
}

void Semafor::setMasiniAsteptare(int n) {
    nrMasiniAsteptare = n;
}

void Semafor::ajusteazaTimpi() {
    if (nrMasiniAsteptare > 5) {
        timpVerde = 10;
    } else if (nrMasiniAsteptare > 2) {
        timpVerde = 6;
    } else {
        timpVerde = 4;
    }
}

Semafor::Culoare Semafor::getCuloare() { return culoareCurenta; }

string Semafor::getCuloareString() {
    if (culoareCurenta == ROSU)   return "ROSU";
    if (culoareCurenta == GALBEN) return "GALBEN";
    return "VERDE";
}

char Semafor::getSimbol() {
    if (culoareCurenta == ROSU)   return 'R';
    if (culoareCurenta == GALBEN) return 'G';
    return 'V';
}

bool Semafor::esteVerde() { return culoareCurenta == VERDE; }

Semafor::~Semafor() {}

#ifndef SEMAFOR_H
#define SEMAFOR_H
#include <string>
using namespace std;
class Semafor {
public:
    enum Culoare { ROSU, GALBEN, VERDE };
private:
    Culoare culoareCurenta;
    int timpRosu;
    int timpGalben;
    int timpVerde;
    int contor;
    int nrMasiniAsteptare;
public:
    Semafor(int tRosu, int tGalben, int tVerde);
    void update();
    void setMasiniAsteptare(int n);
    void ajusteazaTimpi();
    void setCuloare(Culoare c) { culoareCurenta = c; }
    Culoare getCuloare();
    string getCuloareString();
    char getSimbol();
    bool esteVerde();
    ~Semafor();
};
#endif

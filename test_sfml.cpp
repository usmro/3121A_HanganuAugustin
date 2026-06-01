#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Intersectie.h"
#include "Masina.h"
#include "Banda.h"
#include "Semafor.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>

using namespace std;

string intToStr(int x) {
    ostringstream ss; ss << x; return ss.str();
}

sf::Color culoriVizuale[] = {
    sf::Color(220,50,50),
    sf::Color(50,100,220),
    sf::Color(220,220,220),
    sf::Color(40,40,40),
    sf::Color(150,150,150),
    sf::Color(50,180,80),
    sf::Color(220,180,50)
};
string numareMasini[] = {
    "B-01-ABC","CJ-22-XYZ","TM-33-DEF",
    "IS-44-GHI","SV-11-KKK","BT-55-ZZZ"
};
string culoriNume[] = {
    "Rosu","Albastru","Alb","Negru","Gri","Verde","Galben"
};

struct MasinaViz {
    sf::Vector2f pozitie;
    sf::Vector2f target;
    sf::Color culoare;
    MasinaViz(sf::Vector2f p, sf::Vector2f t, sf::Color c)
        : pozitie(p), target(t), culoare(c) {}
};

int main() {
    srand((unsigned)time(0));

    sf::RenderWindow window(
        sf::VideoMode({900, 700}),
        "Simulator Trafic Rutier - Piata Unirii"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        font.openFromFile("C:/Windows/Fonts/calibri.ttf");

    // Semafoare si benzi
    vector<Semafor*> semafoare;
    semafoare.push_back(new Semafor(5,2,4));
    semafoare.push_back(new Semafor(5,2,4));
    semafoare.push_back(new Semafor(5,2,4));
    semafoare.push_back(new Semafor(5,2,4));

    vector<Banda*> benzi;
    benzi.push_back(new Banda("Nord", semafoare[0]));
    benzi.push_back(new Banda("Sud",  semafoare[1]));
    benzi.push_back(new Banda("Est",  semafoare[2]));
    benzi.push_back(new Banda("Vest", semafoare[3]));

    // Stare initiala: Nord-Sud VERDE, Est-Vest ROSU
    semafoare[0]->setCuloare(Semafor::VERDE);
    semafoare[1]->setCuloare(Semafor::VERDE);
    semafoare[2]->setCuloare(Semafor::ROSU);
    semafoare[3]->setCuloare(Semafor::ROSU);

    bool running = false;
    int ciclu = 0, totalTrecute = 0;
    sf::Clock simClock, frameClock;
    float interval = 1.0f;

    // Faze semafoare: 0=NS verde, 1=toti galben, 2=EV verde, 3=toti galben
    int faza = 0;
    int contorFaza = 0;
    int durateFaze[] = {6, 2, 6, 2};

    vector<MasinaViz*> masiniAnimate;
    vector<vector<sf::Color>> culoriBenzi(4);

    float cx = 420.f, cy = 320.f, roadW = 70.f;

    auto getPozBanda = [&](int bi, int idx) -> sf::Vector2f {
        float off = 30.f + idx * 38.f;
        switch(bi) {
            case 0: return {cx-15.f, cy-roadW/2-off};
            case 1: return {cx+15.f, cy+roadW/2+off};
            case 2: return {cx+roadW/2+off, cy+15.f};
            case 3: return {cx-roadW/2-off, cy-15.f};
        }
        return {cx,cy};
    };

    auto getIesire = [&](int bi) -> sf::Vector2f {
        switch(bi) {
            case 0: return {cx-15.f, cy+roadW/2+220.f};
            case 1: return {cx+15.f, cy-roadW/2-220.f};
            case 2: return {cx-roadW/2-220.f, cy+15.f};
            case 3: return {cx+roadW/2+220.f, cy-15.f};
        }
        return {cx,cy};
    };

    auto adaugaRandom = [&]() {
        for (int i=0;i<4;i++) {
            if (rand()%2==0 && benzi[i]->getNrVehicule()<8) {
                int ci = rand()%7;
                int ni = rand()%6;
                benzi[i]->adaugaVehicul(new ::Masina(culoriNume[ci], numareMasini[ni]));
                culoriBenzi[i].push_back(culoriVizuale[ci]);
            }
        }
    };

    struct Buton {
        sf::RectangleShape forma;
        string label;
    };

    auto makeButon = [&](string txt, float x, float y) -> Buton {
        Buton b;
        b.forma.setSize({120.f,40.f});
        b.forma.setPosition({x,y});
        b.forma.setFillColor(sf::Color(60,60,80));
        b.forma.setOutlineColor(sf::Color(120,120,180));
        b.forma.setOutlineThickness(2.f);
        b.label = txt;
        return b;
    };

    auto drawButon = [&](Buton& b) {
        window.draw(b.forma);
        sf::Text t(font, b.label, 16);
        t.setFillColor(sf::Color::White);
        auto p = b.forma.getPosition();
        t.setPosition({p.x+10.f, p.y+10.f});
        window.draw(t);
    };

    auto inButon = [](Buton& b, sf::Vector2i mp) -> bool {
        auto p = b.forma.getPosition();
        auto s = b.forma.getSize();
        return mp.x>=p.x && mp.x<=p.x+s.x && mp.y>=p.y && mp.y<=p.y+s.y;
    };

    Buton btnStart = makeButon("START",   760.f, 30.f);
    Buton btnReset = makeButon("RESET",   760.f, 80.f);
    Buton btnPlus  = makeButon("Viteza+", 760.f, 130.f);
    Buton btnMinus = makeButon("Viteza-", 760.f, 180.f);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* me = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mp = {me->position.x, me->position.y};
                if (inButon(btnStart, mp)) {
                    running = !running;
                    btnStart.label = running ? "PAUZA" : "START";
                    simClock.restart();
                }
                if (inButon(btnReset, mp)) {
                    running = false;
                    btnStart.label = "START";
                    ciclu = 0; totalTrecute = 0;
                    faza = 0; contorFaza = 0;
                    for (int i=0;i<4;i++) {
                        while (!benzi[i]->esteGoala()) {
                            delete benzi[i]->coada.front();
                            benzi[i]->coada.pop();
                        }
                        culoriBenzi[i].clear();
                    }
                    for (auto m:masiniAnimate) delete m;
                    masiniAnimate.clear();
                    // Reset stare semafoare
                    semafoare[0]->setCuloare(Semafor::VERDE);
                    semafoare[1]->setCuloare(Semafor::VERDE);
                    semafoare[2]->setCuloare(Semafor::ROSU);
                    semafoare[3]->setCuloare(Semafor::ROSU);
                }
                if (inButon(btnPlus,  mp)) interval = max(0.2f, interval-0.2f);
                if (inButon(btnMinus, mp)) interval = min(3.0f, interval+0.2f);
            }
        }

        float dt = frameClock.restart().asSeconds();

        if (running && simClock.getElapsedTime().asSeconds() >= interval) {
            simClock.restart();
            ciclu++;
            adaugaRandom();

            // ---- Update faze semafoare sincronizat ----
            contorFaza++;
            if (contorFaza >= durateFaze[faza]) {
                contorFaza = 0;
                faza = (faza + 1) % 4;
            }

            if (faza == 0) {
                semafoare[0]->setCuloare(Semafor::VERDE);
                semafoare[1]->setCuloare(Semafor::VERDE);
                semafoare[2]->setCuloare(Semafor::ROSU);
                semafoare[3]->setCuloare(Semafor::ROSU);
            } else if (faza == 1) {
                for (int i=0;i<4;i++) semafoare[i]->setCuloare(Semafor::GALBEN);
            } else if (faza == 2) {
                semafoare[0]->setCuloare(Semafor::ROSU);
                semafoare[1]->setCuloare(Semafor::ROSU);
                semafoare[2]->setCuloare(Semafor::VERDE);
                semafoare[3]->setCuloare(Semafor::VERDE);
            } else {
                for (int i=0;i<4;i++) semafoare[i]->setCuloare(Semafor::GALBEN);
            }

            // Ajustare timpi inteligenta (mai mult verde daca e coada mare)
            for (int i=0;i<4;i++)
                semafoare[i]->setMasiniAsteptare(benzi[i]->getNrVehicule());

            // Avanseaza masini doar daca e verde
            for (int i=0;i<4;i++) {
                if (!benzi[i]->esteGoala() && semafoare[i]->esteVerde()) {
                    sf::Color c = culoriBenzi[i].empty() ?
                        sf::Color(180,180,180) : culoriBenzi[i].front();
                    if (!culoriBenzi[i].empty())
                        culoriBenzi[i].erase(culoriBenzi[i].begin());

                    MasinaViz* mv = new MasinaViz(getPozBanda(i,0), getIesire(i), c);
                    masiniAnimate.push_back(mv);

                    delete benzi[i]->coada.front();
                    benzi[i]->coada.pop();
                    totalTrecute++;
                }
            }
        }

        // Animatie masini
        for (auto it=masiniAnimate.begin(); it!=masiniAnimate.end();) {
            MasinaViz* m = *it;
            sf::Vector2f d = m->target - m->pozitie;
            float dist = sqrt(d.x*d.x+d.y*d.y);
            if (dist < 3.f) { delete m; it = masiniAnimate.erase(it); }
            else { m->pozitie += (d/dist)*150.f*dt; ++it; }
        }

        // ===== DESENARE =====
        window.clear(sf::Color(34,85,34));

        // Drumuri
        sf::RectangleShape drumH({730.f,roadW});
        drumH.setPosition({0.f,cy-roadW/2});
        drumH.setFillColor(sf::Color(70,70,70));
        window.draw(drumH);

        sf::RectangleShape drumV({roadW,700.f});
        drumV.setPosition({cx-roadW/2,0.f});
        drumV.setFillColor(sf::Color(70,70,70));
        window.draw(drumV);

        sf::RectangleShape centru({roadW,roadW});
        centru.setPosition({cx-roadW/2,cy-roadW/2});
        centru.setFillColor(sf::Color(80,80,80));
        window.draw(centru);

        // Marcaje drum orizontal
        for (float x=0;x<730.f;x+=40.f) {
            if (x>cx-roadW/2-10 && x<cx+roadW/2+10) continue;
            sf::RectangleShape l({20.f,3.f});
            l.setPosition({x,cy-1.5f});
            l.setFillColor(sf::Color(255,255,255,150));
            window.draw(l);
        }
        // Marcaje drum vertical
        for (float y=0;y<700.f;y+=40.f) {
            if (y>cy-roadW/2-10 && y<cy+roadW/2+10) continue;
            sf::RectangleShape l({3.f,20.f});
            l.setPosition({cx-1.5f,y});
            l.setFillColor(sf::Color(255,255,255,150));
            window.draw(l);
        }

        // Semafoare
        float semPos[4][2]={
            {cx+roadW/2+5,  cy-roadW/2-62},
            {cx-roadW/2-25, cy+roadW/2+5},
            {cx+roadW/2+5,  cy+roadW/2+5},
            {cx-roadW/2-25, cy-roadW/2-62}
        };
        for (int i=0;i<4;i++) {
            float sx=semPos[i][0], sy=semPos[i][1];
            sf::RectangleShape corp({20.f,58.f});
            corp.setPosition({sx,sy});
            corp.setFillColor(sf::Color(25,25,25));
            corp.setOutlineColor(sf::Color(80,80,80));
            corp.setOutlineThickness(1.f);
            window.draw(corp);

            Semafor::Culoare c = semafoare[i]->getCuloare();
            sf::CircleShape l1(7.f),l2(7.f),l3(7.f);
            l1.setPosition({sx+3,sy+3});
            l2.setPosition({sx+3,sy+22});
            l3.setPosition({sx+3,sy+41});
            l1.setFillColor(c==Semafor::ROSU   ?sf::Color(240,50,50) :sf::Color(80,20,20));
            l2.setFillColor(c==Semafor::GALBEN  ?sf::Color(240,210,50):sf::Color(80,70,20));
            l3.setFillColor(c==Semafor::VERDE   ?sf::Color(50,220,80) :sf::Color(20,70,30));
            window.draw(l1); window.draw(l2); window.draw(l3);
        }

        // Masini in coada
        for (int i=0;i<4;i++) {
            queue<Vehicul*> tmp = benzi[i]->coada;
            int idx=0, cidx=0;
            while (!tmp.empty()) {
                sf::Vector2f pos = getPozBanda(i,idx);
                sf::Color c = (cidx<(int)culoriBenzi[i].size()) ?
                    culoriBenzi[i][cidx] : sf::Color(180,180,180);

                sf::RectangleShape car;
                if (i==0||i==1) {
                    car.setSize({18.f,28.f});
                    car.setPosition({pos.x-9.f,pos.y-14.f});
                } else {
                    car.setSize({28.f,18.f});
                    car.setPosition({pos.x-14.f,pos.y-9.f});
                }
                car.setFillColor(c);
                car.setOutlineColor(sf::Color(0,0,0,180));
                car.setOutlineThickness(1.f);
                window.draw(car);

                sf::RectangleShape geam;
                if (i==0||i==1) { geam.setSize({12.f,8.f}); geam.setPosition({pos.x-6.f,pos.y-4.f}); }
                else             { geam.setSize({8.f,12.f}); geam.setPosition({pos.x-4.f,pos.y-6.f}); }
                geam.setFillColor(sf::Color(150,200,255,180));
                window.draw(geam);
                tmp.pop(); idx++; cidx++;
            }
        }

        // Masini animate
        for (MasinaViz* m:masiniAnimate) {
            sf::RectangleShape car({26.f,16.f});
            car.setOrigin({13.f,8.f});
            car.setPosition(m->pozitie);
            car.setFillColor(m->culoare);
            car.setOutlineColor(sf::Color::Black);
            car.setOutlineThickness(1.f);
            window.draw(car);
        }

        // Etichete directii
        string dirLabel[]={"NORD","SUD","EST","VEST"};
        sf::Vector2f dirPos[]={
            {cx+roadW/2+8,  cy-roadW/2-95},
            {cx-roadW/2-48, cy+roadW/2+68},
            {cx+roadW/2+8,  cy+roadW/2+68},
            {cx-roadW/2-48, cy-roadW/2-95}
        };
        for (int i=0;i<4;i++) {
            sf::Text lbl(font,dirLabel[i],13);
            lbl.setFillColor(sf::Color(200,200,200));
            lbl.setPosition(dirPos[i]);
            window.draw(lbl);

            sf::Text cnt(font,intToStr(benzi[i]->getNrVehicule())+" mas.",12);
            cnt.setFillColor(sf::Color(180,220,180));
            cnt.setPosition({dirPos[i].x,dirPos[i].y+16.f});
            window.draw(cnt);

            sf::Color tc=sf::Color::White;
            if (semafoare[i]->getCuloare()==Semafor::ROSU)   tc=sf::Color(230,80,80);
            if (semafoare[i]->getCuloare()==Semafor::GALBEN) tc=sf::Color(230,200,80);
            if (semafoare[i]->getCuloare()==Semafor::VERDE)  tc=sf::Color(80,210,80);
            sf::Text semTxt(font,semafoare[i]->getCuloareString(),11);
            semTxt.setFillColor(tc);
            semTxt.setPosition({dirPos[i].x,dirPos[i].y+30.f});
            window.draw(semTxt);
        }

        // Panou dreapta
        sf::RectangleShape panou({160.f,700.f});
        panou.setPosition({730.f,0.f});
        panou.setFillColor(sf::Color(20,20,35));
        window.draw(panou);

        sf::Text titlu(font,"STATISTICI",16);
        titlu.setFillColor(sf::Color(150,180,255));
        titlu.setPosition({742.f,8.f});
        window.draw(titlu);

        drawButon(btnStart);
        drawButon(btnReset);
        drawButon(btnPlus);
        drawButon(btnMinus);

        sf::Text vitTxt(font,"Viteza:"+intToStr((int)(1.f/interval*10))+"x",13);
        vitTxt.setFillColor(sf::Color(180,180,180));
        vitTxt.setPosition({748.f,230.f});
        window.draw(vitTxt);

        // Faza curenta
        string fazaStr[] = {"NS: VERDE","Tranzitie","EV: VERDE","Tranzitie"};
        sf::Text fazaTxt(font, fazaStr[faza], 12);
        fazaTxt.setFillColor(sf::Color(180,220,255));
        fazaTxt.setPosition({742.f,252.f});
        window.draw(fazaTxt);

        vector<pair<string,string>> stats={
            {"Ciclu:",   intToStr(ciclu)},
            {"Trecute:", intToStr(totalTrecute)},
            {"In coada:",intToStr(
                benzi[0]->getNrVehicule()+benzi[1]->getNrVehicule()+
                benzi[2]->getNrVehicule()+benzi[3]->getNrVehicule())},
            {"Nord:", intToStr(benzi[0]->getNrVehicule())+" mas."},
            {"Sud:",  intToStr(benzi[1]->getNrVehicule())+" mas."},
            {"Est:",  intToStr(benzi[2]->getNrVehicule())+" mas."},
            {"Vest:", intToStr(benzi[3]->getNrVehicule())+" mas."},
        };
        float yS=290.f;
        for (auto& s:stats) {
            sf::Text k(font,s.first,13);
            k.setFillColor(sf::Color(160,160,200));
            k.setPosition({748.f,yS});
            window.draw(k);
            sf::Text v(font,s.second,13);
            v.setFillColor(sf::Color::White);
            v.setPosition({840.f,yS});
            window.draw(v);
            yS+=24.f;
        }

        sf::Text status(font,running?">>> RULARE":"--- PAUZA",14);
        status.setFillColor(running?sf::Color(80,220,80):sf::Color(220,100,100));
        status.setPosition({745.f,480.f});
        window.draw(status);

        sf::Text jos(font,"Piata Unirii",14);
        jos.setFillColor(sf::Color(120,120,160));
        jos.setPosition({745.f,650.f});
        window.draw(jos);

        window.display();
    }

    for (auto b:benzi) delete b;
    for (auto s:semafoare) delete s;
    for (auto m:masiniAnimate) delete m;
    return 0;
}

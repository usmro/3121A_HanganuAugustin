#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Intersectie.h"
#include "Masina.h"
#include "Camion.h"
#include "Motocicleta.h"
#include "Banda.h"
#include "Semafor.h"
#include <string>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>
using namespace std;

string intToStr(int x){ostringstream s;s<<x;return s.str();}
string floatToStr(float x,int d=1){ostringstream s;s.precision(d);s<<fixed<<x;return s.str();}

enum TipViz{TIP_MASINA,TIP_CAMION,TIP_MOTO};

sf::Color culoriVizuale[]={
    sf::Color(200,40,40),sf::Color(40,90,200),sf::Color(210,210,210),
    sf::Color(40,40,40),sf::Color(130,130,130),sf::Color(40,160,70),sf::Color(200,160,40)
};
string numareMasini[]={"B-01-ABC","CJ-22-XYZ","TM-33-DEF","IS-44-GHI","SV-11-KKK","BT-55-ZZZ"};
string culoriNume[]  ={"Rosu","Albastru","Alb","Negru","Gri","Verde","Galben"};

struct VehicolInfo{sf::Color culoare;TipViz tip;};

struct Vehicul2D {
    sf::Vector2f pozitie, tinta;
    sf::Color culoare;
    TipViz tip;
    float viteza, vitezaMax;
    int banda;
    bool traverseaza;
    Vehicul2D(sf::Vector2f p,sf::Color c,TipViz t,float vmax,int b)
        :pozitie(p),culoare(c),tip(t),viteza(0.f),vitezaMax(vmax),banda(b),traverseaza(false){}
};

struct Pieton {
    sf::Vector2f pozitie, tinta;
    sf::Color culoare, haine;
    float viteza;
    int directie; // 0=orizontal, 1=vertical
    float legTimer;
    Pieton(sf::Vector2f p,sf::Vector2f t,sf::Color c,sf::Color h,int dir)
        :pozitie(p),tinta(t),culoare(c),haine(h),viteza(0.f),directie(dir),legTimer(0.f){}
};

struct PietonTrotuar {
    sf::Vector2f pozitie, tinta;
    sf::Color culoare, haine;
    float viteza, legTimer;
    PietonTrotuar(sf::Vector2f p, sf::Vector2f t, sf::Color c, sf::Color h)
        :pozitie(p),tinta(t),culoare(c),haine(h),viteza(38.f),legTimer(0.f){}
};

sf::Color culoriPiele[]={sf::Color(255,210,160),sf::Color(210,160,110),sf::Color(160,110,70),sf::Color(240,190,140)};
sf::Color culoriHaine[]={sf::Color(50,80,180),sf::Color(180,50,50),sf::Color(50,150,80),sf::Color(150,100,50),sf::Color(100,50,150),sf::Color(50,130,150)};

// ============================================================
// DESENARE VEHICUL
// banda: 0=Nord(vine de sus->merge jos), 1=Sud(vine de jos->merge sus)
//        2=Est(vine din dreapta->merge stanga), 3=Vest(vine din stanga->merge dreapta)
// ============================================================
void deseneazaVehicul(sf::RenderWindow& w, sf::Vector2f pos, sf::Color c, TipViz tip, int banda){
    bool vert=(banda==0||banda==1);

    if(tip==TIP_MASINA){
        sf::RectangleShape body(vert?sf::Vector2f{18.f,28.f}:sf::Vector2f{28.f,18.f});
        body.setOrigin(body.getSize()/2.f); body.setPosition(pos);
        body.setFillColor(c); body.setOutlineColor(sf::Color(0,0,0,130)); body.setOutlineThickness(1.f);
        w.draw(body);
        sf::RectangleShape geam(vert?sf::Vector2f{12.f,9.f}:sf::Vector2f{9.f,12.f});
        geam.setOrigin(geam.getSize()/2.f); geam.setPosition(pos);
        geam.setFillColor(sf::Color(170,215,255,200)); w.draw(geam);
        sf::CircleShape r(2.5f); r.setFillColor(sf::Color(25,25,25));
        if(vert){
            r.setPosition({pos.x-11.f,pos.y-9.f});w.draw(r);
            r.setPosition({pos.x+5.f, pos.y-9.f});w.draw(r);
            r.setPosition({pos.x-11.f,pos.y+4.f});w.draw(r);
            r.setPosition({pos.x+5.f, pos.y+4.f});w.draw(r);
        }else{
            r.setPosition({pos.x-9.f,pos.y-11.f});w.draw(r);
            r.setPosition({pos.x-9.f,pos.y+5.f}); w.draw(r);
            r.setPosition({pos.x+4.f,pos.y-11.f});w.draw(r);
            r.setPosition({pos.x+4.f,pos.y+5.f}); w.draw(r);
        }
    }
    else if(tip==TIP_CAMION){
        // Remorca si cabina pozitionate corect pe directia de mers
        // banda 0=Nord: merge in jos => cabina jos, remorca sus
        // banda 1=Sud: merge in sus => cabina sus, remorca jos
        // banda 2=Est: merge spre stanga => cabina stanga, remorca dreapta
        // banda 3=Vest: merge spre dreapta => cabina dreapta, remorca stanga
        sf::Vector2f offCab, offRem;
        if(banda==0){offCab={0.f,12.f}; offRem={0.f,-14.f};} // Nord: merge jos
        if(banda==1){offCab={0.f,-12.f};offRem={0.f,14.f}; } // Sud: merge sus
        if(banda==2){offCab={-12.f,0.f};offRem={14.f,0.f}; } // Est: merge stanga
        if(banda==3){offCab={12.f,0.f}; offRem={-14.f,0.f};} // Vest: merge dreapta

        // Remorca
        sf::RectangleShape rem(vert?sf::Vector2f{20.f,24.f}:sf::Vector2f{24.f,20.f});
        rem.setOrigin(rem.getSize()/2.f);
        rem.setPosition({pos.x+offRem.x, pos.y+offRem.y});
        rem.setFillColor(sf::Color(min(255,(int)c.r+40),min(255,(int)c.g+30),min(255,(int)c.b+10)));
        rem.setOutlineColor(sf::Color(0,0,0,120)); rem.setOutlineThickness(1.f);
        w.draw(rem);

        // Cabina
        sf::RectangleShape cab(vert?sf::Vector2f{20.f,16.f}:sf::Vector2f{16.f,20.f});
        cab.setOrigin(cab.getSize()/2.f);
        cab.setPosition({pos.x+offCab.x, pos.y+offCab.y});
        cab.setFillColor(c); cab.setOutlineColor(sf::Color(0,0,0,180)); cab.setOutlineThickness(1.5f);
        w.draw(cab);

        sf::RectangleShape geam(vert?sf::Vector2f{12.f,7.f}:sf::Vector2f{7.f,12.f});
        geam.setOrigin(geam.getSize()/2.f);
        geam.setPosition({pos.x+offCab.x, pos.y+offCab.y});
        geam.setFillColor(sf::Color(170,215,255,200)); w.draw(geam);
    }
    else{ // MOTO
        sf::RectangleShape body(vert?sf::Vector2f{9.f,20.f}:sf::Vector2f{20.f,9.f});
        body.setOrigin(body.getSize()/2.f); body.setPosition(pos);
        body.setFillColor(c); body.setOutlineColor(sf::Color(0,0,0,160)); body.setOutlineThickness(1.f);
        w.draw(body);
        sf::CircleShape cap(3.5f); cap.setOrigin({3.5f,3.5f}); cap.setPosition(pos);
        cap.setFillColor(sf::Color(35,35,35)); w.draw(cap);
        sf::CircleShape r(2.5f); r.setFillColor(sf::Color(20,20,20));
        if(vert){
            r.setPosition({pos.x-2.5f,pos.y-12.f});w.draw(r);
            r.setPosition({pos.x-2.5f,pos.y+7.f}); w.draw(r);
        }else{
            r.setPosition({pos.x-12.f,pos.y-2.5f});w.draw(r);
            r.setPosition({pos.x+7.f, pos.y-2.5f});w.draw(r);
        }
    }
}

void deseneazaPieton(sf::RenderWindow& w, Pieton& p){
    float lx=sin(p.legTimer*8.f)*3.f;
    sf::RectangleShape pic1({2.f,7.f}),pic2({2.f,7.f});
    pic1.setOrigin({1.f,0.f});pic2.setOrigin({1.f,0.f});
    pic1.setPosition({p.pozitie.x-2.f+lx,p.pozitie.y+5.f});
    pic2.setPosition({p.pozitie.x+1.f-lx,p.pozitie.y+5.f});
    pic1.setFillColor(sf::Color(50,35,15));pic2.setFillColor(sf::Color(50,35,15));
    w.draw(pic1);w.draw(pic2);
    sf::RectangleShape corp({6.f,8.f});corp.setOrigin({3.f,0.f});
    corp.setPosition({p.pozitie.x,p.pozitie.y-1.f});corp.setFillColor(p.haine);w.draw(corp);
    sf::CircleShape cap(4.5f);cap.setOrigin({4.5f,4.5f});cap.setPosition(p.pozitie);
    cap.setFillColor(p.culoare);cap.setOutlineColor(sf::Color(0,0,0,80));cap.setOutlineThickness(0.5f);
    w.draw(cap);
}

int main(){
    srand((unsigned)time(0));
    sf::RenderWindow window(sf::VideoMode({920,720}),"Simulator Trafic - Piata Unirii");
    window.setFramerateLimit(60);

    sf::Font font;
    if(!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        font.openFromFile("C:/Windows/Fonts/calibri.ttf");

    vector<Semafor*> semafoare;
    for(int i=0;i<4;i++) semafoare.push_back(new Semafor(5,2,4));
    vector<Banda*> benzi;
    benzi.push_back(new Banda("Nord",semafoare[0]));
    benzi.push_back(new Banda("Sud", semafoare[1]));
    benzi.push_back(new Banda("Est", semafoare[2]));
    benzi.push_back(new Banda("Vest",semafoare[3]));

    float cx=430.f,cy=330.f,roadW=72.f;

    // Linia de stop e DUPA trecerea de pietoni (mai departe de intersectie)
    // Trecerea de pietoni e la ~10px de marginea drumului
    // Linia de stop e la ~25px de marginea drumului
    // Astfel masinile se opresc INAINTE de trecerea de pietoni
    float stopDist=roadW/2+45.f; // distanta de la centrul intersectiei la linia de stop (dupa trecere)

    auto getIntrare=[&](int bi)->sf::Vector2f{
        switch(bi){
            case 0:return{cx-18.f,-40.f};   // Nord
            case 1:return{cx+18.f,760.f};   // Sud
            case 2:return{960.f,cy+18.f};   // Est
            case 3:return{-40.f,cy-18.f};   // Vest
        }return{cx,cy};
    };

    // Linia de stop - unde se opreste vehiculul (INAINTE de trecerea de pietoni)
    auto getStopPos=[&](int bi)->sf::Vector2f{
        switch(bi){
            case 0:return{cx-18.f,cy-stopDist};   // Nord: se opreste sus
            case 1:return{cx+18.f,cy+stopDist};   // Sud: se opreste jos
            case 2:return{cx+stopDist,cy+18.f};   // Est: se opreste dreapta
            case 3:return{cx-stopDist,cy-18.f};   // Vest: se opreste stanga
        }return{cx,cy};
    };

    // Iesire - unde dispare vehiculul dupa intersectie
    auto getIesire=[&](int bi)->sf::Vector2f{
        switch(bi){
            case 0:return{cx-18.f,760.f};
            case 1:return{cx+18.f,-40.f};
            case 2:return{-40.f,cy+18.f};
            case 3:return{960.f,cy-18.f};
        }return{cx,cy};
    };

    // Directia de mers pe fiecare banda
    auto getDirBanda=[](int bi)->sf::Vector2f{
        switch(bi){
            case 0:return{0.f,1.f};   // Nord: merge in jos
            case 1:return{0.f,-1.f};  // Sud: merge in sus
            case 2:return{-1.f,0.f};  // Est: merge spre stanga
            case 3:return{1.f,0.f};   // Vest: merge spre dreapta
        }return{0.f,0.f};
    };

    // Faza 0: NS verde (0,1 verde, 2,3 rosu) - pietoni EV pot traversa
    // Faza 1: galben
    // Faza 2: EV verde (2,3 verde, 0,1 rosu) - pietoni NS pot traversa
    // Faza 3: galben
    int faza=0,contorFaza=0;
    int durateFaze[]={6,2,6,2};

    // Setare initiala: faza 0 = NS verde
    semafoare[0]->setCuloare(Semafor::VERDE);
    semafoare[1]->setCuloare(Semafor::VERDE);
    semafoare[2]->setCuloare(Semafor::ROSU);
    semafoare[3]->setCuloare(Semafor::ROSU);

    bool running=false;
    int ciclu=0,totalTrecute=0,totalMasini=0,totalCamioane=0,totalMoto=0,totalPietoni=0;
    sf::Clock simClock,frameClock;
    float interval=1.0f,timpTotal=0.f;

    vector<deque<Vehicul2D*>> vehiculeBanda(4);
    vector<vector<VehicolInfo>> infoBenzi(4);
    vector<Vehicul2D*> vehiculeTraverseaza;
    vector<Pieton*> pietoni;
    vector<PietonTrotuar*> pietoniiTrotuar;
    float timpSpawnTrotuar=0.f;
    float timpSpawn[4]={0.f,0.f,0.f,0.f};
    float intervalSpawn=2.5f;

    auto spawnVehicul=[&](int bi){
        if((int)vehiculeBanda[bi].size()>=7) return;
        int ci=rand()%7,ni=rand()%6,tr=rand()%100;
        VehicolInfo vi; vi.culoare=culoriVizuale[ci];
        float vmax=130.f;
        if(tr<60){
            benzi[bi]->adaugaVehicul(new ::Masina(culoriNume[ci],numareMasini[ni]));
            vi.tip=TIP_MASINA; vmax=130.f;
        }else if(tr<85){
            benzi[bi]->adaugaVehicul(new Camion(culoriNume[ci],numareMasini[ni]));
            vi.tip=TIP_CAMION; vmax=85.f;
            vi.culoare=sf::Color(min(255,(int)culoriVizuale[ci].r+30),min(255,(int)culoriVizuale[ci].g+20),80);
        }else{
            benzi[bi]->adaugaVehicul(new Motocicleta(culoriNume[ci],numareMasini[ni]));
            vi.tip=TIP_MOTO; vmax=180.f;
            vi.culoare=sf::Color(200,100,20);
        }
        infoBenzi[bi].push_back(vi);
        vehiculeBanda[bi].push_back(new Vehicul2D(getIntrare(bi),vi.culoare,vi.tip,vmax,bi));
    };

    auto distMinima=[](TipViz t)->float{
        if(t==TIP_CAMION)return 55.f;
        if(t==TIP_MOTO)  return 32.f;
        return 42.f;
    };

    struct Buton{sf::RectangleShape forma;string label;bool hover=false;};
    auto makeButon=[&](string txt,float x,float y)->Buton{
        Buton b;b.forma.setSize({130.f,38.f});b.forma.setPosition({x,y});
        b.forma.setFillColor(sf::Color(50,55,80));
        b.forma.setOutlineColor(sf::Color(100,110,180));b.forma.setOutlineThickness(1.5f);
        b.label=txt;return b;
    };
    auto drawButon=[&](Buton& b){
        b.forma.setFillColor(b.hover?sf::Color(80,90,130):sf::Color(50,55,80));
        window.draw(b.forma);
        sf::Text t(font,b.label,15);t.setFillColor(sf::Color::White);
        auto p=b.forma.getPosition();t.setPosition({p.x+12.f,p.y+10.f});window.draw(t);
    };
    auto inButon=[](Buton& b,sf::Vector2i mp)->bool{
        auto p=b.forma.getPosition();auto s=b.forma.getSize();
        return mp.x>=p.x&&mp.x<=p.x+s.x&&mp.y>=p.y&&mp.y<=p.y+s.y;
    };

    Buton btnStart=makeButon("START",  760.f,30.f);
    Buton btnReset=makeButon("RESET",  760.f,76.f);
    Buton btnPlus =makeButon("Viteza+",760.f,122.f);
    Buton btnMinus=makeButon("Viteza-",760.f,168.f);

    while(window.isOpen()){
        sf::Vector2i mp=sf::Mouse::getPosition(window);
        btnStart.hover=inButon(btnStart,mp);btnReset.hover=inButon(btnReset,mp);
        btnPlus.hover=inButon(btnPlus,mp);btnMinus.hover=inButon(btnMinus,mp);

        while(auto event=window.pollEvent()){
            if(event->is<sf::Event::Closed>())window.close();
            if(auto* me=event->getIf<sf::Event::MouseButtonPressed>()){
                sf::Vector2i mpe={me->position.x,me->position.y};
                if(inButon(btnStart,mpe)){
                    running=!running;
                    btnStart.label=running?"PAUZA":"START";
                    simClock.restart();
                }
                if(inButon(btnReset,mpe)){
                    running=false;btnStart.label="START";
                    ciclu=0;totalTrecute=0;totalMasini=0;totalCamioane=0;totalMoto=0;totalPietoni=0;timpTotal=0;
                    faza=0;contorFaza=0;
                    for(int i=0;i<4;i++){
                        for(auto v:vehiculeBanda[i])delete v;
                        vehiculeBanda[i].clear();
                        while(!benzi[i]->esteGoala()){delete benzi[i]->coada.front();benzi[i]->coada.pop();}
                        infoBenzi[i].clear();
                        timpSpawn[i]=0.f;
                    }
                    for(auto v:vehiculeTraverseaza)delete v;vehiculeTraverseaza.clear();
                    for(auto p:pietoni)delete p;pietoni.clear();
                    for(auto p:pietoniiTrotuar)delete p;pietoniiTrotuar.clear();
                    timpSpawnTrotuar=0.f;
                    semafoare[0]->setCuloare(Semafor::VERDE);semafoare[1]->setCuloare(Semafor::VERDE);
                    semafoare[2]->setCuloare(Semafor::ROSU); semafoare[3]->setCuloare(Semafor::ROSU);
                }
                if(inButon(btnPlus,mpe))  intervalSpawn=max(1.0f,intervalSpawn-0.5f);
                if(inButon(btnMinus,mpe)) intervalSpawn=min(6.0f,intervalSpawn+0.5f);
            }
        }

        float dt=frameClock.restart().asSeconds();
        if(dt>0.05f)dt=0.05f;
        if(running)timpTotal+=dt;

        if(running){
            // Spawn vehicule
            for(int i=0;i<4;i++){
                timpSpawn[i]+=dt;
                if(timpSpawn[i]>=intervalSpawn){
                    timpSpawn[i]=0.f;
                    if(rand()%3!=0)spawnVehicul(i);
                }
            }

            // Update faze semafoare
            if(simClock.getElapsedTime().asSeconds()>=interval){
                simClock.restart();ciclu++;
                contorFaza++;
                if(contorFaza>=durateFaze[faza]){contorFaza=0;faza=(faza+1)%4;}

                if(faza==0){
                    semafoare[0]->setCuloare(Semafor::VERDE);semafoare[1]->setCuloare(Semafor::VERDE);
                    semafoare[2]->setCuloare(Semafor::ROSU); semafoare[3]->setCuloare(Semafor::ROSU);
                    // NS verde, EV rosu =>
                    // Pietoni traverseaza trecerile NORD si SUD (traverseaza drumul orizontal EV)
                    // Adica merg de pe trotuar stanga pe trotuar dreapta (sau invers)
                    // pe trecerile de deasupra si de dedesubtul intersectiei
                    if((int)pietoni.size()<8&&rand()%2==0){
                        // Trecerea Nord: y = cy - roadW/2 - 10 (deasupra intersectiei)
                        // Pieton merge orizontal: de la cx-roadW/2-35 la cx+roadW/2+35
                        float yTrecere = cy - roadW/2 - 10.f;
                        float xStart = (rand()%2==0) ? cx-roadW/2-38.f : cx+roadW/2+38.f;
                        float xEnd   = (xStart < cx) ? cx+roadW/2+38.f : cx-roadW/2-38.f;
                        pietoni.push_back(new Pieton(
                            {xStart, yTrecere}, {xEnd, yTrecere},
                            culoriPiele[rand()%4], culoriHaine[rand()%6], 0));
                    }
                    if((int)pietoni.size()<8&&rand()%2==0){
                        // Trecerea Sud: y = cy + roadW/2 + 10 (dedesubtul intersectiei)
                        float yTrecere = cy + roadW/2 + 10.f;
                        float xStart = (rand()%2==0) ? cx-roadW/2-38.f : cx+roadW/2+38.f;
                        float xEnd   = (xStart < cx) ? cx+roadW/2+38.f : cx-roadW/2-38.f;
                        pietoni.push_back(new Pieton(
                            {xStart, yTrecere}, {xEnd, yTrecere},
                            culoriPiele[rand()%4], culoriHaine[rand()%6], 0));
                    }
                }else if(faza==1){
                    for(int i=0;i<4;i++)semafoare[i]->setCuloare(Semafor::GALBEN);
                }else if(faza==2){
                    semafoare[0]->setCuloare(Semafor::ROSU); semafoare[1]->setCuloare(Semafor::ROSU);
                    semafoare[2]->setCuloare(Semafor::VERDE);semafoare[3]->setCuloare(Semafor::VERDE);
                    // EV verde, NS rosu =>
                    // Pietoni traverseaza trecerile EST si VEST (traverseaza drumul vertical NS)
                    // Adica merg de pe trotuar sus pe trotuar jos (sau invers)
                    // pe trecerile din stanga si din dreapta intersectiei
                    if((int)pietoni.size()<8&&rand()%2==0){
                        // Trecerea Vest: x = cx - roadW/2 - 10 (stanga intersectiei)
                        float xTrecere = cx - roadW/2 - 10.f;
                        float yStart = (rand()%2==0) ? cy-roadW/2-38.f : cy+roadW/2+38.f;
                        float yEnd   = (yStart < cy) ? cy+roadW/2+38.f : cy-roadW/2-38.f;
                        pietoni.push_back(new Pieton(
                            {xTrecere, yStart}, {xTrecere, yEnd},
                            culoriPiele[rand()%4], culoriHaine[rand()%6], 1));
                    }
                    if((int)pietoni.size()<8&&rand()%2==0){
                        // Trecerea Est: x = cx + roadW/2 + 10 (dreapta intersectiei)
                        float xTrecere = cx + roadW/2 + 10.f;
                        float yStart = (rand()%2==0) ? cy-roadW/2-38.f : cy+roadW/2+38.f;
                        float yEnd   = (yStart < cy) ? cy+roadW/2+38.f : cy-roadW/2-38.f;
                        pietoni.push_back(new Pieton(
                            {xTrecere, yStart}, {xTrecere, yEnd},
                            culoriPiele[rand()%4], culoriHaine[rand()%6], 1));
                    }
                }else{
                    for(int i=0;i<4;i++)semafoare[i]->setCuloare(Semafor::GALBEN);
                }
                for(int i=0;i<4;i++)semafoare[i]->setMasiniAsteptare(benzi[i]->getNrVehicule());
            }

            // Misca vehicule in coada
            for(int bi=0;bi<4;bi++){
                auto& banda=vehiculeBanda[bi];
                sf::Vector2f stopPos=getStopPos(bi);
                sf::Vector2f iesPos =getIesire(bi);
                sf::Vector2f dir    =getDirBanda(bi);
                bool verde=semafoare[bi]->esteVerde();

                for(int idx=0;idx<(int)banda.size();idx++){
                    Vehicul2D* v=banda[idx];
                    sf::Vector2f tinta;

                    if(idx==0){
                        // Primul: merge spre linia de stop
                        tinta=stopPos;
                        // Daca a ajuns la linia de stop SI e verde => traverseaza
                        sf::Vector2f d2=stopPos-v->pozitie;
                        float dist2=sqrt(d2.x*d2.x+d2.y*d2.y);
                        if(verde&&dist2<12.f){
                            v->traverseaza=true;
                            v->tinta=iesPos;
                            vehiculeTraverseaza.push_back(v);
                            banda.pop_front();
                            if(!benzi[bi]->esteGoala()){delete benzi[bi]->coada.front();benzi[bi]->coada.pop();}
                            if(!infoBenzi[bi].empty())infoBenzi[bi].erase(infoBenzi[bi].begin());
                            totalTrecute++;totalMasini++;
                            idx--;
                            continue;
                        }
                    }else{
                        // Restul: mentin distanta fata de cel din fata
                        Vehicul2D* fata=banda[idx-1];
                        float dist=distMinima(v->tip);
                        tinta=fata->pozitie - dir*dist;
                    }

                    // Misca spre tinta
                    sf::Vector2f d=tinta-v->pozitie;
                    float dlen=sqrt(d.x*d.x+d.y*d.y);
                    float dot=d.x*dir.x+d.y*dir.y;
                    if(dlen>4.f&&dot>0.f){
                        float acc=v->vitezaMax*4.f; // acceleratie mai mare
                        v->viteza=min(v->viteza+acc*dt,v->vitezaMax);
                        if(dlen<50.f)v->viteza=max(8.f,v->viteza-acc*1.5f*dt);
                        v->pozitie+=(d/dlen)*v->viteza*dt;
                    }else{
                        v->viteza=max(0.f,v->viteza-v->vitezaMax*5.f*dt); // franare rapida
                    }
                }
            }

            // Misca vehicule care traverseaza intersectia
            for(auto it=vehiculeTraverseaza.begin();it!=vehiculeTraverseaza.end();){
                Vehicul2D* v=*it;
                sf::Vector2f d=v->tinta-v->pozitie;
                float dlen=sqrt(d.x*d.x+d.y*d.y);
                if(dlen<6.f){delete v;it=vehiculeTraverseaza.erase(it);}
                else{
                    v->viteza=min(v->viteza+v->vitezaMax*2.f*dt,v->vitezaMax);
                    v->pozitie+=(d/dlen)*v->viteza*dt;
                    ++it;
                }
            }

            // Spawn pietoni pe trotuar
            timpSpawnTrotuar+=dt;
            if(timpSpawnTrotuar>3.f && (int)pietoniiTrotuar.size()<12){
                timpSpawnTrotuar=0.f;
                // Trotoarul orizontal sus: y intre cy-roadW/2-10 si cy-roadW/2
                // Trotoarul orizontal jos: y intre cy+roadW/2 si cy+roadW/2+10
                // Trotoarul vertical stanga: x intre cx-roadW/2-10 si cx-roadW/2
                // Trotoarul vertical dreapta: x intre cx+roadW/2 si cx+roadW/2+10
                int tip=rand()%4;
                sf::Color piele=culoriPiele[rand()%4];
                sf::Color haine=culoriHaine[rand()%6];
                float xS,yS,xE,yE;
                if(tip==0){ // trotuar sus
                    yS=cy-roadW/2-7.f; yE=yS;
                    xS=(rand()%2==0)?10.f:cx-roadW/2-15.f;
                    xE=(xS<cx)?cx-roadW/2-15.f:10.f;
                }else if(tip==1){ // trotuar jos
                    yS=cy+roadW/2+7.f; yE=yS;
                    xS=(rand()%2==0)?10.f:cx-roadW/2-15.f;
                    xE=(xS<cx)?cx-roadW/2-15.f:10.f;
                }else if(tip==2){ // trotuar stanga
                    xS=cx-roadW/2-7.f; xE=xS;
                    yS=(rand()%2==0)?10.f:cy-roadW/2-15.f;
                    yE=(yS<cy)?cy-roadW/2-15.f:10.f;
                }else{ // trotuar dreapta
                    xS=cx+roadW/2+7.f; xE=xS;
                    yS=(rand()%2==0)?10.f:cy-roadW/2-15.f;
                    yE=(yS<cy)?cy-roadW/2-15.f:10.f;
                }
                pietoniiTrotuar.push_back(new PietonTrotuar({xS,yS},{xE,yE},piele,haine));
            }

            // Misca pietoni pe trotuar
            for(auto it=pietoniiTrotuar.begin();it!=pietoniiTrotuar.end();){
                PietonTrotuar* p=*it;
                sf::Vector2f d=p->tinta-p->pozitie;
                float dlen=sqrt(d.x*d.x+d.y*d.y);
                if(dlen<5.f){delete p;it=pietoniiTrotuar.erase(it);}
                else{p->legTimer+=dt;p->pozitie+=(d/dlen)*p->viteza*dt;++it;}
            }

            // directie=0 (orizontal, treceri Nord/Sud): pornesc cand NS e ROSU
            // directie=1 (vertical, treceri Est/Vest): pornesc cand EV e ROSU
            // Odata porniti NU se mai opresc
            bool nsEsteRosu=(semafoare[0]->getCuloare()==Semafor::ROSU);
            bool evEsteRosu=(semafoare[2]->getCuloare()==Semafor::ROSU);

            for(auto it=pietoni.begin();it!=pietoni.end();){
                Pieton* p=*it;

                // Daca a inceput sa mearga, continua pana la capat
                if(p->viteza>0.f){
                    sf::Vector2f d=p->tinta-p->pozitie;
                    float dlen=sqrt(d.x*d.x+d.y*d.y);
                    if(dlen<5.f){totalPietoni++;delete p;it=pietoni.erase(it);}
                    else{p->legTimer+=dt;p->pozitie+=(d/dlen)*p->viteza*dt;++it;}
                    continue;
                }

                // Daca nu a inceput, verifica semaforul
                bool poatePorni=false;
                if(p->directie==0 && nsEsteRosu) poatePorni=true;
                if(p->directie==1 && evEsteRosu) poatePorni=true;

                if(poatePorni){
                    p->viteza=52.f; // porneste
                }
                ++it;
            }
        }

        // ===== DESENARE =====
        window.clear(sf::Color(35,80,35));

        // Iarba
        for(int gx=0;gx<920;gx+=36)
            for(int gy=0;gy<720;gy+=36){
                sf::RectangleShape g({35.f,35.f});g.setPosition({(float)gx,(float)gy});
                g.setFillColor((gx+gy)%72<36?sf::Color(38,85,38):sf::Color(42,92,42));
                window.draw(g);
            }


        // ---- ORASEL MIC - top down ----
        auto casa=[&](float x,float y,float w,float h,sf::Color acop,sf::Color contur){
            sf::RectangleShape umbra({w+3.f,h+3.f});umbra.setPosition({x+2.f,y+2.f});umbra.setFillColor(sf::Color(0,0,0,50));window.draw(umbra);
            sf::RectangleShape r({w,h});r.setPosition({x,y});r.setFillColor(acop);r.setOutlineColor(contur);r.setOutlineThickness(2.f);window.draw(r);
            sf::RectangleShape cr({w-6.f,2.f});cr.setPosition({x+3.f,y+h/2.f-1.f});cr.setFillColor(sf::Color(max(0,(int)acop.r-40),max(0,(int)acop.g-40),max(0,(int)acop.b-40)));window.draw(cr);
            sf::RectangleShape cp({2.f,h-4.f});cp.setPosition({x+w/2.f-1.f,y+2.f});cp.setFillColor(sf::Color(max(0,(int)acop.r-30),max(0,(int)acop.g-30),max(0,(int)acop.b-30)));window.draw(cp);
        };
        auto copac=[&](float x,float y){
            sf::CircleShape coroana(8.f);coroana.setOrigin({8.f,8.f});coroana.setPosition({x,y});coroana.setFillColor(sf::Color(30,120,30));coroana.setOutlineColor(sf::Color(20,80,20));coroana.setOutlineThickness(1.f);window.draw(coroana);
            sf::CircleShape centru(4.f);centru.setOrigin({4.f,4.f});centru.setPosition({x,y});centru.setFillColor(sf::Color(20,100,20));window.draw(centru);
        };
        auto parc=[&](float x,float y,float w,float h){
            sf::RectangleShape p({w,h});p.setPosition({x,y});p.setFillColor(sf::Color(45,105,45));p.setOutlineColor(sf::Color(30,80,30));p.setOutlineThickness(1.5f);window.draw(p);
        };
        auto banca=[&](float x,float y,bool orizontal){
            sf::RectangleShape b(orizontal?sf::Vector2f{14.f,5.f}:sf::Vector2f{5.f,14.f});
            b.setPosition({x,y});b.setFillColor(sf::Color(120,80,40));b.setOutlineColor(sf::Color(80,50,20));b.setOutlineThickness(1.f);window.draw(b);
        };
        auto cladire=[&](float x,float y,float w,float h,sf::Color c){
            sf::RectangleShape umbra({w+4.f,h+4.f});umbra.setPosition({x+3.f,y+3.f});umbra.setFillColor(sf::Color(0,0,0,80));window.draw(umbra);
            sf::RectangleShape r({w,h});r.setPosition({x,y});r.setFillColor(c);r.setOutlineColor(sf::Color(max(0,(int)c.r-50),max(0,(int)c.g-50),max(0,(int)c.b-50)));r.setOutlineThickness(2.5f);window.draw(r);
            // Ferestre mici
            for(float fx=x+6.f;fx<x+w-10.f;fx+=12.f)
                for(float fy=y+6.f;fy<y+h-10.f;fy+=12.f){
                    sf::RectangleShape f({7.f,7.f});f.setPosition({fx,fy});f.setFillColor(sf::Color(180,220,255,180));f.setOutlineColor(sf::Color(100,100,100));f.setOutlineThickness(0.5f);window.draw(f);
                }
        };

        // ===== COLT STANGA-SUS =====
        parc(8.f,8.f,190.f,130.f);
        // Case
        casa(12.f, 12.f, 55.f,38.f, sf::Color(180,80,60),  sf::Color(120,50,30));
        casa(80.f, 12.f, 50.f,35.f, sf::Color(80,130,80),  sf::Color(50,90,50));
        casa(148.f,12.f, 45.f,35.f, sf::Color(160,120,70), sf::Color(110,80,40));
        casa(12.f, 68.f, 50.f,35.f, sf::Color(100,100,160),sf::Color(60,60,110));
        casa(75.f, 65.f, 55.f,38.f, sf::Color(170,90,60),  sf::Color(115,58,35));
        casa(145.f,65.f, 48.f,35.f, sf::Color(60,140,120), sf::Color(35,95,80));
        // Copaci
        copac(22.f,115.f);copac(50.f,118.f);copac(80.f,112.f);
        copac(110.f,118.f);copac(140.f,115.f);copac(170.f,112.f);
        // Banci
        banca(35.f,108.f,true);banca(95.f,108.f,true);banca(155.f,108.f,true);

        // ===== COLT DREAPTA-SUS =====
        float rx=cx+roadW/2+12.f;
        parc(rx,8.f,200.f,130.f);
        casa(rx+5.f,  12.f, 55.f,38.f, sf::Color(190,75,55),  sf::Color(130,45,28));
        casa(rx+72.f, 12.f, 50.f,35.f, sf::Color(70,125,85),  sf::Color(40,85,55));
        casa(rx+135.f,12.f, 48.f,35.f, sf::Color(155,115,65), sf::Color(105,75,35));
        casa(rx+5.f,  65.f, 52.f,36.f, sf::Color(105,95,155), sf::Color(65,58,115));
        casa(rx+72.f, 65.f, 55.f,38.f, sf::Color(175,85,65),  sf::Color(118,52,38));
        casa(rx+138.f,65.f, 46.f,35.f, sf::Color(65,135,95),  sf::Color(38,90,60));
        copac(rx+15.f,115.f);copac(rx+50.f,118.f);copac(rx+85.f,112.f);
        copac(rx+120.f,118.f);copac(rx+155.f,115.f);copac(rx+185.f,112.f);
        banca(rx+30.f,108.f,true);banca(rx+98.f,108.f,true);banca(rx+162.f,108.f,true);

        // ===== COLT STANGA-JOS =====
        float by=cy+roadW/2+12.f;
        parc(8.f,by,190.f,140.f);
        casa(12.f, by+8.f,  55.f,38.f, sf::Color(185,75,55),  sf::Color(125,45,30));
        casa(80.f, by+8.f,  50.f,35.f, sf::Color(70,130,85),  sf::Color(42,88,55));
        casa(148.f,by+8.f,  45.f,35.f, sf::Color(150,110,60), sf::Color(100,72,32));
        casa(12.f, by+62.f, 50.f,35.f, sf::Color(100,90,150), sf::Color(62,55,110));
        casa(75.f, by+60.f, 55.f,38.f, sf::Color(168,82,62),  sf::Color(112,50,35));
        casa(145.f,by+60.f, 48.f,35.f, sf::Color(60,130,90),  sf::Color(35,85,58));
        copac(22.f,by+108.f);copac(52.f,by+112.f);copac(82.f,by+108.f);
        copac(112.f,by+112.f);copac(142.f,by+108.f);copac(172.f,by+108.f);
        banca(38.f,by+102.f,true);banca(100.f,by+102.f,true);banca(158.f,by+102.f,true);

        // ===== COLT DREAPTA-JOS =====
        parc(rx,by,200.f,140.f);
        casa(rx+5.f,  by+8.f,  55.f,38.f, sf::Color(178,80,58),  sf::Color(118,48,28));
        casa(rx+72.f, by+8.f,  50.f,35.f, sf::Color(65,132,90),  sf::Color(38,88,58));
        casa(rx+135.f,by+8.f,  48.f,35.f, sf::Color(148,108,58), sf::Color(98,70,30));
        casa(rx+5.f,  by+62.f, 52.f,36.f, sf::Color(98,88,148),  sf::Color(60,52,108));
        casa(rx+72.f, by+62.f, 55.f,38.f, sf::Color(172,82,62),  sf::Color(115,50,35));
        casa(rx+138.f,by+62.f, 46.f,35.f, sf::Color(62,130,92),  sf::Color(36,86,58));
        copac(rx+15.f,by+108.f);copac(rx+50.f,by+112.f);copac(rx+85.f,by+108.f);
        copac(rx+120.f,by+112.f);copac(rx+155.f,by+108.f);copac(rx+188.f,by+108.f);
        banca(rx+32.f,by+102.f,true);banca(rx+100.f,by+102.f,true);banca(rx+165.f,by+102.f,true);

        // ===== CLADIRI SPECIALE pe marginea trotuarului =====
        // Primarie (sus, centru)
        cladire(cx-40.f, 5.f, 80.f,55.f, sf::Color(220,210,185));
        sf::Text txtPrim(font,"PRIMARIE",9);txtPrim.setFillColor(sf::Color(60,40,20));txtPrim.setPosition({cx-34.f,32.f});window.draw(txtPrim);

        // Magazine (jos, centru)
        cladire(cx-55.f, by+5.f, 50.f,48.f, sf::Color(185,160,210));
        sf::Text txtMag1(font,"MARKET",9);txtMag1.setFillColor(sf::Color(60,30,80));txtMag1.setPosition({cx-50.f,by+25.f});window.draw(txtMag1);
        cladire(cx+5.f, by+5.f, 50.f,48.f, sf::Color(210,185,155));
        sf::Text txtMag2(font,"CAFE",9);txtMag2.setFillColor(sf::Color(80,40,20));txtMag2.setPosition({cx+18.f,by+25.f});window.draw(txtMag2);

        // Scoala (stanga, centru)
        cladire(5.f, cy-35.f, 55.f,70.f, sf::Color(210,195,155));
        sf::Text txtSc(font,"SCOALA",9);txtSc.setFillColor(sf::Color(60,50,20));txtSc.setPosition({10.f,cy-12.f});window.draw(txtSc);

        // Spital (dreapta, centru)
        cladire(rx+5.f, cy-35.f, 55.f,70.f, sf::Color(210,210,210));
        sf::Text txtSp(font,"SPITAL",9);txtSp.setFillColor(sf::Color(180,30,30));txtSp.setPosition({rx+10.f,cy-12.f});window.draw(txtSp);
        // Cruce rosie spital
        sf::RectangleShape cr1({20.f,6.f}),cr2({6.f,20.f});
        cr1.setPosition({rx+17.f,cy-30.f});cr2.setPosition({rx+23.f,cy-36.f});
        cr1.setFillColor(sf::Color(200,30,30));cr2.setFillColor(sf::Color(200,30,30));
        window.draw(cr1);window.draw(cr2);


        sf::RectangleShape trotH({730.f,roadW+20.f});
        trotH.setPosition({0.f,cy-roadW/2-10.f});trotH.setFillColor(sf::Color(150,142,122));window.draw(trotH);
        sf::RectangleShape trotV({roadW+20.f,720.f});
        trotV.setPosition({cx-roadW/2-10.f,0.f});trotV.setFillColor(sf::Color(150,142,122));window.draw(trotV);

        // Drum
        sf::RectangleShape drumH({730.f,roadW});
        drumH.setPosition({0.f,cy-roadW/2});drumH.setFillColor(sf::Color(62,62,62));window.draw(drumH);
        sf::RectangleShape drumV({roadW,720.f});
        drumV.setPosition({cx-roadW/2,0.f});drumV.setFillColor(sf::Color(62,62,62));window.draw(drumV);
        sf::RectangleShape centru({roadW,roadW});
        centru.setPosition({cx-roadW/2,cy-roadW/2});centru.setFillColor(sf::Color(68,68,68));window.draw(centru);

        // Marcaje
        for(float x=0;x<730.f;x+=40.f){
            if(x>cx-roadW/2-5&&x<cx+roadW/2+5)continue;
            sf::RectangleShape l({22.f,3.f});l.setPosition({x,cy-1.5f});
            l.setFillColor(sf::Color(255,255,255,150));window.draw(l);
        }
        for(float y=0;y<720.f;y+=40.f){
            if(y>cy-roadW/2-5&&y<cy+roadW/2+5)continue;
            sf::RectangleShape l({3.f,22.f});l.setPosition({cx-1.5f,y});
            l.setFillColor(sf::Color(255,255,255,150));window.draw(l);
        }

        // ---- Treceri de pietoni ----
        // Faza 0,1 = NS verde => trecerile Nord+Sud active (pietoni orizontali trec)
        // Faza 2,3 = EV verde => trecerile Est+Vest active (pietoni verticali trec)
        bool trecereNordSudActiva=(faza==0||faza==1);
        bool trecereEstVestActiva=(faza==2||faza==3);
        sf::Color culNS=trecereNordSudActiva?sf::Color(255,255,255,230):sf::Color(160,55,55,160);
        sf::Color culEV=trecereEstVestActiva?sf::Color(255,255,255,230):sf::Color(160,55,55,160);

        float dungW=7.f, dungGap=5.f, dungL=22.f;
        // Trecere Nord (sus) - pietoni merg orizontal
        for(float xd=cx-roadW/2+3.f;xd<cx+roadW/2-8.f;xd+=dungW+dungGap){
            sf::RectangleShape tr({dungW,dungL});tr.setPosition({xd,cy-roadW/2-dungL});
            tr.setFillColor(culNS);window.draw(tr);
        }
        // Trecere Sud (jos) - pietoni merg orizontal
        for(float xd=cx-roadW/2+3.f;xd<cx+roadW/2-8.f;xd+=dungW+dungGap){
            sf::RectangleShape tr({dungW,dungL});tr.setPosition({xd,cy+roadW/2});
            tr.setFillColor(culNS);window.draw(tr);
        }
        // Trecere Vest (stanga) - pietoni merg vertical
        for(float yd=cy-roadW/2+3.f;yd<cy+roadW/2-8.f;yd+=dungW+dungGap){
            sf::RectangleShape tr({dungL,dungW});tr.setPosition({cx-roadW/2-dungL,yd});
            tr.setFillColor(culEV);window.draw(tr);
        }
        // Trecere Est (dreapta) - pietoni merg vertical
        for(float yd=cy-roadW/2+3.f;yd<cy+roadW/2-8.f;yd+=dungW+dungGap){
            sf::RectangleShape tr({dungL,dungW});tr.setPosition({cx+roadW/2,yd});
            tr.setFillColor(culEV);window.draw(tr);
        }

        // Semafoare pietoni
        auto drawSemPieton=[&](float x,float y,bool verde){
            sf::RectangleShape corp({11.f,22.f});corp.setPosition({x,y});
            corp.setFillColor(sf::Color(18,18,18));corp.setOutlineColor(sf::Color(55,55,55));corp.setOutlineThickness(1.f);
            window.draw(corp);
            sf::CircleShape lr(4.f),lg(4.f);
            lr.setPosition({x+1.5f,y+1.5f});lg.setPosition({x+1.5f,y+13.f});
            lr.setFillColor(!verde?sf::Color(240,50,50):sf::Color(65,12,12));
            lg.setFillColor(verde?sf::Color(50,230,80):sf::Color(12,55,18));
            window.draw(lr);window.draw(lg);
        };
        // Semafoare pietoni Nord+Sud: verde cand trecerea Nord-Sud e activa
        drawSemPieton(cx-roadW/2-24,cy-roadW/2-26,trecereNordSudActiva);
        drawSemPieton(cx+roadW/2+13,cy-roadW/2-26,trecereNordSudActiva);
        drawSemPieton(cx-roadW/2-24,cy+roadW/2+4, trecereNordSudActiva);
        drawSemPieton(cx+roadW/2+13,cy+roadW/2+4, trecereNordSudActiva);
        // Semafoare pietoni Est+Vest: verde cand trecerea Est-Vest e activa
        drawSemPieton(cx-roadW/2-26,cy-roadW/2-24,trecereEstVestActiva);
        drawSemPieton(cx-roadW/2-26,cy+roadW/2+13,trecereEstVestActiva);
        drawSemPieton(cx+roadW/2+4, cy-roadW/2-24,trecereEstVestActiva);
        drawSemPieton(cx+roadW/2+4, cy+roadW/2+13,trecereEstVestActiva);

        // Semafoare masini
        float semPos[4][2]={
            {cx+roadW/2+10,cy-roadW/2-72},
            {cx-roadW/2-33,cy+roadW/2+10},
            {cx+roadW/2+10,cy+roadW/2+10},
            {cx-roadW/2-33,cy-roadW/2-72}
        };
        for(int i=0;i<4;i++){
            float sx=semPos[i][0],sy=semPos[i][1];
            sf::RectangleShape stalp({4.f,20.f});stalp.setPosition({sx+9.f,sy+62.f});
            stalp.setFillColor(sf::Color(70,70,70));window.draw(stalp);
            sf::RectangleShape corp({22.f,62.f});corp.setPosition({sx,sy});
            corp.setFillColor(sf::Color(20,20,20));corp.setOutlineColor(sf::Color(65,65,65));corp.setOutlineThickness(1.5f);
            window.draw(corp);
            Semafor::Culoare c=semafoare[i]->getCuloare();
            for(int j=0;j<3;j++){sf::CircleShape ring(8.5f);ring.setOrigin({8.5f,8.5f});ring.setPosition({sx+11.f,sy+13.f+(float)j*18.f});ring.setFillColor(sf::Color(8,8,8));window.draw(ring);}
            sf::CircleShape l1(7.f),l2(7.f),l3(7.f);
            l1.setOrigin({7.f,7.f});l2.setOrigin({7.f,7.f});l3.setOrigin({7.f,7.f});
            l1.setPosition({sx+11.f,sy+13.f});l2.setPosition({sx+11.f,sy+31.f});l3.setPosition({sx+11.f,sy+49.f});
            l1.setFillColor(c==Semafor::ROSU  ?sf::Color(255,55,55):sf::Color(65,12,12));
            l2.setFillColor(c==Semafor::GALBEN?sf::Color(255,215,45):sf::Color(65,58,12));
            l3.setFillColor(c==Semafor::VERDE ?sf::Color(55,235,85):sf::Color(12,62,22));
            if(c==Semafor::ROSU){sf::CircleShape g(11.f);g.setOrigin({11.f,11.f});g.setPosition({sx+11.f,sy+13.f});g.setFillColor(sf::Color(255,55,55,50));window.draw(g);}
            if(c==Semafor::GALBEN){sf::CircleShape g(11.f);g.setOrigin({11.f,11.f});g.setPosition({sx+11.f,sy+31.f});g.setFillColor(sf::Color(255,215,45,50));window.draw(g);}
            if(c==Semafor::VERDE){sf::CircleShape g(11.f);g.setOrigin({11.f,11.f});g.setPosition({sx+11.f,sy+49.f});g.setFillColor(sf::Color(55,235,85,50));window.draw(g);}
            window.draw(l1);window.draw(l2);window.draw(l3);
        }

        // Linie stop (vizuala)
        for(int bi=0;bi<4;bi++){
            sf::RectangleShape ls;
            switch(bi){
                case 0:ls.setSize({roadW,3.f});ls.setPosition({cx-roadW/2,cy-stopDist-1.f});break;
                case 1:ls.setSize({roadW,3.f});ls.setPosition({cx-roadW/2,cy+stopDist-1.f});break;
                case 2:ls.setSize({3.f,roadW});ls.setPosition({cx+stopDist-1.f,cy-roadW/2});break;
                case 3:ls.setSize({3.f,roadW});ls.setPosition({cx-stopDist-1.f,cy-roadW/2});break;
            }
            ls.setFillColor(sf::Color(255,255,255,200));window.draw(ls);
        }

        // Vehicule in coada
        for(int bi=0;bi<4;bi++)
            for(auto v:vehiculeBanda[bi])
                deseneazaVehicul(window,v->pozitie,v->culoare,v->tip,bi);

        // Vehicule care traverseaza
        for(auto v:vehiculeTraverseaza)
            deseneazaVehicul(window,v->pozitie,v->culoare,v->tip,v->banda);

        // Pietoni pe trecere
        for(auto p:pietoni)deseneazaPieton(window,*p);

        // Pietoni pe trotuar
        for(auto p:pietoniiTrotuar){
            float lx=sin(p->legTimer*8.f)*3.f;
            sf::RectangleShape pic1({2.f,7.f}),pic2({2.f,7.f});
            pic1.setOrigin({1.f,0.f});pic2.setOrigin({1.f,0.f});
            pic1.setPosition({p->pozitie.x-2.f+lx,p->pozitie.y+4.f});
            pic2.setPosition({p->pozitie.x+1.f-lx,p->pozitie.y+4.f});
            pic1.setFillColor(sf::Color(50,35,15));pic2.setFillColor(sf::Color(50,35,15));
            window.draw(pic1);window.draw(pic2);
            sf::RectangleShape corp({6.f,8.f});corp.setOrigin({3.f,0.f});
            corp.setPosition({p->pozitie.x,p->pozitie.y-1.f});corp.setFillColor(p->haine);window.draw(corp);
            sf::CircleShape cap(4.5f);cap.setOrigin({4.5f,4.5f});cap.setPosition(p->pozitie);
            cap.setFillColor(p->culoare);cap.setOutlineColor(sf::Color(0,0,0,80));cap.setOutlineThickness(0.5f);
            window.draw(cap);
        }

        // Etichete
        string dirLabel[]={"NORD","SUD","EST","VEST"};
        sf::Vector2f dirPos[]={
            {cx+roadW/2+14,cy-roadW/2-105},
            {cx-roadW/2-54,cy+roadW/2+82},
            {cx+roadW/2+14,cy+roadW/2+82},
            {cx-roadW/2-54,cy-roadW/2-105}
        };
        for(int i=0;i<4;i++){
            sf::Text lbl(font,dirLabel[i],13);lbl.setFillColor(sf::Color(215,215,215));lbl.setPosition(dirPos[i]);window.draw(lbl);
            sf::Text cnt(font,intToStr((int)vehiculeBanda[i].size())+" veh.",12);cnt.setFillColor(sf::Color(170,225,170));cnt.setPosition({dirPos[i].x,dirPos[i].y+17.f});window.draw(cnt);
            sf::Color tc=(semafoare[i]->getCuloare()==Semafor::ROSU)?sf::Color(235,75,75):(semafoare[i]->getCuloare()==Semafor::GALBEN)?sf::Color(235,205,75):sf::Color(75,215,75);
            sf::Text st(font,semafoare[i]->getCuloareString(),11);st.setFillColor(tc);st.setPosition({dirPos[i].x,dirPos[i].y+32.f});window.draw(st);
        }

        // Panou dreapta
        sf::RectangleShape panou({160.f,720.f});panou.setPosition({760.f,0.f});panou.setFillColor(sf::Color(15,15,28));window.draw(panou);
        sf::RectangleShape sep2({2.f,720.f});sep2.setPosition({760.f,0.f});sep2.setFillColor(sf::Color(55,55,95));window.draw(sep2);

        sf::Text titlu(font,"SIMULATOR",16);titlu.setFillColor(sf::Color(140,170,255));titlu.setPosition({770.f,8.f});window.draw(titlu);
        sf::Text titlu2(font,"TRAFIC RUTIER",12);titlu2.setFillColor(sf::Color(90,110,190));titlu2.setPosition({770.f,27.f});window.draw(titlu2);

        sf::RectangleShape lsep({140.f,1.f});lsep.setFillColor(sf::Color(55,55,95));lsep.setPosition({770.f,46.f});window.draw(lsep);

        drawButon(btnStart);drawButon(btnReset);drawButon(btnPlus);drawButon(btnMinus);

        sf::Text spTxt(font,"Spawn:"+floatToStr(intervalSpawn)+"s",12);
        spTxt.setFillColor(sf::Color(150,150,195));spTxt.setPosition({770.f,218.f});window.draw(spTxt);

        lsep.setPosition({770.f,233.f});window.draw(lsep);

        string fazaStr[]={"NS:VERDE / EV:ROSU","Tranzitie","EV:VERDE / NS:ROSU","Tranzitie"};
        sf::Color fazaCol[]={sf::Color(75,215,75),sf::Color(225,195,55),sf::Color(75,215,75),sf::Color(225,195,55)};
        sf::Text fazaTxt(font,fazaStr[faza],11);fazaTxt.setFillColor(fazaCol[faza]);fazaTxt.setPosition({770.f,240.f});window.draw(fazaTxt);
        sf::Text timpTxt(font,"Timp:"+floatToStr(timpTotal)+"s",12);timpTxt.setFillColor(sf::Color(150,150,195));timpTxt.setPosition({770.f,258.f});window.draw(timpTxt);

        lsep.setPosition({770.f,275.f});window.draw(lsep);
        sf::Text leg(font,"Vehicule:",13);leg.setFillColor(sf::Color(170,170,215));leg.setPosition({770.f,280.f});window.draw(leg);

        auto drawLeg=[&](float y,sf::Color c,float w,float h,string txt){
            sf::RectangleShape r({w,h});r.setPosition({770.f,y});r.setFillColor(c);r.setOutlineColor(sf::Color(0,0,0,100));r.setOutlineThickness(1.f);window.draw(r);
            sf::Text t(font,txt,11);t.setFillColor(sf::Color(195,195,195));t.setPosition({770.f+w+5,y-1});window.draw(t);
        };
        drawLeg(298.f,sf::Color(40,90,200),15.f,10.f,"Masina");
        drawLeg(315.f,sf::Color(175,135,75),19.f,12.f,"Camion");
        drawLeg(332.f,sf::Color(195,95,18), 11.f,8.f, "Moto");
        sf::CircleShape pc(4.5f);pc.setPosition({770.f,349.f});pc.setFillColor(sf::Color(255,205,150));window.draw(pc);
        sf::Text ptxt(font,"Pieton",11);ptxt.setFillColor(sf::Color(195,195,195));ptxt.setPosition({781.f,347.f});window.draw(ptxt);

        lsep.setPosition({770.f,366.f});window.draw(lsep);
        sf::Text stit(font,"Statistici:",13);stit.setFillColor(sf::Color(170,170,215));stit.setPosition({770.f,372.f});window.draw(stit);

        vector<pair<string,string>> stats={
            {"Ciclu:",   intToStr(ciclu)},
            {"Total:",   intToStr(totalTrecute)},
            {"Masini:",  intToStr(totalMasini)},
            {"Camioane:",intToStr(totalCamioane)},
            {"Moto:",    intToStr(totalMoto)},
            {"Pietoni:", intToStr(totalPietoni)},
            {"Nord:",    intToStr((int)vehiculeBanda[0].size())},
            {"Sud:",     intToStr((int)vehiculeBanda[1].size())},
            {"Est:",     intToStr((int)vehiculeBanda[2].size())},
            {"Vest:",    intToStr((int)vehiculeBanda[3].size())},
        };
        float yS=392.f;
        for(auto& s:stats){
            sf::Text k(font,s.first,12);k.setFillColor(sf::Color(130,130,185));k.setPosition({770.f,yS});window.draw(k);
            sf::Text v(font,s.second,12);v.setFillColor(sf::Color(215,215,215));v.setPosition({848.f,yS});window.draw(v);
            yS+=21.f;
        }

        lsep.setPosition({770.f,610.f});window.draw(lsep);
        sf::Text status(font,running?">>> RULARE":"  PAUZA",15);
        status.setFillColor(running?sf::Color(75,225,75):sf::Color(225,95,95));
        status.setPosition({770.f,618.f});window.draw(status);
        sf::Text jos(font,"Piata Unirii",12);jos.setFillColor(sf::Color(90,90,140));jos.setPosition({770.f,700.f});window.draw(jos);

        window.display();
    }

    for(int i=0;i<4;i++){for(auto v:vehiculeBanda[i])delete v;}
    for(auto v:vehiculeTraverseaza)delete v;
    for(auto s:semafoare)delete s;
    for(auto b:benzi)delete b;
    for(auto p:pietoni)delete p;
    for(auto p:pietoniiTrotuar)delete p;
    return 0;
}

#include<cstdio>
#include<cstdlib>
#include<vector>
#include<algorithm>
#include<ctime>
#include<set>
using namespace std;

//===========================================
// Definicje klas
//===========================================

class Punkt{
public:
  int x;
  int y;
  Punkt(): x(0), y(0) {}
  Punkt(int x,int y): x(x), y(y) {}
  
  bool operator<(const Punkt& p) const {
    return x != p.x ? x < p.x : y < p.y;
  }

};

class Robot {
public:
  int nr;
  int t;
  char* kierunek;
  int* kroki;
  int gora;
  int dol;
  int lewo;
  int prawo;
  set<Punkt> punkty;
  
  Robot(int nr,int t,char* kierunek, int* kroki) : nr(nr), t(t), kierunek(kierunek),kroki(kroki) {
    wyliczRozmiar();
  }
  
private:
  void wyliczRozmiar(){
    gora=dol=lewo=prawo=0;
    int aktX = 0;
    int aktY = 0;
    punkty.insert(*(new Punkt(aktX,aktY)));
    
    for(int i=0;i<t;++i){
      //printf("%d: %c %d | aktX:%d aktY:%d \n",nr,kierunek[i],kroki[i],aktX,aktY);
	if(kierunek[i]=='N'){
	  gora = min(gora,aktY-kroki[i]);
	  for(int j=0;j<kroki[i];++j)
	    punkty.insert(*(new Punkt(aktX,--aktY)));
	}
	else if(kierunek[i]=='S'){
	  dol = max(dol,aktY+kroki[i]);
	  for(int j=0;j<kroki[i];++j)
	    punkty.insert(*(new Punkt(aktX,++aktY)));
	}
	else if(kierunek[i]=='E'){
	  prawo = max(prawo,aktX+kroki[i]);
	  for(int j=0;j<kroki[i];++j)
	    punkty.insert(*(new Punkt(++aktX,aktY)));
	}
	else if(kierunek[i]=='W'){
	  lewo = min(lewo,aktX-kroki[i]);
	  for(int j=0;j<kroki[i];++j)
	    punkty.insert(*(new Punkt(--aktX,aktY)));
	}
    }
    gora=-gora;
    lewo=-lewo;
  }
  
};

//===========================================
// Globalne zmienne
//===========================================

int w,h,s,k; //s - minimalne zapelnienie, k - ile robotow
vector<Robot*> roboty;
vector<pair<Robot*, Punkt> > wynik;
int aktZapelnienie=0;
int** plansza;

//===========================================
// Wczytanie danych
//===========================================

void wczytaj_dane(){
  scanf("%d %d\n",&w,&h);
  scanf("%d\n",&s);
  scanf("%d\n",&k);
  for(int i=0;i<k;++i){
    int nr,t;
    scanf("%d\n",&nr);
    scanf("%d\n",&t);
    char* kierunek = new char[t];
    int* kroki = new int[t];
    for(int j=0;j<t;++j)
      scanf("%c %d\n",&kierunek[j], &kroki[j]);
    roboty.push_back(new Robot(nr,t,kierunek,kroki));
  }
}

//===========================================
// Wypisywanie
//===========================================

void wypisz_roboty(){
  printf("--------------\n");
  printf("WSZYSTKIE ROBOTY\n");
  printf("--------------\n");
  for(vector<Robot*>::iterator it = roboty.begin(); it!=roboty.end();++it){
    printf("%d g:%d d:%d l:%d p:%d\n",(*it)->nr,(*it)->gora,(*it)->dol,(*it)->lewo,(*it)->prawo);
    for(set<Punkt>::iterator it2 = (*it)->punkty.begin(); it2!=(*it)->punkty.end();++it2)
      printf("   (%d, %d)\n",it2->x,it2->y);
  }
  printf("--------------\n");
}

void stworz_plansze_jako_macierz(){
  plansza = new int*[h+1];
  for(int i=1;i<=h;++i)
    plansza[i]=new int[w+1];
  for(int i=1;i<=h;i++)
    for(int j=1;j<=w;j++)
      plansza[i][j]=0;
  
}

void wypisz_plansze(){
  stworz_plansze_jako_macierz();
  
  for(vector<pair<Robot*,Punkt> >::iterator it = wynik.begin();it!=wynik.end();++it){
    int x=it->second.x;
    int y=it->second.y;
    for(set<Punkt>::iterator it2 = it->first->punkty.begin(); it2!=it->first->punkty.end();++it2){
      printf("wynik: %d %d\n",it2->x+x,it2->y+y);
      fflush(stdout);
      plansza[it2->x+x][it2->y+y]=it->first->nr;
    }
  }
  printf("--------------\n");
  printf("PLANSZA\n");
  printf("--------------\n");
  for(int y=1;y<=h;y++){
    for(int x=1;x<=w;x++)
      printf("%d ",plansza[x][y]);
    printf("\n");
  }
  printf("--------------\n");
  
}

void wypisz_wynik(){
  printf("--------------\n");
  printf("WYNIK\n");
  printf("--------------\n");

  for(vector<pair<Robot*,Punkt> >::iterator it = wynik.begin();it!=wynik.end();++it){
    int x=it->second.x;
    int y=it->second.y;
    printf("%d: %d %d \n",it->first->nr,x,y);
    for(set<Punkt>::iterator it2 = it->first->punkty.begin(); it2!=it->first->punkty.end();++it2){
      printf("  (%d, %d)",it2->x+x,it2->y+y);
      fflush(stdout);
    }
    printf("\n");
  }
  printf("--------------\n");

}

void mieszaj_roboty(){
  random_shuffle (roboty.begin(), roboty.end());
}

//===========================================
// Rozwiazanie
//===========================================

bool roboty_koliduja(Robot* robot1, Punkt punkt1, Robot* robot2, Punkt punkt2){
  printf("Czy koliduja %d(%d,%d) z %d(%d,%d)\n", robot1->nr, punkt1.x, punkt1.y, robot2->nr, punkt2.x, punkt2.y);
  //przyjmujemy, x_min mniejsze dla 1szego robota
  if(punkt2.x-robot2->lewo < punkt1.x-robot1->lewo){
    swap(punkt1,punkt2);
    swap(robot1,robot2);
  }
  
  //zmienne pomocnicze
  int Y1_min=punkt1.y-robot1->gora;
  int Y1_max=punkt1.y+robot1->dol;
  int X1_min=punkt1.x-robot1->lewo;
  int X1_max=punkt1.x+robot1->prawo;
  int Y2_min=punkt2.y-robot2->gora;
  int Y2_max=punkt2.y+robot2->dol;
  int X2_min=punkt2.x-robot2->lewo;
  int X2_max=punkt2.x+robot2->prawo;
  printf("zakres %d: ([%d - %d],[%d - %d])\n", robot1->nr, X1_min, X1_max, Y1_min, Y1_max);
  printf("zakres %d: ([%d - %d],[%d - %d])\n", robot2->nr, X2_min, X2_max, Y2_min, Y2_max);
  
  // jeśli nie mogą mieć wspólnych punktów
  if( (X2_min > X1_max) || (Y1_min > Y2_max) || (Y2_min > Y1_max) ){
    printf("   nie maja wspolnych punktow: NIE\n");
    return false;
  }
  
  //jeśli roboty układają sie w krzyz
  if((X1_max >= X2_max && Y2_min <= Y1_min && Y2_max >= Y1_max) ||
     (Y1_min >= Y2_min && Y1_max <= Y2_max && X1_min <= X2_min && X1_max >= X2_max) ||
     (Y2_min >= Y1_min && Y2_max <= Y1_max && X2_min <= X1_min && X2_max >= X1_max)){
    printf("   na pewno sie gdzies przecinaja: TAK\n");
    return true;
  }
  
  //minimalizacja liczby porownan (porownujemy mniejszy set z wiekszym)
  if(robot1->punkty.size() > robot2->punkty.size()){
    swap(punkt1,punkt2);
    swap(robot1,robot2);
  }
  for(set<Punkt>::iterator it = robot1->punkty.begin(); it!=robot1->punkty.end();++it) {
    Punkt przeskalowany(it->x + punkt1.x - punkt2.x, it->y + punkt1.y - punkt2.y);
    if(robot2->punkty.find(przeskalowany) != robot2->punkty.end()){
      printf("   koliduja w punkcie (%d,%d): TAK\n",it->x + punkt1.x,it->y + punkt1.y);
      return true;
    }
  }
  
  printf("   nie kolidują z żadnym punkcjie: NIE\n");
  return false;
}

bool da_sie_dorzucic(Robot* robot, Punkt punkt){
  for(vector<pair<Robot*, Punkt> >::iterator it = wynik.begin(); it!=wynik.end();++it){
    Robot* innyRobot = it->first;
    Punkt innyPunkt = it->second;
    if(roboty_koliduja(robot,punkt,innyRobot,innyPunkt))
      return false;
  }
  return true;
  
}

void rozstaw_robota(int &xMin, int &xMax, int &yMin, int &yMax,Robot* robot){
    for(int y=yMin;y<=yMax;++y)
      for(int x=xMin;x<=xMax;++x){
	Punkt pkt(x,y);
	if(da_sie_dorzucic(robot,pkt)){
	  wynik.push_back(pair<Robot*,Punkt>(robot,pkt));
	  aktZapelnienie+=robot->punkty.size();
	  return;
	}
      }
}

void rozstaw_roboty(){
  int yMin,yMax,xMin,xMax;
  for(vector<Robot*>::iterator it = roboty.begin(); it!=roboty.end();++it){
    Robot* robot = (*it);
    yMin=1+robot->gora;
    yMax=h-robot->dol;
    xMin=1+robot->lewo;
    xMax=w-robot->prawo;
    
    rozstaw_robota(xMin, xMax, yMin, yMax,robot);
    if(aktZapelnienie>=s)
      return;
    
  } 
}


//===========================================
// Testy
//===========================================
void assert(bool ans){
  if(ans)
    printf("[ok]\n");
  else
    printf("[error]\n");
  fflush(stdout);
}

void test(){
  Punkt p[10][10];
  
  for(int x=1;x<10;x++)
    for(int y=0;y<10;y++)
      p[x][y]=*(new Punkt(x,y));
  
  //1 z 2
  assert(roboty_koliduja(roboty[0], p[1][1], roboty[1], p[1][1]));
  assert(!roboty_koliduja(roboty[0], p[1][1], roboty[1], p[2][1]));
  assert(!roboty_koliduja(roboty[0], p[1][1], roboty[1], p[3][1]));
  assert(!roboty_koliduja(roboty[0], p[1][1], roboty[1], p[5][1]));
  assert(roboty_koliduja(roboty[0], p[1][1], roboty[1], p[2][2]));
  assert(roboty_koliduja(roboty[0], p[1][2], roboty[1], p[2][1]));
  
  //1 z 5
  assert(roboty_koliduja(roboty[0], p[1][1], roboty[4], p[2][1]));
  assert(roboty_koliduja(roboty[0], p[1][1], roboty[4], p[2][2]));
  assert(roboty_koliduja(roboty[0], p[1][1], roboty[4], p[2][3]));
  
  //2 z 3
  assert(!roboty_koliduja(roboty[1], p[2][1], roboty[2], p[3][2]));
}

//===========================================
// Main
//===========================================

int main(){
  srand(unsigned(time(0)));
  wczytaj_dane();
  //wypisz_roboty();
  //test();
  
  
  while(aktZapelnienie<s){
    mieszaj_roboty();
    wynik.clear();
    aktZapelnienie=0;
    rozstaw_roboty();
  }

  wypisz_wynik();
  
  //wypisz_roboty();
  wypisz_plansze();
}




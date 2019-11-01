/* Otrzymuje polecenia od glownego programu. Korzysta w calosci z modulu trie. */
/* Z jednej strony jest jak gdyby jego czescia odpowiadajaca bezposrednio za   */
/* przetwarzenie zapytan modulu glownego. Robi to rozdzielajac poszczegolne    */
/* podzadania miedzy odpowiednie funkcje modulu trie i modulu DSU, z ktorego   */
/* trie korzysta. Z drugiej, stanowi bariere abstrakcji miedzy modulem glownym */
/* a wlasciwa struktura drzewa trie.                                           */
#include <stdlib.h>
#include <stdbool.h> 
#include <inttypes.h>
#include "dsu.h"
#include "trie.h"
#include "queryHandler.h"


void initialise()
{
  initialiseTrie();
}

void deinitialise()
{
  deinitialiseTrie();
}

void declareHistory(char* history, int historySize)
{
  trieAdd(history, historySize);
}

void removeHistory(char* history, int historySize)
{
  trieRemoveSubtree(history, historySize);
}

bool isHistoryValid(char* history, int historySize)
{
  //W kazdym wierzcholku trie trzymamy wskaznik na reprezentanta
  //w DSU tego wierzcholka. Kazda dopuszczalna historia ma w DSU
  //dokladnie jednego reprezentanta. Jesli historia nie ma przy-
  //pisanej energii, w reprezentancie trzymamy energie rowna 0.
  DsuNodePtr dsuRepresentative = getDsuRepresentative(history, historySize);
  return dsuRepresentative != NULL;
}

//Zwraca false jesli napotkano bledy, true w przeciwnym przypadku.
bool equaliseHistories(char* history1, int historySize1, char* history2, int historySize2)
{
  DsuNodePtr dsuRepresentative1 = getDsuRepresentative(history1, historySize1); 
  DsuNodePtr dsuRepresentative2 = getDsuRepresentative(history2, historySize2);

  //Ktorakolwiek z historii jest niedopuszczalna.
  if (dsuRepresentative1 == NULL || dsuRepresentative2 == NULL) 
    return false;

  //Zrownanie dopuszczalnej historii z soba sama - nie zmieniamy niczego.
  if (dsuRepresentative1 == dsuRepresentative2)
    return true;

  uint64_t energy1 = dsuRepresentative1->component->energy;
  uint64_t energy2 = dsuRepresentative2->component->energy;
  //Zadna z historii nie ma przypisanej energii.
  if (energy1 == 0 && energy2 == 0) 
    return false;

  if (energy1 != 0 && energy2 != 0)
  {
    uint64_t newEnergy = energy1/2 + energy2/2;
    if ((energy1&1) && (energy2&1)) ++newEnergy;
    //Mnozenie tak zeby uniknac wyjscia poza typ.
    //Jesli obie liczby byly nieparzyste to musimy jeszcze dodac 1.
    Union(dsuRepresentative1, dsuRepresentative2, newEnergy);
    return true;
  }

  //W przeciwnym wypadku jedna z historii nie ma przypisanej energii, a druga ma.
  if (energy1 == 0)
    Union(dsuRepresentative1, dsuRepresentative2, energy2);
  else Union(dsuRepresentative1, dsuRepresentative2, energy1);
  return true;
}

//Zwraca 0 jesli napotkano bledy, w przeciwnym wypadku odpowiednia wartosc energii.
uint64_t getEnergy(char* history, int historySize)
{
  DsuNodePtr dsuRepresentative = getDsuRepresentative(history, historySize); 
  if (dsuRepresentative == NULL) //Niedopuszczalna historia
    return 0;

  //Jesli historia jest dopuszczalna, ale nie ma przypisanej energii,
  //to zwrocone zostanie 0, ktore trzymamy jako informacje o braku
  //przypisanej energii. 
  return dsuRepresentative->component->energy;
}

//Zwraca false jesli napotkano bledy, true w przeciwnym przypadku.
bool setEnergy(char* history, int historySize, uint64_t newEnergy)
{
  DsuNodePtr dsuRepresentative = getDsuRepresentative(history, historySize); 
  if (dsuRepresentative == NULL) //Niedopuszczalna historia
    return false;

  dsuRepresentative->component->energy = newEnergy;
  return true;
}

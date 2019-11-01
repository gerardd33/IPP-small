/* Strukture zbiorow rozlacznych utrzymujemy w postaci zbioru repre-  */
/* zentantow klas abstrakcji (DsuComponent), utrzymujacych informacje */
/* o tym, jakie wierzcholki (DsuNode) naleza do klasy (implicite w    */
/* postaci listy - w praktyce utrzymuja tylko wskaznik na poczatek i  */
/* koniec tej listy, a kazdy element listy utrzymuje wskaznik na pop- */
/* rzedni i nastepny element listy. W ten sposob umozliwiamy usuwanie */
/* wierzcholkow ze struktury DSU.                                     */
#include <stdlib.h>
#include <stdbool.h> 
#include <inttypes.h>
#include "dsu.h"


DsuNodePtr makeDsuNode()
{
  //Tworzymy takze skladowa zawierajaca na razie tylko nasz nowo stworzony wierzcholek.
  DsuNodePtr newNode = (DsuNodePtr)malloc(sizeof(struct DsuNode));
  if (newNode == NULL) exit(1);
  DsuNodePtr newListBeginning = (DsuNodePtr)malloc(sizeof(struct DsuNode));
  if (newListBeginning == NULL) exit(1);
  DsuNodePtr newListEnd = (DsuNodePtr)malloc(sizeof(struct DsuNode));
  if (newListEnd == NULL) exit(1);

  DsuComponentPtr newComponent = (DsuComponentPtr)malloc(sizeof(struct DsuComponent));
  if (newComponent == NULL) exit(1);
  newComponent->energy = 0;
  newComponent->size = 1;
  
  newNode->component = newComponent;
  newListBeginning->component = newComponent;
  newListEnd->component = newComponent;

  newNode->previousOnList = newListBeginning;
  newNode->nextOnList = newListEnd;
  newListBeginning->previousOnList = NULL;
  newListBeginning->nextOnList = newNode;
  newListEnd->previousOnList = newNode;
  newListEnd->nextOnList = NULL;

  newComponent->listBeginning = newListBeginning;
  newComponent->listEnd = newListEnd;
  return newNode;
}

static void eraseComponent(DsuComponentPtr component)
{
  free(component->listBeginning);
  free(component->listEnd);
  free(component);
}

void eraseNode(DsuNodePtr node)
{
  --node->component->size;
  if (node->component->size != 0)
  {
    node->nextOnList->previousOnList = node->previousOnList;
    node->previousOnList->nextOnList = node->nextOnList;
  }
  else eraseComponent(node->component);
  free(node);
}

void swapPointers(DsuComponentPtr* component1, DsuComponentPtr* component2)
{
  DsuComponentPtr tmp = *component1;
  *component1 = *component2;
  *component2 = tmp;
}

void Union(DsuNodePtr node1, DsuNodePtr node2, uint64_t newEnergy)
{
  DsuComponentPtr component1 = node1->component;
  DsuComponentPtr component2 = node2->component;
  component1->energy = newEnergy;
  component2->energy = newEnergy;

  if (component1 == component2) //Juz sa w jednej klasie abstrakcji.
    return;

  if (component1->size < component2->size)
    swapPointers(&component1, &component2); //Teraz component2 jest mniejsza.
  component1->size += component2->size;

  //Laczymy obie listy, przepinajac tak, ze pomijamy atrapowy koniec 
  //listy component1 i atrapowy poczatek listy component2.
  DsuNodePtr end1 = component1->listEnd;
  DsuNodePtr end2 = component2->listEnd;
  DsuNodePtr beginning2 = component2->listBeginning;

  //Zmieniamy kazdemu elementowi z listy component2 skladowa na component1.
  DsuNodePtr current = beginning2->nextOnList;
  while (current != component2->listEnd)
  {
    current->component = component1;
    current = current->nextOnList;
  }

  end1->previousOnList->nextOnList = beginning2->nextOnList;
  beginning2->nextOnList->previousOnList = end1->previousOnList;

  end2->previousOnList->nextOnList = end1;
  end1->previousOnList = end2->previousOnList;
  eraseComponent(component2);
}

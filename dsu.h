#ifndef DSU_H
#define DSU_H

struct DsuNode;
struct DsuComponent;
typedef struct DsuNode* DsuNodePtr;
typedef struct DsuComponent* DsuComponentPtr;

struct DsuNode
{
  DsuComponentPtr component; //Wskaznik na reprezentanta skladowej,
  //do ktorej nalezy ten wierzcholek
  DsuNodePtr previousOnList;
  DsuNodePtr nextOnList;
};

struct DsuComponent
{
  uint64_t energy;
  int size;
  DsuNodePtr listBeginning;
  DsuNodePtr listEnd;
};

DsuNodePtr makeDsuNode();
void eraseNode(DsuNodePtr node);
extern void Union(DsuNodePtr node1, DsuNodePtr node2, uint64_t newEnergy);

#endif /* DSU_H */
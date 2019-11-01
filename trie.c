/* Stanowi implementacje wlasciwej struktury drzewa trie. W kazdym */
/* wezle trzymamy wskaznik na reprezentanta jakiejs historii w     */
/* strukturze DSU.                                                 */
#include <stdlib.h>
#include <stdbool.h> 
#include <inttypes.h>
#include "dsu.h"
#include "trie.h"


struct TrieNode
{
  DsuNodePtr dsuRepresentativeHere;
  TrieNodePtr edge[4];
};

static TrieNodePtr makeTrieNode()
{
  TrieNodePtr newNode = (TrieNodePtr)malloc(sizeof(struct TrieNode));
  if (newNode == NULL) exit(1);
  newNode->dsuRepresentativeHere = makeDsuNode();
  for (int i=0; i<4; ++i) 
    newNode->edge[i] = NULL;
  return newNode;
}

static void eraseSubtree(TrieNodePtr currentSubtree)
{
  for (int i=0; i<4; ++i)
  {
    if (currentSubtree->edge[i] != NULL)
      eraseSubtree(currentSubtree->edge[i]);
  }

  eraseNode(currentSubtree->dsuRepresentativeHere);
  free(currentSubtree);
}

//Korzen drzewa trie, odpowiadajacy pustemu stringowi.
static TrieNodePtr trieOrigin;
void initialiseTrie()
{
  trieOrigin = makeTrieNode();
}

void deinitialiseTrie()
{
  eraseSubtree(trieOrigin);
}

void trieAdd(char* string, int stringSize)
{
  TrieNodePtr current = trieOrigin;
  for (int i=0; i<stringSize; ++i)
  {
    if (current->edge[string[i]-'0'] == NULL)
      current->edge[string[i]-'0'] = makeTrieNode();
    current = current->edge[string[i]-'0'];
  }
}

void trieRemoveSubtree(char* string, int stringSize)
{
  TrieNodePtr current = trieOrigin;
  for (int i=0; i<stringSize-1; ++i)
  {
    if (current->edge[string[i]-'0'] == NULL)
      return;
    current = current->edge[string[i]-'0'];
  }

  //Doszlismy do wezla, z ktorego wychodzi krawedz do wezla 
  //odpowiadajacego pelnemu stringowi. Musimy usunac te krawedz
  //i cale poddrzewo pod nia.
  TrieNodePtr currentSubtree = current->edge[string[stringSize-1]-'0'];
  if (currentSubtree == NULL) return;
  current->edge[string[stringSize-1]-'0'] = NULL;
  eraseSubtree(currentSubtree);
}

DsuNodePtr getDsuRepresentative(char* string, int stringSize)
{
  TrieNodePtr current = trieOrigin;
  for (int i=0; i<stringSize; ++i)
  {
    if (current->edge[string[i]-'0'] == NULL)
      return NULL;
    current = current->edge[string[i]-'0'];
  }

  return current->dsuRepresentativeHere;
}

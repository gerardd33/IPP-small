#ifndef TRIE_H
#define TRIE_H

struct TrieNode;
typedef struct TrieNode* TrieNodePtr;
void initialiseTrie();
extern void deinitialiseTrie();
extern void trieAdd(char* history, int historySize);
extern void trieRemoveSubtree(char* history, int historySize);
extern DsuNodePtr getDsuRepresentative(char* history, int historySize);

#endif /* TRIE_H */
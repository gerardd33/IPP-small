#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

extern void initialise();
extern void deinitialise();
extern void declareHistory(char* history, int historySize);
extern void removeHistory(char* history, int historySize);
extern bool isHistoryValid(char* history, int historySize);
extern bool equaliseHistories(char* history1, int historySize1, char* history2, int historySize2);
extern uint64_t getEnergy(char* history, int historySize);
extern bool setEnergy(char* history, int historySize, uint64_t newEnergy);

#endif /* QUERYHANDLER_H */
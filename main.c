/* Wywoluje funkcje wczytujaca linie wejscia z modulu inputParser. */
/* Otrzymuje sparsowana linie wejscia w postaci struktury Command, */
/* zawierajacej wszystkie potrzebne informacje o wlasnie wczytanej */
/* linii. Interpretuje polecenie z tej linii, przekazujac wykona-  */
/* nie odpowiednich zadan modulowi queryHandler. Na koniec zwraca  */
/* wynik tych operacji.                                            */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "queryHandler.h"
#include "inputParser.h"


void interpretCommand(CommandPtr command)
{
  enum commandType currentType = command->type;
  //Drugi warunek - EOF w srodku linii, polecenie niezakonczone '\n' - 
  //- nie wypisujemy ERROR.
  if (currentType == Ignore || currentType == IgnoreEOF)
    return;

  if (currentType == Error || currentType == EndOfFile)
  {
    fprintf(stderr, "ERROR\n");
    return;
  }

  normaliseString(command->history1, &(command->historySize1));
  if (currentType == Declare)
  {
    declareHistory(command->history1, command->historySize1);
    printf("OK\n");
    free(command->history1);
  }
  else if (currentType == Remove)
  {
    removeHistory(command->history1, command->historySize1);
    printf("OK\n");
    free(command->history1);
  }
  else if (currentType == Valid)
  {
    if (isHistoryValid(command->history1, command->historySize1))
      printf("YES\n");
    else printf("NO\n");    
    free(command->history1);
  }
  else if (currentType == Equal)
  {
    normaliseString(command->history2, &(command->historySize2));

    if (equaliseHistories(command->history1, command->historySize1, 
      command->history2, command->historySize2))
      printf("OK\n");
    else fprintf(stderr, "ERROR\n");
    free(command->history1);
    free(command->history2);
  }     
  else if (currentType == GetEnergy)
  {
    uint64_t result = getEnergy(command->history1, command->historySize1);
    if (result != 0)
      printf("%" PRIu64 "\n", result);
    else fprintf(stderr, "ERROR\n");
    free(command->history1);
  }
  else if (currentType == SetEnergy)
  {
    if (setEnergy(command->history1, command->historySize1, command->energy))
      printf("OK\n");
    else fprintf(stderr, "ERROR\n");
    free(command->history1);
  }
}

int main()
{
  initialise();
  while (true)
  {
    int lastChar = getchar();
    if (lastChar != EOF)
    {
      ungetc(lastChar, stdin);
      CommandPtr nextCommand = readLine();
      interpretCommand(nextCommand);

      if (nextCommand->type == EndOfFile || 
        nextCommand->type == IgnoreEOF)
      {
        free(nextCommand);
        deinitialise();
        return 0;
      }

      free(nextCommand);
    }
    else break;
  }
  deinitialise();
  return 0;
}

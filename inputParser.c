/* Wczytuje linie wejscia, przetwarza ja odpowiednio i przekazuje */
/* modulowi glownemu wszystkie potrzebne informacje o wczytanej   */
/* linii w postaci struktury Command.                             */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include "inputParser.h"


//Stringi z ktorymi bedziemy porownywac wejscie, zeby roz-
//poznawac typ polecenia. Zakonczone kropka dla ulatwienia
//okreslania konca stringa. 
const char* stringDeclare = "DECLARE.";
const char* stringRemove = "REMOVE.";
const char* stringValid = "VALID.";
const char* stringEqual = "EQUAL.";
const char* stringEnergy = "ENERGY.";

CommandPtr makeCommand()
{
  CommandPtr command = (CommandPtr)malloc(sizeof(struct Command));
  if (command == NULL) exit(1);
  command->type = Ignore;
  command->history1 = NULL;
  command->historySize1 = 0;
  command->history2 = NULL;
  command->historySize2 = 0;
  command->energy = 0;
  return command;
}

void readLineToTheEnd(int lastChar, CommandPtr command)
{
  ungetc(lastChar, stdin);
  lastChar = getchar();
  while (lastChar != '\n')
  {
    if (lastChar == EOF)
    {
      if (command->type == Ignore)
        command->type = IgnoreEOF;
      else command->type = EndOfFile;
      return;
    }

    lastChar = getchar();
  }
}

void readNumber(CommandPtr command)
{
  int lastChar = getchar();
  //Zera wiodace.
  if (lastChar == '0' || !isdigit(lastChar))
  {
    command->type = Error;
    free(command->history1);
    readLineToTheEnd(lastChar, command);
    return;
  }

  uint64_t result = 0, previousResult = 0;
  bool energyWithinRange = true;

  while (isdigit(lastChar))
  {
    previousResult = result;
    result = result*10 + (lastChar-'0');
    //uint64_t sie "przekrecil" - liczba spoza zakresu.
    if (result < previousResult)
    {
      energyWithinRange = false;
      break;
    }

    lastChar = getchar();
  }

  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(command->history1);
    return;
  }

  //Cos innego niz '\n' bezposrednio po liczbie.
  if (lastChar != '\n' || !energyWithinRange)
  {
    command->type = Error;
    free(command->history1);
    if (lastChar != '\n')
      readLineToTheEnd(lastChar, command);
    return;
  }

  command->energy = result;
  command->type = SetEnergy;
}

//Ustawia stringSize na rzeczywista dlugosc poprawnego stringa
//(na razie okresla dlugosc zaalokowanego obszaru pamieci).
void normaliseString(char* string, int* stringSize)
{
  int index = 0;
  while (index < (*stringSize) && isdigit(string[index]))
    ++index;
  (*stringSize) = index;
}

//Wstawia znak x na index-owa pozycje w stringu.
void insertToString(char** string, int* stringSize, int* index, int x) 
{
  (*string)[*index] = x;
  ++(*index);

  //Tablica dynamiczna.
  if (*index == *stringSize)
  {
    (*stringSize) *= 2;
    (*string) = realloc(*string, (*stringSize)*sizeof(char));
  }
}

//Wczytuje poprawnego stringa (zlozonego ze znakow '0', '1', '2', '3').
//Zwraca pierwszy niepoprawny znak wczytany po jego wczytaniu.
int readString(char** string, int* stringSize)
{
  (*string)[0] = '#'; //Znak niebedacy liczba na poczatku,
  //zeby wygodnie rozpoznawac puste stringi.
  (*stringSize) = 1;
  int index = 0;

  int lastChar = getchar();
  while (isdigit(lastChar) && (lastChar-'0')<=3)
  {
    insertToString(string, stringSize, &index, lastChar);
    lastChar = getchar();
  }
  //Inicjalizujemy reszte zaalokowanej pamieci na stringa czymkolwiek.
  for (int i=index; i<(*stringSize); ++i) 
    (*string)[i] = '#';

  return lastChar;
}

//Wczytuje stringa do command->history1.
//Zwraca false jesli napotkano bledy, true w przeciwnym przypadku.
bool readExactlyOneString(CommandPtr command)
{
  command->history1 = malloc(sizeof(char));
  if (command->history1 == NULL) exit(1);
  //Pierwszy niepoprawny znak wczytany po wczytaniu stringa.
  int lastChar = readString(&(command->history1), &(command->historySize1));
  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(command->history1);
    return false;
  }

  //Pusty string lub cos innego niz '\n' bezposrednio po stringu.
  if (!isdigit(command->history1[0]) || lastChar != '\n')
  {
    command->type = Error;
    free(command->history1);
    readLineToTheEnd(lastChar, command);
    return false;
  }

  return true;
}

bool compareString(char* string, const char* stringModel)
{
  int index = 0;
  while (stringModel[index] != '.')
  {
    if (string[index] != stringModel[index])
      return false;
    ++index;
  }

  //Typ polecenia jest dluzszy niz wzorzec (np. REMOVEX).
  if (isalpha(string[index]))
    return false;
  return true;
}

void readTypeBloc(int* lastChar, char** typeBloc, int* typeBlocSize)
{
  //Bedziemy do tej tablicy wczytywac pierwszy blok wejscia 
  //odpowiadajacy typowi polecenia. Pierwszy znak mamy juz wczytany.
  //Typ polecenia moze miec do 7 znakow (najdluzszy - DECLARE).
  (*typeBloc) = malloc(8*sizeof(char));
  if ((*typeBloc) == NULL) exit(1);
  (*typeBlocSize) = 0;

  while (isalpha(*lastChar) && (*typeBlocSize)<7) 
  {
    (*typeBloc)[(*typeBlocSize)++] = (*lastChar);
    (*lastChar) = getchar();
  }
  //Inicjalizujemy reszte zaalokowanej pamieci na stringa czymkolwiek.
  for (int i=(*typeBlocSize); i<8; ++i) 
    (*typeBloc)[i] = '#';
}

void readTwoStrings(CommandPtr command)
{
  command->history1 = malloc(sizeof(char));
  if (command->history1 == NULL) exit(1);
  //Pierwszy niepoprawny znak wczytany po wczytaniu stringa.
  int lastChar = readString((&command->history1), &(command->historySize1));
  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(command->history1);
    return;
  }
  
  //Pusty pierwszy argument lub brak spacji oddzielajacej dwa argumenty.
  if (!isdigit(command->history1[0]) || lastChar != ' ')
  {
    command->type = Error;
    free(command->history1);
    readLineToTheEnd(lastChar, command);
    return;
  }

  command->history2 = malloc(sizeof(char));
  if (command->history2 == NULL) exit(1);
  lastChar = readString((&command->history2), &(command->historySize2));
  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(command->history1);
    free(command->history2);
    return;
  }

  //Pusty drugi argument lub cos innego niz '\n' bezposrednio po nim.
  if (!isdigit(command->history2[0]) || lastChar != '\n')
  {
    command->type = Error;
    free(command->history1);
    free(command->history2);
    readLineToTheEnd(lastChar, command);
    return;
  }

  command->type = Equal;
}

void readStringAndNumber(CommandPtr command)
{
  command->history1 = malloc(sizeof(char));
  if (command->history1 == NULL) exit(1);
  //Pierwszy niepoprawny znak wczytany po wczytaniu stringa.
  int lastChar = readString(&(command->history1), &(command->historySize1));
  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(command->history1);
    return;
  }

  //Pusty pierwszy argument lub cos innego niz ' ' lub '\n' bezposrednio po nim.
  if (!isdigit(command->history1[0]) || (lastChar != ' ' && lastChar != '\n'))
  {
    command->type = Error;
    free(command->history1);
    readLineToTheEnd(lastChar, command);
    return;
  }

  //Jednoargumentowe ENERGY.
  if (lastChar == '\n')
    command->type = GetEnergy;
  else readNumber(command); //Dwuargumentowe ENERGY.
}

//Sprawdza typ polecenia i wczytujemy odpowiednia 
//liczbe kolejnych blokow (argumentow).
void readArguments(char* typeBloc, CommandPtr command)
{
  if (compareString(typeBloc, stringDeclare))
  {
    free(typeBloc);
    if (readExactlyOneString(command))
      command->type = Declare;
  }
  else if (compareString(typeBloc, stringRemove))
  {
    free(typeBloc);
    if (readExactlyOneString(command)) 
      command->type = Remove;
  }
  else if (compareString(typeBloc, stringValid))
  {
    free(typeBloc);
    if (readExactlyOneString(command)) 
      command->type = Valid;
  }
  else if (compareString(typeBloc, stringEqual))
  {
    free(typeBloc);
    readTwoStrings(command);
  }
  else if (compareString(typeBloc, stringEnergy))
  {
    free(typeBloc);
    readStringAndNumber(command);
  }
  else 
  {
    free(typeBloc);
    command->type = Error;
    readLineToTheEnd('#', command);
  }
}

CommandPtr readLine()
{
  CommandPtr command = makeCommand();
  int lastChar = getchar();
  if (lastChar == '\n' || lastChar == '#')
  {
    command->type = Ignore;
    readLineToTheEnd(lastChar, command);
    return command;
  }

  char* typeBloc;
  int typeBlocSize;
  readTypeBloc(&lastChar, &typeBloc, &typeBlocSize);
  
  if (lastChar == EOF)
  {
    command->type = EndOfFile;
    free(typeBloc);
    return command;
  }

  //Pusty typ polecenia lub niepuste polecenie i brak spacji po nim. 
  if (typeBlocSize == 0 || lastChar != ' ')
  {
    command->type = Error;
    free(typeBloc);
    readLineToTheEnd(lastChar, command);
    return command;
  }

  readArguments(typeBloc, command);
  return command;
}

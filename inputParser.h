#ifndef INPUTPARSER_H
#define INPUTPARSER_H

//Typ mowiacy jakiego typu jest polecenie z wejscia. Poza 
//standardowymi z tresci definiujemy Ignore (zaczynajace
//sie '#' lub puste), EndOfFile (konczace sie znakiem konca
//pliku), IgnoreEOF (zaczynajace sie '#' i konczace EOF - 
//wtedy nie ma bledu), Error (blad niezwiazany z EOF).
enum commandType {Declare, Remove, Valid, Equal, GetEnergy,
  SetEnergy, Ignore, IgnoreEOF, EndOfFile, Error};

struct Command
{
  enum commandType type;
  char* history1;
  int historySize1;
  char* history2; //Druga historia do polecenia EQUAL,
  //Jesli typ polecenia jest inny, ignorujemy te komorki.
  int historySize2;
  uint64_t energy; //Argument dla dwuargumentowego polecenia
  //ENERGY. Jesli typ polecenia jest inny, ignorujemy te komorke.
};

typedef struct Command* CommandPtr;
extern CommandPtr readLine();
extern void normaliseString(char* string, int* stringSize);

#endif /* INPUTPARSER_H */
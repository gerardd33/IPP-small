# Pierwszym argumentem jest sciezka do programu wykonywalnego lub jego nazwa.
# Drugim argumentem jest sciezka do folderu z testami. 
# Raporty: OK - poprawna odpowiedz, WA ("Wrong Answer") - niepoprawna odpowiedz / blad

#!/bin/bash
PROG=$1 
DIR=$2 

# Sprawdza, czy pierwszym argumentem jest nazwa pliku.
FIRSTLETTER=$(echo $PROG | cut -c1-1)
if ! [ "$FIRSTLETTER" == "." ] && ! [ "$FIRSTLETTER" == "/" ]
  then PROG="./$PROG"
fi

make
touch user.out user.err

for TEST in $(ls $DIR | grep .in$)
do
  
  TESTNAME="${TEST%.*}"
  echo "Checking: $TESTNAME"
  TEST="$DIR/$TESTNAME"

  IN="$TEST.in"
  OUT="$TEST.out"
  ERR="$TEST.err"

  # Bledy z valgrinda wypisywane sa do user.err
  valgrind -q --leak-check=full --show-leak-kinds=all \
  --errors-for-leak-kinds=all --track-origins=yes \
  $PROG < $IN > user.out 2> user.err

  OUT_RESULT="$(diff -bq user.out $OUT)"
  ERR_RESULT="$(diff -bq user.err $ERR)"

  printf "|  OUT: "
  if [ -z "$OUT_RESULT" ]
    then tput setaf 2 && tput bold && printf "OK"
    else tput setaf 1 && tput bold && printf "WA"
  fi
  tput sgr0 && printf "  |"

  printf "  ERR: "
  if [ -z "$ERR_RESULT" ]
    then tput setaf 2 && tput bold && printf "OK"
    else tput setaf 1 && tput bold && printf "WA"
  fi
  tput sgr0 && printf "  |"

  if ! [ -z "$OUT_RESULT" ] || ! [ -z "$ERR_RESULT" ]
    then echo && echo && printf "$TESTNAME: "
    tput setaf 1 && tput bold && printf "WRONG ANSWER!"
    tput sgr0 && echo
  fi
  echo && echo

done

rm user.out user.err

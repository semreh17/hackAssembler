#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "symbolTable.h"

const int numDefaultSymbols = 23;

// array di struct di tipo symbol
const symbol defaultSymbols[] = {
    {0, "SP"},   {1, "LCL"},         {2, "ARG"},     {3, "THIS"}, {4, "THAT"},
    {0, "R0"},   {1, "R1"},          {2, "R2"},      {3, "R3"},   {4, "R4"},
    {5, "R5"},   {6, "R6"},          {7, "R7"},      {8, "R8"},   {9, "R9"},
    {10, "R10"}, {11, "R11"},        {12, "R12"},    {13, "R13"}, {14, "R14"},
    {15, "R15"}, {0x4000, "SCREEN"}, {0x6000, "KBD"}};

void parseSymbols(char *line, symbolTable *st) {
  static unsigned short numeroRiga = 0;

  if (isInstruction(line)) {
    numeroRiga++;
    //        printf("--%u\n ",numeroRiga);
    return;
  }

  line = pulisciRiga(line);
  if (strlen(line) == 0) {
    return;
  }
  if (line[0] == '(') {

    // i simboli non cominciano con numeri!
    if (line[1] != '\0' && !(line[1] <= '9' && line[1] >= '0')) {
      char *end = strchr(line, ')');
      if (end != NULL) {
        *end = '\0'; // rimuove ')' da line
        line++;      // line è solo il simbolo

        addSymbol(line, numeroRiga, st);
        //                printf("questo e' il numero della riga %u\n",
        //                numeroRiga);
        return;
      }
    }
  }

  printf("Errore nel simbolo a riga: %d\n", numeroRiga);
  abort();
}

/* Sostituisce un simbolo in istruzioni assembly con l'indirizzo in tabella */

void replaceSymbols(char *line, symbolTable *st) {
  static unsigned short varaddr = 16;

  line = pulisciRiga(line);
  if (line[0] == '@') {
    /* TODO */
    // i simboli non cominciano con numeri! vedi slides.
    // se è un simbolo (non un indirizzo), cercalo in tabella altrimenti
    // aggiungilo. in entrambi i casi, applica la sostituzione con sprintf

    // estraggo il simbolo dalla a instruction
    char symbol[1000];
    sscanf(line, "@%s", symbol);

    // non deve essere un numero
    if (!(symbol[0] >= '0' && symbol[0] <= '9')) {

      // controllo se è presente nella tabella default
      for (int i = 0; i < numDefaultSymbols; ++i) {
        if (strcmp(symbol, defaultSymbols[i].name) == 0) {
          sprintf(line, "@%u", defaultSymbols[i].address);
          return;
        }
      }

      // se è un simbolo lo cerco nella symboltable
      unsigned short address = findBySymbol(symbol, st);

      // se non è stato trovato lo aggiungo
      if (address == MISSING) {
        addSymbol(symbol, varaddr, st);
        address = varaddr;
        varaddr++;
      }

      // rimpiazzo il simbolo con il suo address
      sprintf(line, "@%u", address);
      puts(line);
    }
  }
}

/* Cerca un simbolo in tabella, tramite hashvalue */

unsigned short findBySymbol(char *name, symbolTable *st) {
  size_t h = hash(name, st->tableLen);
  while (strlen(st->symbols[h].name) != 0 &&
         strcmp(st->symbols[h].name, name) != 0) {
    h = (h + 1) % st->tableLen;
  }
  if (strlen(st->symbols[h].name) == 0) {
    return MISSING;
  }

  return st->symbols[h].address;
}

/* Aggiunge un simbolo in tabella */

void addSymbol(const char *name, unsigned short address, symbolTable *st) {
  int h = hash(name, st->tableLen);
  while (strlen(st->symbols[h].name) != 0)
    h = (h + 1) % st->tableLen;

  symbol sym;
  sym.address = address;
  strcpy(sym.name, name);

  st->symbols[h] = sym;

  if (st->size++ >= st->tableLen / 2)
    expand(st);
}

/* per espansione tabella e hashing */

void initDefault(symbolTable *st) {
  st->symbols = calloc(INITIAL_SIZE, sizeof(symbol));
  st->tableLen = INITIAL_SIZE;
  st->size = 0;

  // what? Perché fa una calloc uguale per la seconda volta sempre su
  // st->symbols
  st->symbols = calloc(INITIAL_SIZE, sizeof(symbol));

  copySymbols(st, defaultSymbols, numDefaultSymbols);
}

void copySymbols(symbolTable *dest, const symbol *src, const size_t srclen) {
  int i;

  // itera 23 volte perchè in questa funzione viene passato numDefaultSymbols
  // che è definito globalmente uguale a 23 (23 è il numero di simboli default)
  for (i = 0; i < srclen; i++) {

    // controlla che l'attributo name, dello struct attualmente puntato, non sia
    // vuoto in caso positivo chiama la funzione addSymbol, passandogli i due
    // attributi dello struct defaultSymbols attualmente puntato e la
    // symbolTable inizializzata in assembler.c
    if (strlen(src[i].name) != 0) {
      addSymbol(src[i].name, src[i].address, dest);
    }
  }
}

void expand(symbolTable *st) {
  symbol *tableVals = st->symbols;
  size_t oldLen = st->tableLen;

  /* double the table size and add 1 on expansion */
  st->tableLen = (st->tableLen * 2) + 1;
  st->symbols = calloc(st->tableLen, sizeof(symbol));
  st->size = 0;

  copySymbols(st, tableVals, oldLen);

  free(tableVals);
}

size_t hash(const char *key, size_t M) { // genera hash values

  int h, a = 31415, b = 27183;

  for (h = 0; *key != 0; key++)
    h = (a * h + *key) % M;

  return (h < 0) ? (h + M) : h;
}

#include "parser.h"
#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// per semplicità usiamo delle semplici coppia chiave-valore, ma si potrebbero
// usare strutture dati apposite e performanti come le hash maps (come
// symbolTable). A differenza delle hash maps, queste semplici mappe si
// scandiscono sequenzialmente.

// I valori sono numerici: in assembler.c (funzione output) tradurremo in
// "binario"

// La dimensione delle tabelle (cioè XXX qui sotto) è parte della struttura
// solo per facilitare la scansione riga-per-riga: ovvero con un for da
// indice 0 a quel numero-1.
// Si potrebbe evitare di scriverlo hard-coded, ma andrebbe adattato il codice.

tabella compTableNoa = {18,
                        {           // sostituire XXXX col numero di righe
                         {"0", 42}, // vedi tabella nelle slide: 101010
                         {"1", 63}, // idem: 111111
                         /* TODO */
                         {"-1", 58},
                         {"D", 12},
                         {"A", 48},
                         {"!D", 13},
                         {"!A", 49},
                         {"-D", 15},
                         {"-A", 51},
                         {"D+1", 31},
                         {"A+1", 55},
                         {"D-1", 14},
                         {"A-1", 50},
                         {"D+A", 2},
                         {"D-A", 19},
                         {"A-D", 7},
                         {"D&A", 0},
                         {"D|A", 21}}};

tabella compTableWitha = {10,
                          {/* TODO */
                           {"M", 48},
                           {"!M", 49},
                           {"-M", 51},
                           {"M+1", 55},
                           {"M-1", 50},
                           {"D+M", 2},
                           {"D-M", 19},
                           {"M-D", 7},
                           {"D&M", 0},
                           {"D|M", 21}}};

tabella jumpTable = {8,
                     {
                         // sostituire XXXX col numero di righe
                         {"NULL", 0},
                         {"JGT", 1},
                         {"JEQ", 2},
                         /* TODO */
                         {"JGE", 3},
                         {"JLT", 4},
                         {"JNE", 5},
                         {"JLE", 6},
                         {"JMP", 7},
                     }};

tabella destTable = {8,
                     {
                         // sostituire XXXX col numero di righe
                         {"NULL", 0},
                         {"M", 1},
                         {"D", 2},
                         {"MD", 3},
                         /* TODO */
                         {"A", 4},
                         {"AM", 5},
                         {"AD", 6},
                         {"AMD", 7},
                     }};

instruction *parseInstruction(char *riga) {

  riga = pulisciRiga(riga);

  if (strlen(riga) == 0)
    return NULL;

  if (riga[0] == '(')
    return NULL;

  if (riga[0] == '@') { // è una A-instruction

    if (riga[1] <= '9' && riga[1] >= '0') {

      /* TODO */
      // creare l'istruzione
      // assegnare il tipo A
      // prendere l'indirizzo, per esempio con sscanf (vedi main)
      // se ok, testare il range (per es. usare shift): è a 15 bit?
      // assegnare l'indirizzo e ritornare l'istruzione

      // Dichiarare il puntatore alla variabile di tipo "istruction" e
      // istanziare la  memoria necessaria

      instruction *inst = (instruction *)malloc(sizeof(instruction));
      if (inst == NULL) {
        exit(EXIT_FAILURE);
      }

      inst->type = A;
      int address;
      if (sscanf(riga + 1, "%d", &address) == 1) {
        // controllo se l'indirizzo è 15 bit
        if (address >= 0 && address <= 0x7FFF) {
          inst->valore = (unsigned short)address;
          return inst;
        }
      }
      free(inst);
      return NULL;
    }
  } else {

    // le 3 parti di una C-instruction (vedi parser.h)
    unsigned char comp, dest, jump, a;

    /* TODO */
    // usare parseC: passare le 3 componenti per riferimento: così verranno
    // settate da parseC
    // parseC può settare MISSING o ERROR in ciascuna componente
    // creare l'istruzione e ritornarla
    parseC(riga, &comp, &dest, &jump, &a);

    instruction *inst = (instruction *)malloc(sizeof(instruction));
    if (inst == NULL) {
      exit(EXIT_FAILURE);
    }

    inst->type = C;
    inst->comp = comp;
    inst->a = a;
    inst->dest = dest;
    inst->jump = jump;
    return inst;
  }

  // dummy, solo per compilare (questa è una funzione non-void)
  abort();
}

// fa il parsing della C-instruction ed assegna i valori alle 3 componenti
void parseC(char *riga, unsigned char *comp, unsigned char *dest,
            unsigned char *jump, unsigned char *a) {

  *dest = 0;
  *jump = 0;
  *a = 0;

  // strchr ritorna un puntatore alla prima occorrenza del carattere
  char *compSubString = strchr(riga, '=');
  char *jumpSubString = strchr(riga, ';');

  if (jumpSubString != NULL) {
    jumpSubString[0] = '\0'; // ora riga termina prima del ';' cioè è dest=comp
    // oppure *(jumpSubString) = '\0';
    jumpSubString++;
    jumpSubString = rimuoviSpazi(jumpSubString);
    // jumpSubString punta solo alla direttiva di jump (JEQ, JMP...)
    // la cerchiamo nella tabella giusta e ritorniamo la codifica
    // cioè un unsigned char (8 bit, anche se basterebbero 6+1).
    *jump = traduciInBinario(jumpSubString, &jumpTable);
  } else {
    *jump = 0;
  }

  if (compSubString != NULL) {

    // caso dest=comp
    // usare riga come sopra per estrarre dest e comp

    /* TODO */

    int foundM = 0;

    *compSubString = '\0';
    compSubString = rimuoviSpazi(compSubString + 1);

    for (int i = 0; i < strlen(compSubString); ++i) {
      if (compSubString[i] == 'M') {
        foundM = 1;
      }
    }
    if (foundM == 0) {
      *comp = traduciInBinario(compSubString, &compTableNoa);
    } else {
      *a = 1;
      *comp = traduciInBinario(compSubString, &compTableWitha);
    }
    //        printf("comp tradotto: %u\n", *comp);

    char *destSubString = riga;
    destSubString = rimuoviSpazi(destSubString);

    *dest = traduciInBinario(destSubString, &destTable);

  } else {

    // è presente solo comp
    /* TODO */
    int foundM = 0;
    for (int i = 0; i < sizeof(comp); ++i) {
      if (comp[i] == 'M') {
        foundM = 1;
      }
    }
    if (foundM == 0) {
      *comp = traduciInBinario(riga, &compTableNoa);
    } else {
      *a = 1;
      *comp = traduciInBinario(riga, &compTableWitha);
    }
  }
}

int isInstruction(char *riga) {
  // ritorniamo 1 se la riga è una instruction, altrimenti 0.
  /* TODO */
  riga = pulisciRiga(riga);
  if (riga[0] == '(' || riga[0] == '\n' || riga[0] == '\0') {
    return 0;
  } else if (riga[0] >= '0' && riga[0] <= '9') {
    return 1;
  }
  return ERROR;
}

// traduce, semplicemente cercando nelle tabelle
unsigned char traduciInBinario(char *chiave, tabella *tabella) {

  int i;
  //    printf("chiave %s\n", chiave);
  //    printf("lunghezza tabella %d\n", tabella->lunghezza);

  for (i = 0; i < tabella->lunghezza; i++) {
    if (strcmp(chiave, tabella->righe[i].chiave) == 0) {
      return tabella->righe[i].valore;
    }
  }

  return MISSING;
}

char *pulisciRiga(char *riga) {

  riga = rimuoviSpazi(riga);
  char *commento = strstr(riga, "//");
  if (commento != NULL) {
    *commento = '\0';          // terminiamo la stringa
    riga = rimuoviSpazi(riga); // in caso ci fossero spazi prima del commento
  }

  return riga;
}

// rimuovere spazi all'inizio ed alla fine della stringa.
// in un paio di esercizi in classe abbiamo visto sia come eliminarli solo
// all'nizio della stringa che come farlo anche all'interno. qui serve solo
// il trimming ad inizio e fine. per farlo alla fine segue un ragionamento
// simile a come farlo all'inizio della stringa.
char *rimuoviSpazi(char *str) {

  /* TODO */

  // rimuovere spazi in testa
  while (*str && (*str == ' ' || *str == '\t')) {
    str++;
  }

  // rimuovere spazi in coda
  char *end = str + strlen(str) - 1;
  while (end > str && (*end == ' ' || *end == '\t')) {
    end--;
  }
  end[1] = '\0';

  return str;
}

// se l'ultimo carattere è un newline lo togliamo (terminando la stringa)
void rimuoviNewLine(char *str) {

  /* TODO */
  int i = 0;
  // scorro tutta la riga e controllo ogni carattere
  // che sia diverso dal carattere newline

  while (str[i] != '\0') {
    if (str[i] == '\n') {
      str[i] = '\0';
    } else {
      i++;
    }
  }
}

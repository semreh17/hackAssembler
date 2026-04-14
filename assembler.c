#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "parser.h"
#include "symbolTable.h"


//data è il *numero* corrispondente alla codifica binaria a 16 bit
void output(FILE *f, unsigned short data) {
    
    char temp[18];
    for (int i = 0; i < 16; ++i) {
        temp[i] = '0';
    }
	//dobbiamo stampare in "binario" (ovvero sequenza di caratteri da scrivere nel file
	//.hack, che è sempre un file di testo) le cifre di data
        
    //essendo un file di testo, ad ogni riga dobbiamo ricordarci di 
    //inserire un carattere di newline sequito dal nul (fine stringa).


    /* TODO */
    int numerDiBit = 15;
    int actualBinaryDigit;
    while (data >= 1) {
        actualBinaryDigit = data % 2;
        data = data/2;
        temp[numerDiBit] = actualBinaryDigit + '0';
        numerDiBit--;
    }

    temp[16] = '\n';
    temp[17] = '\0';

    printf("in binario: %s\n", temp);
    fprintf(f, "%s", temp);

}


//legge il file, traduce, ed invoca la funzione output
void assemble(FILE *asmfile, FILE *hackfile) {

    char buf[256]; //max 256 chars per riga dal file
    
    //inizializzazione symbol table
    symbolTable st;
    initDefault(&st);

    //prima passata (vedi slides su assemblatore)
    // qui credo si riferisca all'inserimento delle
    // etichette dichiarate tramite direttiva
    // (YYY) con relativo valore
    //semplice implementazione. la struttura può essere migliorata. 
    
    while(fgets(buf, 256 , asmfile)!=NULL) {

		rimuoviNewLine(buf);
		
		printf("ho letto %s\n", buf);
         
        //completamento della symbol table  
		parseSymbols(buf, &st);

    }
 
    //riposiziona il puntatore di lettura del file all'inizio del file
    rewind(asmfile);

    //seconda passata (vedi slides su assemblatore)
    // i simboli in A-instruction non ancora in symbol
    // table vengono aggiunti con relativi valori
    while(fgets(buf, 256 , asmfile)!=NULL) {

        // non so quanto senso abbia richiamare questa funzione
        // in quanto i caratteri di newlIne
        rimuoviNewLine(buf);

        //usiamo la symbol table
        replaceSymbols(buf, &st);

        instruction* op;

        //da stringa buf a type instruction (che ha campi valore, dest, comp, jump,
        //codificati già in binario - parte del progetto da completare: vedi parser.c).
        op = parseInstruction(buf);


        //data deve essere la codifica in binario finale di tutta l'instruction

        if(op != NULL) {
            unsigned short data;

            if(op->type == A) {
                data = op->valore;

            }else{

            //invece di prendere le codifiche in binario di dest,comp,jump
            //e concatenarle direttamente come stringhe (facendo attenzione che
            //ciascuna abbia la lunghezza giusta), usiamo shift per calcolare il
            //valore di data.

            //poi la conversione in stringa avverrà nella funzione
            //output (in questo modo separiamo la logica dell'assemblatore da
            //quella di scrittura sul file testuale .hack).

            //formato C-instuctions: 111 a c1-c6 d1-d3 j1-j3
            //quindi 7 (cioè 0x7) shifted di 13 + comp di 6 + ...

            /* TODO */
                data = 0;
                //data = (0x7 << 13) + (0x3F << 6) + (0x7 << 3) + 0x7;

//                printf("prima di tutto: %u\n", data);
                // Format: 111 a c1-c6 d1-d3 j1-j3
                data = 0b111 << 13;  // opcode (111) nei 3 bit leftmost
//                printf("opcode: %u\n", data);

                data|= (op->a << 12);
//                printf("BIT A: %u\n", op->a);

                // calcola la rappresentazione in binario per la parte comp
                unsigned char compBinary = op->comp;
                data |= (compBinary << 6);
//                printf("comp: %u\n", compBinary);

                // calcola la rappresentazione in binario per la parte dest
                unsigned char destBinary = op->dest;
                data |= (destBinary << 3);
//                printf("dest: %u\n", destBinary);


                // calcola la rappresentazione in binario per la parte jump
                unsigned char jumpBinary = op->jump;
                data |= jumpBinary;
//                printf("jump: %u\n", jumpBinary);

           }

           //scriviamo data sul file di destinazione

           printf("numero: %u\n", data);

           output(hackfile, data);
           free(op);

        }
    }
}





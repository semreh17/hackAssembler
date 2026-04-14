#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "parser.h"
#include "symbolTable.h"


void output(FILE *asmfile, unsigned short data);

void assemble(FILE *asmfile, FILE *hackfile);

#endif

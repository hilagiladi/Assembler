
#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"


int main(int argc, char *argv[]) {
    int i;
    FILE *sourceFile;

    /*prints a massage to the user that tells about the program*/
    printf("\nHello!\nThis program writes an assembler for a unique assembly language and at the end creates output files.\n\n");


    /*checking if the user didn't insert any file name*/
    if (argc < 2) {
        printf("\nYou must write at least on name of file in the commend line.\n");
        exit(0);
    }

    /* run the assembler on every file*/
    for (i = 1; i < argc; i++) {
        initialize();/*initialize all the tables for the next file*/
        sourceFile = openAndAssembleFile(argv, i);/*open the next file*/
        preAssembler(sourceFile);  /*start assemble the file*/
        freeAllocatedMemory(); /*free allocate memory after finish assembling all the files */
    }
    return 0;
}
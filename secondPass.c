
#include "assembler.h"
#include "constants.h"
#include "encoding.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

/*assembling the file line by line - second pass*/
void secondPassLine(char *line);

/*Functions*/

void secondPass(FILE *afterMacroFile) {
    char line[MAX_LINE_LEN];
    /* step 1:*/
    globalData.IC = START_ADDRESS;


    /* if its is an empty or comment line the assembler will skip it.*/
    for (globalData.lineNumber = 1; fgets(line, MAX_LINE_LEN, afterMacroFile); globalData.lineNumber++) {
        if (!ignoreLine(line)) {
            /* stage 2*/
            secondPassLine(line);
        }
    }
    /*closing the afterMacroFile after finishing to assemble it*/
    fclose(afterMacroFile);

    /* stage 10:*/
    /*checking if they were any errors in the source file, if so we stop trying to compile this file*/
    if (globalData.errorFlag == TRUE) {
        return;
    }

    /* in the end of the second pass, if there were no errors we write the output files*/
    writeOutputFiles();
}

void secondPassLine(char *line) {
    int dataInstructionNumber;
    char *instruction;
    char *parameters;

    /* getting fields from the line*/
    instruction = getInstruction(line);
    parameters = getParameters(line);
    removeSpaces(parameters);  /*we checked that the parameters are valid in the first pass*/

    /* step 4*/
    /* we encoded them in the first pass*/
    dataInstructionNumber = searchInArray(dataInstructionsNames, DATA_INSTRUCTIONS_NUM, instruction);
    if (dataInstructionNumber == dot_data || dataInstructionNumber == dot_string ||
        dataInstructionNumber == dot_struct || dataInstructionNumber == dot_extern) {
        return;
    }

    /* step 5:*/
    if (dataInstructionNumber == dot_entry) {
        struct Node *node = searchSymbol(globalData.symbolTableHead, parameters);

        if (node == NULL) {
            printError("entry label is not in the symbol table");
            return;
        }
        if (node->data->type == EXTERNAL) {
            printError("label is defined as .entry and .extern");
            return;
        }

        /*step 6:*/
        node->data->isEntry = TRUE;  /*the current symbol is from type ENTRY*/
        return;
    }
    /* step 7 + 8 (IC is updated inside the function)*/
    encodeAdditionalWords(parameters);
}

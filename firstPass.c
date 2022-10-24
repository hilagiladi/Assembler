
#include "assembler.h"
#include "constants.h"
#include "encoding.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

/*assembling the file line by line - first pass*/
void firstPassLine(char *line);

/*first pass for data instructions*/
void firstPassData(int labelFlag, char *label, char *instruction, char *parameters, struct Data data);

/*first pass for data instructions first group: .data, .string, .struct*/
void dataFirstGroup(int labelFlag, char *label, int dataInstructionNumber, char *parameters, struct Data data);

/*first pass for data instructions first group: .extern, .entry*/
void dataSecondGroup(char *label, int dataInstructionNumber, char *parameters, struct Data data);

/*first pass for actions*/
void firstPassInstruction(int labelFlag, char *label, char *instruction, char *parameters, struct Data data);


/*Functions*/

void firstPass(FILE *afterMacroFile) {
    struct Node *current;
    char line[MAX_LINE_LEN];

    /*  step 1:*/
    globalData.IC = START_ADDRESS;
    globalData.DC = 0;

    line[MAX_LINE_LEN - 2] = '\n';  /*to check the line length*/

    /*rewind the afterMacroFile to start read it again*/
    rewind(afterMacroFile);

    /* if its is an empty or comment line the assembler will skip it.*/
    for (globalData.lineNumber = 1; fgets(line, MAX_LINE_LEN, afterMacroFile); globalData.lineNumber++) {
        if (!ignoreLine(line)) {
            /*check if the line is too long and prints error if it is*/
            if (line[MAX_LINE_LEN - 2] != '\n' && line[MAX_LINE_LEN - 2] != '\0') {
                line[MAX_LINE_LEN - 2] = '\n';  /*for the next check of line length*/
                printError("Line is too long - the maximum allowed line length is 80.");
                fgets(line, MAX_LINE_LEN, afterMacroFile);
            } else
                /* stage 2*/
                firstPassLine(line);
        }
    }


    /*stage 16:*/
    /*checking if they were any errors in the source file, if there were, we stop trying to compile this file*/
    if (globalData.errorFlag == TRUE) {
        fclose(afterMacroFile);
        return;
    }

    /* stage 17:*/
    /*updating all the data symbol with IC value*/
    current = globalData.symbolTableHead;
    while (current != NULL && globalData.symbolTableHead->data != NULL) {
        if (current->data->type == DATA)
            current->data->value += globalData.IC;
        current = current->next;
    }

    /*rewind the afterMacroFile to start read it again in second pass*/
    rewind(afterMacroFile);

    /*start second pass*/
    secondPass(afterMacroFile);

}

void firstPassLine(char *line) {
    char *label;
    char *instruction;
    char *parameters;
    struct Data data;
    int labelFlag;

    data.isEntry = FALSE;  /*in the second pass it can change to "TRUE"*/
    labelFlag = FALSE;

    /* getting fields from the line*/
    label = getLabel(line);
    instruction = getInstruction(line);
    parameters = getParameters(line);

    /* stage 3: check if there is a label and if the label is valid*/
    if (label != NULL && isValidLabel(label)) {
        /*stage 4:*/
        labelFlag = TRUE;
    }

    /*printing error if the label it's not valid, and it is not extern or entry instruction */
    if (label != NULL && !isValidLabel(label) &&
        (strcmp(instruction, ".entry") != 0 && strcmp(instruction, ".extern") != 0)) {
        printError("label is not valid");
        return;
    }

    /*continue second pass for data instruction or action */
    if (isDataInstructionsName(instruction))
        firstPassData(labelFlag, label, instruction, parameters, data);
    else
        firstPassInstruction(labelFlag, label, instruction, parameters, data);


}

void firstPassData(int labelFlag, char *label, char *instruction, char *parameters, struct Data data) {
    int dataInstructionNumber;

    /*getting the instruction*/
    dataInstructionNumber = searchInArray(dataInstructionsNames, DATA_INSTRUCTIONS_NUM, instruction);

    /* stage 5:*/
    /*continue second pass for 2 groups of instructions:
     * group 1: data, string and struct
     * group 2: entry and extern */
    if (dataInstructionNumber == dot_data || dataInstructionNumber == dot_string || dataInstructionNumber == dot_struct)
        dataFirstGroup(labelFlag, label, dataInstructionNumber, parameters, data);
    else
        dataSecondGroup(label, dataInstructionNumber, parameters, data);
}

void dataFirstGroup(int labelFlag, char *label, int dataInstructionNumber, char *parameters, struct Data data) {
    /* stage 6:*/
    if (labelFlag == TRUE) {
        /* check if the symbol already exist*/
        if (searchSymbol(globalData.symbolTableHead, label) != NULL) {
            printError("Label already exist");
            return;
        }

        /*defined the symbol and insert it to the symbol table*/
        data.symbol = label;
        data.type = DATA;
        data.value = globalData.DC;
        insertSymbol(data);
    }
    /* stage 7:*/
    /*encode all the words of the data instructions*/
    if (dataInstructionNumber == dot_string)
        encodeString(parameters);
    else if (dataInstructionNumber == dot_struct)
        encodeStruct(parameters);
    else {
        if (!validCommas(parameters)) {
            printError("Invalid parameters syntax (commas) in .data installation.");
            return;
        }
        removeSpaces(parameters);
        encodeData(parameters);
    }

}

void dataSecondGroup(char *label, int dataInstructionNumber, char *parameters, struct Data data) {
    /* stage 8:*/

    /*checking if we have more than one symbol after the entry or extern instruction*/
    if (countParameters(parameters) != 1) {
        printError("invalid number of parameters on .extern or .entry instruction, should be 1 parameter.");
        return;
    }

    /*stage 9:*/
    /*printing warning if the label it's not valid in extern or entry instruction */
    if (label != NULL && !isValidLabel(label)) {
        printf("\nWarning! in file: %s on line %d: label before .extern or .entry instruction is not valid\n.",
               globalData.sourceFileName, globalData.lineNumber);
    }


    /*checking if there is a symbol before .extern or .entry and print a warning without stopping the program */
    if (label != NULL) {
        printf("\nWarning! in file: %s on line %d: label can not be defined at the beginning of a .extern or .entry instruction\n.",
               globalData.sourceFileName, globalData.lineNumber);
    }

    if (dataInstructionNumber == dot_extern) {
        /* check if the parameter label is valid*/
        if (!isValidLabel(parameters)) {
            printError("Invalid label after .extern");
            return;
        }
        /* check if the symbol already exist*/
        if (searchSymbol(globalData.symbolTableHead, parameters) != NULL) {
            printError("extern Label already exist");
            return;
        }

        /*defined the symbol and insert it to the symbol table*/
        data.value = 0;
        data.symbol = parameters;
        data.type = EXTERNAL;
        insertSymbol(data);

    } else { /*it is .entry instruction*/
        /* check if the parameter label is valid*/
        if (!isValidLabel(parameters)) {
            printError("Invalid label after .entry");
            return;
        }
    }
}

void firstPassInstruction(int labelFlag, char *label, char *instruction, char *parameters, struct Data data) {
    /* stage 11:*/
    int actionNumber;

    if (labelFlag == TRUE) {
        /* check if the symbol already exist*/
        if (searchSymbol(globalData.symbolTableHead, label) != NULL) {
            printError("Label already exist");
            return;
        }

        /*defined the symbol and insert it to the symbol table*/
        data.symbol = label;
        data.type = CODE;
        data.value = globalData.IC;
        insertSymbol(data);
    }
    /* stage 12:*/
    /*getting the action number*/

    if (instruction == NULL) {
        printError("missing instruction");
        return;
    }

    actionNumber = searchInArray(actionsNames, ACTIONS_NAMES_NUM, instruction);
    if (actionNumber == -1) {
        printError("Invalid instruction name");
        return;
    }

    /*checking if the parameter's syntax (commas) are valid*/
    if (!validCommas(parameters)) {
        printError("Invalid parameters syntax (commas)");
        return;
    }
    removeSpaces(parameters);

    /* stage 13:*/
    /*encode the first word*/
    encodeInstructionFirstWord(actionNumber, parameters);

    /* stage 14:*/
    /* calculate L using the first word we just encoded - it has the addressing methods*/
    globalData.IC += calculateL(actionNumber, getBits(globalData.instructionImage[globalData.IC - START_ADDRESS], 4, 5),
                                getBits(globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 3));

}


#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"


/*Declarations*/

/*writing the macro line by line */
void preAssemblerLine(char *line, FILE *sourceFile, FILE *afterMacroFile);

/*getting the first word in the line - for checking if its "macro"*/
char *getMacro(char *line);

/*returns TRUE or FALSE if the macro is valid and prints error if it is not */
int isValidMacroName(char *macroName);

/*getting all the macro data until we see "endmacro"*/
void readMacro(char *line, FILE *sourceFile);

/*Functions*/

void preAssembler(FILE *sourceFile) {
    char line[MAX_LINE_LEN];
    FILE *afterMacroFile;
    globalData.lineNumber = 1;

    /*creating a file with ending ".am" to write on the file after reading the macros*/
    afterMacroFile = createFile(".am", "w+");

    /* read line from the file: */
    /* if its is an empty or comment line we write it to the file as it is,
     * otherwise we preassembling it.*/
    for (globalData.lineNumber = 1; fgets(line, MAX_LINE_LEN, sourceFile); globalData.lineNumber++) {
        if (ignoreLine(line))
            fprintf(afterMacroFile, "%s",
                    line);
        else
            preAssemblerLine(line, sourceFile, afterMacroFile);
    }

    /*closing the source file after finishing to read*/
    fclose(sourceFile);

    /*checking if they were any errors in the source file, if there were, we stop trying to compile this file*/
    if (globalData.errorFlag == TRUE) {
        fclose(afterMacroFile);
        return;
    }

    firstPass(afterMacroFile);  /*start assemble the file*/
}

void preAssemblerLine(char *line, FILE *sourceFile, FILE *afterMacroFile) {
    char *instruction;
    struct MacroNode *macroNode;

    instruction = getMacro(line);


    /* checking if it's a macro that already defined, if it is we copy all its data to the afterMacroFile */
    macroNode = searchMacro(globalData.macroTableHead, instruction);
    if (macroNode != NULL) {  /*for example line is "m1"*/
        fprintf(afterMacroFile, "%s", macroNode->data->value);
        return;
    }

    /* if it's a new macro we start to read it*/
    if (instruction != NULL && strcmp(instruction, "macro") == 0) {
        readMacro(line, sourceFile);
        return;
    }

    /*write the regular line to the file*/
    fprintf(afterMacroFile, "%s", line);

}

int isValidMacroName(char *macroName) {
    /*checking if the macro name is valid according to the rules of label */
    if (isValidLabel(macroName))
        return TRUE;
    return FALSE;
}

char *getMacro(char *line) {
    char *lineCopy;
    char *macro;

    lineCopy = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(lineCopy);
    globalData.pointers[globalData.pointersCounter++] = lineCopy;
    strcpy(lineCopy, line);

    macro = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(macro);
    globalData.pointers[globalData.pointersCounter++] = macro;

    /*getting the first word and remove its spaces*/
    macro = strtok(lineCopy, " ");
    removeSpaces(macro);

    return macro;

}

void readMacro(char *line, FILE *sourceFile) {
    char temp[MAX_LINE_LEN];
    int macroFlag;
    struct MacroData data;
    macroFlag = TRUE;

    data.name = getParameters(line); /*update macro name*/

    /*checking if the macro name is valid. if not prints an error*/
    if (!isValidMacroName(data.name)) {
        printError("Macro name not valid");
        return;
    }

    /*checking if the macro already exist, if it is prints an error */
    if (searchMacro(globalData.macroTableHead, data.name) != NULL) {
        printError("This macro name was already defined earlier");
        return;
    }

    /*allocate memory to the data value
     * according to the guidance given in the forum, it can be assumed that the macro content is limited to up to 10 lines,
     * so we will perform the allocation in this way and not by using realloc which is a more expensive allocation */
    data.value = (char *) malloc(MAX_LINE_LEN * 10 + 1);
    checkMemoryAllocation(data.value);
    globalData.pointers[globalData.pointersCounter++] = data.value;

    /*the macro is valid - getting all its data:*/
    while (fgets(line, MAX_LINE_LEN, sourceFile)) {
        globalData.lineNumber++;
        strcpy(temp, line);
        removeSpaces(temp);

        /*checking if we macro is finished*/
        if (strcmp(temp, "endmacro") == 0) {
            /* stage 8:*/
            insertMacro(data);
            return;
        }

        /*adding more of the macro data*/
        if (macroFlag == TRUE) {
            strcpy(data.value, line);
            macroFlag = FALSE;
        } else {
            strcat(data.value, line);
        }
    }

}



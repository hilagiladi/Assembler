
#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

/*returns TRUE or FALSE if parameter is an action Name */
int isActionsName(char *parameter);

/* return the amount of chars we need for skipping the label if existing*/
int skipLabel(const char *line);


/*Functions*/

void freeAllocatedMemory() {
    int i;
    struct Node *currentSymbol;
    struct Node *tempSymbol;
    struct MacroNode *currentMacro;
    struct MacroNode *tempMacro;
    struct ExternalNode *currentExternal;
    struct ExternalNode *tempExternal;

    /* free the memory allocated for the file name */
    free(globalData.sourceFileName);

    /* free the symbol table*/
    currentSymbol = globalData.symbolTableHead;
    while (currentSymbol != NULL) {
        tempSymbol = currentSymbol->next;
        free(currentSymbol->data);
        free(currentSymbol);
        currentSymbol = tempSymbol;
    }

    /* free the external table*/
    currentExternal = globalData.externalTableHead;
    while (currentExternal != NULL) {
        tempExternal = currentExternal->next;
        free(currentExternal);
        currentExternal = tempExternal;
    }

    /* free the macro table*/
    currentMacro = globalData.macroTableHead;
    while (currentMacro != NULL) {
        tempMacro = currentMacro->next;
        free(currentMacro->data);
        free(currentMacro);
        currentMacro = tempMacro;
    }

    /*free all allocates memory during the program*/
    for (i = 0; i < globalData.pointersCounter; i++) {
        if (globalData.pointers[i] != NULL)
            free(globalData.pointers[i]);
    }

}

int skipSpacesFromTheStart(const char *line) {
    int i = 0;

    /*counting the spaces chars in the beginning of the string*/
    while (line != NULL && isspace(line[i]))
        i++;

    return i;
}

int skipSpacesFromTheEnd(const char *line) {
    int i;
    int cnt = 0;

    if (line == NULL)
        return 0;

    /*counting the number of chars needed to delete from the end of the string*/
    for (i = strlen(line) - 1; i >= 0 && isspace(line[i]); i--)
        cnt++;


    /*returning the first index of the string needed to delete*/
    return strlen(line) - cnt;
}

void removeSpaces(char *str) {
    int i;

    int count = 0; /* To keep track of non-space character count*/

    /* if the current char is not a space we increase count*/
    /*move it to index 'count++'.*/
    for (i = 0; str[i]; i++) {
        if (!isspace(str[i]))
            str[count++] = str[i];
    }

    str[count] = '\0';
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';
}

int skipLabel(const char *line) {
    int i;

    /*if we found a label we are returning the number of sales needed to skip*/
    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] == ':')
            return i + 1;
    }
    return 0;
}

int ignoreLine(char *line) {
    int i;

    /*check if it is a comment line*/
    if (line[0] == ';')
        return TRUE;

    /*check if it is an empty line*/
    for (i = 0; line[i] != '\0'; i++) {
        if (!isspace(line[i]))
            return FALSE;
    }
    return TRUE;  /*only spaces*/
}

char *getInstruction(char *line) {
    char *instruction;
    char *lineCopy;

    /*allocate memory for pointers and put it in the pointers array*/
    instruction = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(instruction);
    globalData.pointers[globalData.pointersCounter++] = instruction;

    lineCopy = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(lineCopy);
    globalData.pointers[globalData.pointersCounter++] = lineCopy;
    strcpy(lineCopy, line);

    /*checking if there is a label and skip it if there is*/
    if (strchr(lineCopy, ':') != NULL)
        lineCopy += skipLabel(lineCopy);

    lineCopy += skipSpacesFromTheStart(lineCopy);
    instruction = strtok(lineCopy, " ");

    instruction += skipSpacesFromTheStart(instruction);
    if (instruction == NULL)
        return NULL;

    /*putting the end char in the cell before the spaces */
    instruction[skipSpacesFromTheEnd(instruction)] = '\0';

    return instruction;
}

char *getLabel(char *line) {
    char *label;
    char *lineCopy;

    /*allocate memory for pointer and put it in the pointers array*/
    lineCopy = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(lineCopy);
    globalData.pointers[globalData.pointersCounter++] = lineCopy;
    strcpy(lineCopy, line);


    /* check if we have a label*/
    if (strchr(lineCopy, ':') == NULL)
        return NULL;

    label = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(label);
    globalData.pointers[globalData.pointersCounter++] = label;


    /* remove spaces from the start and from the end*/
    lineCopy += skipSpacesFromTheStart(lineCopy);
    lineCopy[skipSpacesFromTheEnd(lineCopy)] = '\0';

    /* there is a label - lets find it:*/
    label = strtok(lineCopy, ":");


    return label;
}

char *getParameters(char *line) {
    char *lineCopy;

    /*allocate memory for pointers and put it in the pointers array*/
    lineCopy = (char *) malloc(strlen(line) + 1);
    checkMemoryAllocation(lineCopy);
    globalData.pointers[globalData.pointersCounter++] = lineCopy;
    strcpy(lineCopy, line);


    lineCopy += skipSpacesFromTheStart(lineCopy);
    lineCopy += skipLabel(lineCopy);  /*skip the label if the line has a label*/
    lineCopy += skipSpacesFromTheStart(lineCopy);

    /* now we skip the action:*/
    while (!isspace(*lineCopy) && strlen(lineCopy) > 0)
        lineCopy++;

    /* now we clean the parameters from spaces in the start and in the end:*/
    lineCopy += skipSpacesFromTheStart(lineCopy);
    lineCopy[skipSpacesFromTheEnd(lineCopy)] = '\0';


    return lineCopy;
}

int isValidLabel(char *label) {
    char firstChar;
    int i;

    if (label == NULL)
        return FALSE;

    firstChar = label[0];

    /*checking if the label starts with an alpha, if it doesn't contain over 30 chars, and it is not a saved word */
    if (!isalpha(firstChar) || strlen(label) > 30 || isspace(label[strlen(label) - 1]) ||
        isDataInstructionsName(label) || isActionsName(label) || isRegister(label)) {
        return FALSE;
    }

    /*checking if the label contain only digits and alpha chars */
    for (i = 0; i < strlen(label); i++) {
        if (!isalpha(label[i]) && !isdigit(label[i]))
            return FALSE;
    }

    return TRUE;
}

int isValidNumber(char *number) {
    int validNumberFlag;
    int i;

    if (strlen(number) == 0)
        return FALSE;

    /*checking if the number starts with plus or minus sign and skip it if so */
    if (number[0] == '-' || number[0] == '+') {
        number++;
    }

    validNumberFlag = TRUE;

    /*checking every char in the number to see if it valid, it will also check if the number is not real number */
    for (i = 0; number[i] != '\0' && validNumberFlag; i++) {
        if (!isdigit(number[i])) {
            validNumberFlag = FALSE;
        }
    }

    return validNumberFlag;

}

int validCommas(char *parameters) {

    /* checking if the first or last char in thr parameters is comma*/
    if (parameters[0] == ',' || parameters[strlen(parameters) - 1] == ',') {
        return FALSE;
    }
    while (TRUE) {

        /*check if we finished scanning the parameters*/
        if (parameters[0] == '\0')
            return TRUE;

        /* skip the parameter*/
        while (parameters[0] != ',' && !isspace(parameters[0]) && parameters[0] != '\0')
            parameters++;

        /*check if we finished scanning the parameters*/
        if (parameters[0] == '\0')
            return TRUE;

        /*if not we skip the space between parameters*/
        while (isspace(parameters[0]))
            parameters++;

        /* we expect a comma*/
        if (parameters[0] != ',') {
            return FALSE;
        }
        parameters++;  /*skip the comma*/

        parameters += skipSpacesFromTheStart(parameters);

        /*now we should have the next parameter. if the next char is comma then it is invalid*/
        if (parameters[0] == ',') {
            return FALSE;
        }
    }
}

int countParameters(char *parameters) {
    int i, count;

    /*check if there is no parameters*/
    if (parameters[0] == '\0')
        return 0;

    /*counting how many commas in the parameters and returning +1*/
    count = 0;
    for (i = 0; parameters[i] != '\0'; i++) {
        if (parameters[i] == ',')
            count++;
    }

    return count + 1;
}

int isRegister(char *parameter) {
    if (searchInArray(registers, REGISTERS_NUM, parameter) == -1)
        return FALSE;
    return TRUE;
}

int isActionsName(char *parameter) {

    if (searchInArray(actionsNames, ACTIONS_NAMES_NUM, parameter) == -1)
        return FALSE;
    return TRUE;
}

int isDataInstructionsName(char *parameter) {
    if (searchInArray(dataInstructionsNames, DATA_INSTRUCTIONS_NUM, parameter) == -1)
        return FALSE;
    return TRUE;
}
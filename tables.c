
#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

struct globalVariables globalData;

char *dataInstructionsNames[] = {".data", ".string", ".struct", ".extern", ".entry"};

char *actionsNames[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "get", "prn",
                        "jsr", "rts", "hlt"};
char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};


/* Functions:*/

void initialize() {
    int i;
    globalData.IC = 0;
    globalData.DC = 0;
    globalData.lineNumber = 0;
    globalData.errorFlag = FALSE;
    globalData.symbolTableHead = NULL;
    globalData.externalTableHead = NULL;
    globalData.macroTableHead = NULL;
    globalData.sourceFileName = (char *) malloc(sizeof(char *) * MAX_LINE_LEN + 1);
    checkMemoryAllocation(globalData.sourceFileName);

    /*puts NULL in all pointers before allocate memory*/
    for (i = 0; i <= globalData.pointersCounter; i++)
        globalData.pointers[i] = NULL;
    globalData.pointersCounter = 0;
}

void insertSymbol(struct Data newData) {
    /* allocate node */
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    checkMemoryAllocation(newNode);
    newNode->data = (struct Data *) malloc(sizeof(struct Data));
    checkMemoryAllocation(newNode->data);

    /* put in the data  */
    *(newNode->data) = newData;

    /* make next of new node as head */
    newNode->next = globalData.symbolTableHead;

    /*  move the head to point to the new node */
    globalData.symbolTableHead = newNode;
}

struct Node *searchSymbol(struct Node *head, char *symbol) {
    struct Node *current = head;

    /*search the symbol and return its node. if it doesn't exist - return NULL*/
    while (current != NULL) {
        if (strcmp(current->data->symbol, symbol) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void insertExternal(char *symbol, int value) {
    /*  allocate node */
    struct ExternalNode *newNode = (struct ExternalNode *) malloc(sizeof(struct ExternalNode));
    checkMemoryAllocation(newNode);

    /*  put in the data  */
    newNode->symbol = symbol;
    newNode->value = value;

    /*  Make next of new node as head */
    newNode->next = globalData.externalTableHead;

    /*  move the head to point to the new node */
    globalData.externalTableHead = newNode;
}

void insertMacro(struct MacroData newData) {
    /*  allocate node */
    struct MacroNode *newNode = (struct MacroNode *) malloc(sizeof(struct MacroNode));
    checkMemoryAllocation(newNode);
    newNode->data = (struct MacroData *) malloc(sizeof(struct MacroData));
    checkMemoryAllocation(newNode->data);

    /*  put in the data  */

    *(newNode->data) = newData;

    /*  Make next of new node as head */
    newNode->next = globalData.macroTableHead;

    /*  move the head to point to the new node */
    globalData.macroTableHead = newNode;
}

struct MacroNode *searchMacro(struct MacroNode *head, char *name) {
    struct MacroNode *current = head;

    /*search the macro and return its node. if it doesn't exist - return NULL*/
    while (current != NULL) {
        if (strcmp(current->data->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

int searchInArray(char *arr[], int length, char *str) {
    int i;
    if (str == NULL)
        return -1;

    /*search  string in the given array and return it. if it doesn't exist - return -1*/
    for (i = 0; i < length; i++) {
        if (strcmp(arr[i], str) == 0)
            return i;
    }
    return -1;
}

char *toBasis32(int number) {
    Word w;
    char *basis32String;
    char *base_32[] = {"!", "@", "#", "$", "%", "^", "&", "*", "<", ">", "a", "b", "c", "d", "e", "f", "g", "h", "i",
                       "j",
                       "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v"};
    w.value = number;
    basis32String = (char *) malloc(3);
    checkMemoryAllocation(basis32String);
    globalData.pointers[globalData.pointersCounter++] = basis32String;

    /*convert the word value to the unique basis 32*/
    basis32String[0] = *base_32[getBits(w, 5, 9)];
    basis32String[1] = *base_32[getBits(w, 0, 4)];
    basis32String[2] = '\0';


    return basis32String;
}


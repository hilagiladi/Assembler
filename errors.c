#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"


void printError(char *errormsg) {
    globalData.errorFlag = TRUE; /* turning the flag on in case of error*/
    fprintf(stderr, "\nError in file: %s on line %d: %s\n", globalData.sourceFileName, globalData.lineNumber, errormsg);
}

void checkMemoryAllocation(void *memoryAllocated) {
    if (memoryAllocated == NULL) {
        fprintf(stderr, "Failed to allocate memory! exit the program");
        exit(1); /*exit the program if the memory allocation failed*/
    }
}
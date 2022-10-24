
#include <stdio.h>

/* function declarations by files:*/

/* macro:----------------------------------*/
/*writing the macro */
void preAssembler(FILE *sourceFile);


/*first pass:------------------------------*/
/*assembling the file - first pass*/
void firstPass(FILE *afterMacroFile);


/*second pass:-------------------------------*/
/*assembling the file - second pass*/
void secondPass(FILE *afterMacroFile);


/* errors:-----------------------------------*/
/*prints the error message that has given as parameter + the line number of the error occurred + name of the file
 * and also turning on the error flag*/
void printError(char *errormsg);

/*checking if the memory allocation worked, if not the program will print error and stop*/
void checkMemoryAllocation(void *memoryAllocated);


/*files:--------------------------------------*/
/*opening the files given from the user one by one*/
FILE *openAndAssembleFile(char **argv, int i);

/*creating outputs files*/
FILE *createFile(char *fileEnding, const char *typeOfOpening);

/*writing all needed out put files */
void writeOutputFiles();

/*returns a string after converting to unique basis 32*/
char *toBasis32(int number);







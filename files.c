#include "assembler.h"
#include "constants.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

/*create and return an output file with a specific ending*/
/*writing an object file with ending: ".ob"*/
void writeObjectFile();

/*writing an entry file with ending: ".ent" if needed*/
void writeEntryFile();

/*writing an external file with ending: ".ext" if needed*/
void writeExternalFile();


/*Functions*/

void writeObjectFile() {
    int i;
    FILE *obFile;

    /*create an object file with ending: ".ob"*/
    obFile = createFile(".ob", "w");

    /*writing to the file the headlines: IC and DC with unique basis 32 */
    fprintf(obFile, "%s %s\n", toBasis32(globalData.IC - START_ADDRESS), toBasis32(globalData.DC));

    /* writing to the file the instructionImage*/
    for (i = 0; i < globalData.IC - START_ADDRESS; i++) {
        fprintf(obFile, "%s %s\n", toBasis32(i + START_ADDRESS), toBasis32(globalData.instructionImage[i].value));
    }
    /* writing to the file the dataImage*/
    for (i = 0; i < globalData.DC; i++) {
        fprintf(obFile, "%s %s\n", toBasis32(globalData.IC + i), toBasis32(globalData.dataImage[i].value));
    }
    fclose(obFile);
}

void writeEntryFile() {
    FILE *entFile;
    struct Node *current;
    int hasEntryLabel;

    current = globalData.symbolTableHead;
    hasEntryLabel = FALSE;

    /* check if we do not have any entry symbol*/
    while (current != NULL && !hasEntryLabel) {
        if (current->data->isEntry)
            hasEntryLabel = TRUE;
        current = current->next;
    }

    if (!hasEntryLabel)
        return;  /*do not create the file if we do not have an entry symbol*/

    /*create an entry file with ending: ".ent"*/
    entFile = createFile(".ent", "w");

    current = globalData.symbolTableHead;

    /* writing to the file all the entry symbol in symbolTableHead*/
    while (current != NULL) {
        if (current->data->isEntry)
            fprintf(entFile, "%s %s\n", current->data->symbol, toBasis32(current->data->value));
        current = current->next;
    }
    fclose(entFile);
}

void writeExternalFile() {
    FILE *extFile;
    struct ExternalNode *current;

    current = globalData.externalTableHead;

    /* check if we do not have any extern symbol*/
    if (current == NULL)
        return;

    /*create an extern file with ending: ".ext"*/
    extFile = createFile(".ext", "w");

    /* writing to the file all the extern symbol in externalTableHead*/
    while (current != NULL) {
        fprintf(extFile, "%s %s\n", current->symbol, toBasis32(current->value));
        current = current->next;
    }

    fclose(extFile);
}

void writeOutputFiles() {
    /*write all outputs file*/
    writeObjectFile();
    writeEntryFile();
    writeExternalFile();
}

FILE *createFile(char *fileEnding, const char *typeOfOpening) {
    FILE *outputFile;
    char *outputFileName;
    char *lastFileEnding;

    outputFileName = (char *) malloc(
            (strlen(globalData.sourceFileName) + strlen(fileEnding)) + 1);  /*file name + .ob + 1*/
    checkMemoryAllocation(outputFileName);
    globalData.pointers[globalData.pointersCounter++] = outputFileName;

    lastFileEnding = (char *) malloc((strlen(fileEnding)) + 1);
    checkMemoryAllocation(lastFileEnding);
    globalData.pointers[globalData.pointersCounter++] = lastFileEnding;


    lastFileEnding = strchr(globalData.sourceFileName, '.');/*getting the lastFileEnding */
    globalData.sourceFileName[strlen(globalData.sourceFileName) -
                              strlen(lastFileEnding)] = '\0';/*deleting the last ending file*/
    outputFileName = strcat(globalData.sourceFileName, fileEnding);/*putting the new last ending file*/

    /*checking if opening the file succeeded and stop the program if it isn't*/
    if (!(outputFile = fopen(outputFileName, typeOfOpening))) {
        fprintf(stderr, "\nCannot open file\n");
        exit(1);
    }
    return outputFile;
}

FILE *openAndAssembleFile(char **argv, int i) {
    FILE *sourceFile;

    /*coping the file name from the commend line and adding ".as"*/
    strcpy(globalData.sourceFileName, argv[i]);
    globalData.sourceFileName = strcat(globalData.sourceFileName, ".as");

    /*checking if opening the file succeeded and stop the program if it isn't*/
    if (!(sourceFile = fopen(globalData.sourceFileName, "r"))) {
        fprintf(stderr, "\nCannot open file\nPlease make sure you write the file name in the commend line.\n");
        exit(1);
    }

    return sourceFile;
}

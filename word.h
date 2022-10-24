
#define MAX_DATA_LEN 1000
#define MAX_INSTRUCTION_LEN 1000
#define MAX_POINTERS 10000

/* structs*/

typedef struct {
    unsigned int value: 10;
} Word;

/*a variable that content all the globals changing variable during the program*/
struct globalVariables {
    int IC;
    int DC;
    int lineNumber;
    int errorFlag;
    char *sourceFileName;
    struct Node *symbolTableHead;
    struct ExternalNode *externalTableHead;
    struct MacroNode *macroTableHead;
    Word dataImage[MAX_DATA_LEN];
    Word instructionImage[MAX_INSTRUCTION_LEN];
    char *pointers[MAX_POINTERS];
    int pointersCounter;
};


/* function declarations */

/*  put in bits [start...end] the value*/
void changeBits(Word *w, unsigned int start, unsigned int end, int value);

/* returns the value of the specific bits location */
int getBits(Word w, unsigned int start, unsigned int end);

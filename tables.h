
/* structs and enums: */

typedef enum {
    DATA = 1, CODE, EXTERNAL
} symbol_type;

struct Data {
    char *symbol;
    symbol_type type;
    int isEntry;
    int value;
};

struct Node {
    struct Data *data;
    struct Node *next;
};

struct ExternalNode {
    char *symbol;
    int value;
    struct ExternalNode *next;
};

struct MacroData {
    char *name;
    char *value;
};

struct MacroNode {
    struct MacroData *data;
    struct MacroNode *next;
};

enum actions_names {
    mov = 0, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, get, prn, jsr, rts, hlt
};

enum data_instructions_names {
    dot_data = 0, dot_string, dot_struct, dot_extern, dot_entry
};


/*externals*/

extern struct globalVariables globalData;

/*constants array*/
extern char *dataInstructionsNames[];
extern char *actionsNames[];
extern char *registers[];


/* function declarations */

/*initialize symbol table,macro table, global data struct and pointers array */
void initialize();

/*Insert a new node to the symbolTableHead*/
void insertSymbol(struct Data newData);

/*Insert a new macro node to the macroTableHead*/
void insertMacro(struct MacroData newData);

/*Insert a new node to the externalTableHead*/
void insertExternal(char *symbol, int value);

/*returns a MacroNode based on the name given as parameter
 * if the name given doesn't exist in the macroTableHead, the function returns NULL*/
struct MacroNode *searchMacro(struct MacroNode *head, char *name);

/*returns a Node based on the name given as parameter
 * if the name given doesn't exist in the symbolTableHead, the function returns NULL*/
struct Node *searchSymbol(struct Node *head, char *symbol);

/*returns TRUE or FALSE if the string given as parameter exist in the array given */
int searchInArray(char *arr[], int length, char *str);



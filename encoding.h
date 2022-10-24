

/* function declarations */

/*returns the number of words that needs to be encoding on a specific line  */
int calculateL(int instruction, int sourceAddressing, int destAddressing);

/*encode the first word of the line - in first pass*/
void encodeInstructionFirstWord(int number, char *parameters);

/*encode all the additional words needed in the line - in second pass*/
void encodeAdditionalWords(char *parameters);

/*encode data instruction*/
void encodeData(char *parameters);

/*encode string instruction*/
void encodeString(char *parameters);

/*encode struct instruction*/
void encodeStruct(char *parameters);


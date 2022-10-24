
/* function declarations */

/*Free tables allocated memory*/
void freeAllocatedMemory();

/*returns TRUE or FALSE if the line given as parameter is an empty line or a comment line: ";" */
int ignoreLine(char *line);

/*returns the label in the line if it exists and valid. Otherwise, it returns NULL */
char *getLabel(char *line);

/*returns the instruction in the line */
char *getInstruction(char *line);

/*returns the parameters in the line */
char *getParameters(char *line);

/*removes all the spaces from the string given as parameter*/
void removeSpaces(char *str);

/* return the amount of chars we need to skip that are the spaces at the beginning*/
int skipSpacesFromTheStart(const char *line);

/* returns the last character from the end that contains white cells */
int skipSpacesFromTheEnd(const char *line);

/*returns TRUE or FALSE if the label is valid and prints error if it is not */
int isValidLabel(char *label);

/*returns TRUE or FALSE if the number is valid and prints error if it is not */
int isValidNumber(char *number);

/*returns the number of parameters exists */
int countParameters(char *parameters);

/*returns TRUE or FALSE if the commas between the parameters are written properly */
int validCommas(char *parameters);

/*returns TRUE or FALSE if parameter is a register */
int isRegister(char *parameter);

/*returns TRUE or FALSE if parameter is an instruction Name */
int isDataInstructionsName(char *parameter);

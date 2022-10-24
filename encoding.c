
#include "assembler.h"
#include "constants.h"
#include "encoding.h"
#include "tables.h"
#include "utils.h"
#include "word.h"

/*Declarations*/

/*returns the instruction group number according to the number of parameter related to the action number*/
int instructionGroupNumber(int actionNumber);

/*returns the addressing method number according to the parameter*/
int getAddressingMethod(char *parameter);

/*encode the parameter based on his addressing method*/
void encodeParameter(char *parameter, int addressingMethod, int isSource, int bothRegisters);

/*encode a symbol*/
void encodeSymbol(char *symbol);

/*encode a number*/
void encodeNumber(char *number);

/*encode immediate addressing method - method 0*/
void encodeImmediateAddressing(char *parameter);

/*encode direct addressing method - method 1*/
void encodeDirectAddressing(char *parameter);

/*encode access to record addressing method - method 2*/
void encodeAccessToRecordAddressing(char *parameter);

/*encode direct register addressing method - method 3*/
void encodeDirectRegisterAddressing(char *parameter, int isSource, int bothRegisters);

/*encode the first word of installations that has 2 operands*/
void encodeInstructionFirstWordGroup1(int actionNumber, char *parameters, int numberOfParameters, Word *firstWord);

/*encode the first word of installations that has 1 operands*/
void encodeInstructionFirstWordGroup2(int actionNumber, char *parameters, int numberOfParameters, Word *firstWord);

/*encode the first word of installations that has no operands*/
void encodeInstructionFirstWordGroup3(int numberOfParameters);


/*Functions*/

int instructionGroupNumber(int actionNumber) {
    /* group 1:*/
    /* lea, sub, add, cmp, mov*/
    if (actionNumber == mov || actionNumber == cmp || actionNumber == add || actionNumber == sub ||
        actionNumber == lea) {
        return 1;
    }

    /* group 2:*/
    /* not, clr, inc, dec, jmp, bne, get, prn, jsr*/
    if (actionNumber == not || actionNumber == clr || actionNumber == inc || actionNumber == dec
        || actionNumber == jmp || actionNumber == bne || actionNumber == get || actionNumber == prn ||
        actionNumber == jsr) {
        return 2;
    }

    /* group 3:*/
    /*  rts, hlt*/
    return 3;
}

int getAddressingMethod(char *parameter) {
    if (parameter[0] == '#')/* method number 0*/
        return 0;
    if (isRegister(parameter)) /* method number 3*/
        return 3;
    if (strchr(parameter, '.'))  /*if it has a '.' than it is method 2*/
        return 2;
    if (isValidLabel(parameter))
        return 1;  /*if there is a valid label and has no '.' than ti method 1*/

    return -1; /*if we get here than the parameters don't have a valid method addressing*/
}

int calculateL(int instruction, int sourceAddressing, int destAddressing) {
    int L;
    int group;

    L = 1;
    group = instructionGroupNumber(instruction);
    /* two parameters*/
    if (group == 1) {
        if (sourceAddressing == 0 || sourceAddressing == 1 || sourceAddressing == 3)
            L++;
        if (sourceAddressing == 2)
            L += 2;

        if (destAddressing == 0 || destAddressing == 1 || destAddressing == 3)
            L++;

        if (destAddressing == 2)
            L += 2;

        /*  special case - addressing method 3 twice -  the operands will share one word*/
        if (sourceAddressing == 3 && destAddressing == 3) {
            L--;
        }

    } else if (group == 2) {  /*one parameter*/
        if (destAddressing == 0 || destAddressing == 1 || destAddressing == 3)
            L++;
        if (destAddressing == 2)
            L += 2;
    }
    /*  group 3 has only one word - always!*/
    return L;
}

void encodeInstructionFirstWord(int actionNumber, char *parameters) {
    /* parameters are without spaces and separated with commas*/
    Word firstWord;
    int groupNumber = instructionGroupNumber(actionNumber);
    int numberOfParameters = countParameters(parameters);

    firstWord.value = 0;
    changeBits(&firstWord, 6, 9, actionNumber); /* put the opcode in bits 6-9*/
    /*   group1:*/
    /*   mov, cmp, add, sub, lea*/
    if (groupNumber == 1) {
        encodeInstructionFirstWordGroup1(actionNumber, parameters, numberOfParameters, &firstWord);
    }

    /* group 2 - one operand:*/
    /* not, clr, inc, dec, jmp, bne, get, prn, jsr*/
    if (groupNumber == 2) {
        encodeInstructionFirstWordGroup2(actionNumber, parameters, numberOfParameters, &firstWord);
    }

    /* group 3 - no operand:*/
    /*  rts, hlt*/
    if (groupNumber == 3) {
        encodeInstructionFirstWordGroup3(numberOfParameters);
    }

    /*insert the new encoded word to the instruction image */
    globalData.instructionImage[globalData.IC - START_ADDRESS] = firstWord;

}

void encodeInstructionFirstWordGroup1(int actionNumber, char *parameters, int numberOfParameters, Word *firstWord) {
    char *sourceOperand;
    char *destOperand;
    int destAddressingMethod;
    int sourceAddressingMethod;

    /*checking if tne amount of operands is valid -  should be 2 */
    if (numberOfParameters != 2) {
        printError("invalid number of parameters, should be 2 parameters.");
        return;
    }

    /*  get the source and destination operands*/
    sourceOperand = strtok(parameters, ",");
    destOperand = strtok(NULL, ",");

    /*  get the source and destination operands addressing method*/
    sourceAddressingMethod = getAddressingMethod(sourceOperand);
    destAddressingMethod = getAddressingMethod(destOperand);

    /*checking if the operand is valid*/
    if (sourceAddressingMethod == -1) {
        printError("invalid source operand");
        return;
    }
    if (destAddressingMethod == -1) {
        printError("invalid dest operand");
        return;
    }

    /*checking if the operand valid for the action*/
    if ((actionNumber == mov || actionNumber == add || actionNumber == sub) &&
        destAddressingMethod == 0) {/*it can't have direct addressing method in dest operand*/
        printError("invalid dest operand");
        return;
    }
    if (actionNumber == lea) {
        if (sourceAddressingMethod == 0 || sourceAddressingMethod == 3) {
            printError("invalid source operand");
            return;
        }
        if (destAddressingMethod == 0) {
            printError("invalid dest operand");
            return;
        }
    }

    /*encode addressing method for the source and destination operand*/
    changeBits(firstWord, 4, 5, sourceAddressingMethod);
    changeBits(firstWord, 2, 3, destAddressingMethod);
}

void encodeInstructionFirstWordGroup2(int actionNumber, char *parameters, int numberOfParameters, Word *firstWord) {
    char *destOperand;
    int destAddressingMethod;

    /*checking if tne amount of operands is valid -  should be 1 */
    if (numberOfParameters != 1) {
        printError("invalid number of parameters, should be 1 parameter.");
        return;
    }
    destOperand = parameters;  /*only one parameter*/
    destAddressingMethod = getAddressingMethod(destOperand);

    /*checking if the operand is valid*/
    if (destAddressingMethod == -1) {
        printError("invalid dest operand");
        return;
    }

    /*checking if the operand valid for the action*/
    if (actionNumber != prn && destAddressingMethod == 0) {
        printError("invalid dest operand");
        return;
    }

    /*encode addressing method for the source and destination operand*/
    changeBits(firstWord, 2, 3, destAddressingMethod);
}

void encodeInstructionFirstWordGroup3(int numberOfParameters) {

    /*checking if tne amount of operands is valid -  should be zero */
    if (numberOfParameters != 0) {
        printError("invalid number of parameters, should not be any parameter.");
        return;
    }
}

void encodeAdditionalWords(char *parameters) {
    char *destOperand, *sourceOperand;
    int sourceAddressing, destAddressing, groupNumber;

    /*the opcode is in 6-9 bits of the first word*/
    groupNumber = instructionGroupNumber(getBits(globalData.instructionImage[globalData.IC - START_ADDRESS], 6, 9));
    /*the source addressing method is in 4-5 bits of the first word*/
    sourceAddressing = getBits(globalData.instructionImage[globalData.IC - START_ADDRESS], 4, 5);
    /*the dest addressing method is in 2-3 bits of the first word*/
    destAddressing = getBits(globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 3);

    /*group1:*/
    /* mov, cmp, add, sub, lea*/
    if (groupNumber == 1) {
        /* get the source and destination operands*/
        sourceOperand = strtok(parameters, ",");
        destOperand = strtok(NULL, ",");

        /* encode the source and destination operands*/
        encodeParameter(sourceOperand, sourceAddressing, TRUE, FALSE);

        /* checking if the dest and source operands are registers.
         * if they are, the additional word will be a combination of the two previous ones*/
        if (sourceAddressing == 3 && destAddressing == 3) {
            encodeParameter(destOperand, destAddressing, FALSE, TRUE);
        } else {
            encodeParameter(destOperand, destAddressing, FALSE, FALSE);
        }
    }

    /* group 2 - one operand:*/
    /* not, clr, inc, dec, jmp, bne, get, prn, jsr*/
    if (groupNumber == 2) {
        destOperand = parameters;  /*only one parameter*/
        encodeParameter(destOperand, destAddressing, FALSE, FALSE);
    }
    /* group 3 has no operand to encode*/
    globalData.IC++;  /*skip to the next instruction*/
}

void encodeSymbol(char *symbol) {
    struct Node *node = searchSymbol(globalData.symbolTableHead, symbol);
    globalData.instructionImage[++globalData.IC - START_ADDRESS].value = 0;

    /*checking if the symbol parameter exist in the symbol Table */
    if (node == NULL) {
        printError("operand label is not in defined in the symbol table");
        return;
    }

    /*encode the symbol value*/
    changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 9, node->data->value);

    /*encode A.R.E and check if it's an external symbol. if its, add it external table for the .ext file */
    if (node->data->type == EXTERNAL) {
        changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 0, 1,
                   1);  /* put bits "01"=E in A,R,E */
        insertExternal(node->data->symbol, globalData.IC);
    } else {
        changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 0, 1,
                   2);   /* put bits "10"=R in A,R,E */
    }
}

void encodeParameter(char *parameter, int addressingMethod, int isSource, int bothRegisters) {
    switch (addressingMethod) {
        case 0:  /*the parameter is a # plus a number*/
            encodeImmediateAddressing(parameter);
            break;
        case 1:  /*the parameter is a symbol*/
            encodeDirectAddressing(parameter);
            break;
        case 2:  /*the parameter is a symbol in the pattern: "S1.1"*/
            encodeAccessToRecordAddressing(parameter);
            break;
        case 3:  /*the parameter is a register*/
            encodeDirectRegisterAddressing(parameter, isSource, bothRegisters);
            break;
        default:
            printError("addressing method is not any of 0,1,2,3");
    }

}

void encodeImmediateAddressing(char *parameter) {
    globalData.instructionImage[++globalData.IC - START_ADDRESS].value = 0;
    parameter++;  /*skip the #*/

    /*check if the number is valid and encode it if is*/
    if (!isValidNumber(parameter)) {
        printError("invalid number parameter");
        return;
    }
    changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 9, atoi(parameter));
}

void encodeDirectAddressing(char *parameter) {
    encodeSymbol(parameter);
}

void encodeAccessToRecordAddressing(char *parameter) {
    int number;
    unsigned int parameterLen;

    parameterLen = strlen(parameter);

    /*checking the parameter syntax */
    if ((parameter[parameterLen - 1] != '1' && parameter[parameterLen - 1] != '2')
        || parameter[parameterLen - 2] != '.') {
        printError("invalid syntax operand for addressing method 2.");
        globalData.IC += 2; /*skipping this 2 words*/
        return;
    }

    /*getting the number after the '.' for encoding */
    number = parameter[parameterLen - 1] - '0';
    /*extract the symbol from the parameter*/
    parameter[parameterLen - 2] = '\0';

    /* encode the label*/
    encodeSymbol(parameter);

    /* now we encode the number*/
    globalData.instructionImage[++globalData.IC - START_ADDRESS].value = 0;
    changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 9, number);
}

void encodeDirectRegisterAddressing(char *parameter, int isSource, int bothRegisters) {
    /*If both operands are not registers then each operand will be registered in a separate word,
     * and then we will have to code the second operand in another word.
       Otherwise, they will share a common word. */
    if (!bothRegisters)
        globalData.instructionImage[++globalData.IC - START_ADDRESS].value = 0;
    if (isSource) {  /*source operand*/
        changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 6, 9,
                   searchInArray(registers, REGISTERS_NUM, parameter));
    }
    if (!isSource) {  /*dest operand*/
        changeBits(&globalData.instructionImage[globalData.IC - START_ADDRESS], 2, 5,
                   searchInArray(registers, REGISTERS_NUM, parameter));
    }
}

void encodeData(char *parameters) {
    char *parameter;

    /*checking if the parameters are valid: amount and syntax*/
    if (countParameters(parameters) < 1) {
        printError("invalid number of parameters on .data instruction, should be at least 1 number parameter.");
        return;
    }
    if (!validCommas(parameters)) {
        printError("Invalid parameters syntax (commas)");
        return;
    }

    /*separate each parameter, checking if it valid and encode it*/
    parameter = strtok(parameters, ",");
    while (parameter != NULL) {
        if (!isValidNumber(parameter)) {
            printError("invalid number parameter in .data instruction");
            return;
        }
        encodeNumber(parameter);
        parameter = strtok(NULL, ",");
    }
}

void encodeString(char *parameters) {
    int i;
    unsigned int len = strlen(parameters);

    /*checking if the amount of parameters is valid*/
    if (countParameters(parameters) < 1) {
        printError("invalid number of parameters on .string instruction, should be 1 parameter.");
        return;
    }

    /* remove spaces from the ending:*/
    while (isspace(parameters[len - 1])) {
        parameters[len - 1] = '\0';
        len--;
    }

    /* remove spaces from the start:*/
    while (isspace(parameters[0])) {
        parameters++;
        len--;
    }
    /*checking if the string has " in the start and in the end */
    if (!(strlen(parameters) >= 2 && parameters[0] == '\"' && parameters[len - 1] == '\"')) {
        printError("invalid string parameter - does not start or end with \"");
        return;
    }
    parameters++;  /*remove the first "*/
    parameters[len - 2] = '\0';  /*remove the last "*/


    /*the string parameter is valid - encode every char in the string*/
    for (i = 0; parameters[i] != '\0'; i++) {
        globalData.dataImage[globalData.DC++].value = (unsigned char) parameters[i];  /*encode the current char*/
    }
    globalData.dataImage[globalData.DC++].value = '\0';  /*put the ending char in the data table*/
}

void encodeStruct(char *parameters) {
    char *parameter1;
    char *parameter2;

    /*checking if the parameters are valid: amount and syntax*/
    if (countParameters(parameters) != 2) {
        printError("invalid number of parameters on .struct instruction, should be 2 parameter.");
        return;
    }
    if (!validCommas(parameters)) {
        printError("Invalid parameters syntax (commas)");
        return;
    }

    /*getting the 2 parameters*/
    parameter1 = strtok(parameters, ",");
    parameter2 = strtok(NULL, ",");

    /* encode the number if it's valid*/
    removeSpaces(parameter1);
    if (!isValidNumber(parameter1)) {
        printError("invalid number parameter in .struct instruction");
        return;
    }
    encodeNumber(parameter1);

    /* encode the string*/
    encodeString(parameter2);
}

void encodeNumber(char *number) {
    /*encode the number to the data image */
    globalData.dataImage[globalData.DC++].value = atoi(number);
}
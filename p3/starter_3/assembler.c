/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

struct LabelAddressPair
{
    char label[MAXLINELENGTH];
    int address;
};

struct LabelAddressPair labelAddressPairsStorage[1000];

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int checkreg(int reg);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
int getOpcode(char *opcode);
int inputLabelAddressPair(struct LabelAddressPair *pairs, int numPairs, char *label, int address);
int checkNumAndfindLabelAddress(struct LabelAddressPair *pairs, int numPairs, char *label);

int main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL)
    {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    ////////////////////////////////////////////////////////////////////
    // First pass: read labels and their addresses.
    ////////////////////////////////////////////////////////////////////

    int numLabels = 0;
    int address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        if (strlen(label) > 0)
        {
            if (inputLabelAddressPair(labelAddressPairsStorage, numLabels, label, address) == -1)
            {
                printf("error: duplicate label %s\n", label);
                exit(1);
            }
            numLabels++;
        }
        address++;
        /* code */
    }

    // for (int i = 0; i < numLabels; ++i)
    // {
    //     printf("Label: %s, Address: %d\n", labelAddressPairsStorage[i].label, labelAddressPairsStorage[i].address);
    // }

    ////////////////////////////////////////////////////////////////////
    // Second pass: read instructions and output machine code.
    ////////////////////////////////////////////////////////////////////

    rewind(inFilePtr);
    address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        // line inputed; label, opcode, arg0, arg1, arg2 are set
        int output_word = 0;
        // According to the doc, read the opcode first.
        int opcode_num = getOpcode(opcode);
        if (opcode_num == -1)
        {
            // .fill instruction
            output_word = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg0);
        }
        else
        {
            output_word |= (opcode_num << 22);
            if (opcode_num <= 1)
            {
                // R-type
                int regA = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg0);
                int regB = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg1);
                int destReg = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg2);
                checkreg(regA), checkreg(regB), checkreg(destReg);
                output_word |= (regA << 19);
                output_word |= (regB << 16);
                output_word |= destReg;
            }
            else if (opcode_num <= 4)
            {
                // I-type
                int regA = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg0);
                int regB = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg1);
                checkreg(regA), checkreg(regB);
                output_word |= (regA << 19);
                output_word |= (regB << 16);
                if (opcode_num == 4)
                {
                    int offset = 0;
                    // beq
                    if (isNumber(arg2))
                    {
                        offset = atoi(arg2);
                    }
                    else
                    {
                        int target = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg2);
                        offset = target - (address + 1);
                    }
                    if (offset < -32768 || offset > 32767)
                    {
                        printf("error: offset out of range\n");
                        exit(1);
                    }
                    output_word |= (offset & 0xFFFF);
                }
                else
                {
                    // lw or sw
                    int offset = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg2);
                    if (offset < -32768 || offset > 32767)
                    {
                        printf("error: offset out of range\n");
                        exit(1);
                    }
                    output_word |= (offset & 0xFFFF);
                }
            }
            else
            {
                // J-type or O-type
                if (opcode_num == 5)
                {
                    // J-type
                    int regA = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg0);
                    int regB = checkNumAndfindLabelAddress(labelAddressPairsStorage, numLabels, arg1);
                    checkreg(regA), checkreg(regB);
                    output_word |= (regA << 19);
                    output_word |= (regB << 16);
                }
                else
                {
                    // O-type
                    // no args
                }
            }
        }

        printHexToFile(outFilePtr, output_word);
        address++;
    }

    /* here is an example for how to use readAndParse to read a line from
    inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    // rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    // if (!strcmp(opcode, "add")) {
    //     /* do whatever you need to do for opcode "add" */
    // }

    /* here is an example of using isNumber. "5" is a number, so this will
       return true */
    // if(isNumber("5")) {
    //     printf("It's a number\n");
    // }

    /* here is an example of using printHexToFile. This will print a
       machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);

    return (0);
}

int checkreg(int reg)
{
    if (reg < 0 || reg > 7)
    {
        printf("error: register number out of range\n");
        exit(1);
    }
    return reg;
}

int inputLabelAddressPair(struct LabelAddressPair *pairs, int numPairs, char *label, int address)
{
    // Check for duplicate labels
    for (int i = 0; i < numPairs; ++i)
    {
        if (!strcmp(pairs[i].label, label))
        {
            printf("Duplicate label found: %s\n", label);
            exit(1);
            return -1; // Duplicate label
        }
    }
    strcpy(pairs[numPairs].label, label);
    pairs[numPairs].address = address;
    return 0; // Success
}

int checkNumAndfindLabelAddress(struct LabelAddressPair *pairs, int numPairs, char *label)
{
    if (isNumber(label))
    {
        return atoi(label);
    }

    for (int i = 0; i < numPairs; ++i)
    {
        if (strcmp(pairs[i].label, label) == 0)
        {
            return pairs[i].address;
        }
    }
    printf("Use of undefined label: %s\n", label);
    exit(1);
    return -1; // Not found
}

int getOpcode(char *opcode)
{
    if (!strcmp(opcode, "add"))
    {
        return 0;
    }
    else if (!strcmp(opcode, "nor"))
    {
        return 1;
    }
    else if (!strcmp(opcode, "lw"))
    {
        return 2;
    }
    else if (!strcmp(opcode, "sw"))
    {
        return 3;
    }
    else if (!strcmp(opcode, "beq"))
    {
        return 4;
    }
    else if (!strcmp(opcode, "jalr"))
    {
        return 5;
    }
    else if (!strcmp(opcode, "halt"))
    {
        return 6;
    }
    else if (!strcmp(opcode, "noop"))
    {
        return 7;
    }
    else if (!strcmp(opcode, ".fill"))
    {
        return -1;
    }
    else
    {
        printf("Invalid opcode: %s\n", opcode);
        exit(1);
    }
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line)
{
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for (int line_idx = 0; line_idx < strlen(line); ++line_idx)
    {
        int line_char_is_whitespace = 0;
        for (int whitespace_idx = 0; whitespace_idx < 4; ++whitespace_idx)
        {
            if (line[line_idx] == whitespace[whitespace_idx])
            {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if (!line_char_is_whitespace)
        {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr)
{
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for (int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address)
    {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH - 1)
        {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if (lineIsBlank(line))
        {
            if (!blank_line_encountered)
            {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        }
        else
        {
            if (blank_line_encountered)
            {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}

/*
 * NOTE: The code defined below is not to be modifed as it is implimented correctly.
 */

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
    {
        /* reached end of file */
        return (0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH - 1)
    {
        printf("error: line too long\n");
        exit(1);
    }

    // Ignore blank lines at the end of the file.
    if (lineIsBlank(line))
    {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label))
    {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
           opcode, arg0, arg1, arg2);

    return (1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return ((sscanf(string, "%d%c", &num, &c)) == 1);
}

// Prints a machine code word in the proper hex format to the file
static inline void
printHexToFile(FILE *outFilePtr, int word)
{
    fprintf(outFilePtr, "0x%08X\n", word);
}

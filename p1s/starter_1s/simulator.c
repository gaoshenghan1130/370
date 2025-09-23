/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// DO NOT CHANGE THE FOLLOWING DEFINITIONS

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8        /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct
    stateStruct
{
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
    int numInstructionsExecuted;
} stateType;

void printState(stateType *);

void printStats(stateType *);

static inline int convertNum(int32_t);

enum opcodeType
{
    ADD = 0,
    NOR,
    LW,
    SW,
    BEQ,
    JALR,
    HALT,
    NOOP
};

struct instructionFields
{
    enum opcodeType opcode;
    int regA;
    int regB;
    int destReg;
    int offsetField;
};

struct instructionFields decoder(int instruction)
{
    int opcode = (instruction >> 22) & 0x7; // Extract bits 22-24 for opcode
    int regA = (instruction >> 19) & 0x7;   // Extract bits 19-21 for regA
    int regB = (instruction >> 16) & 0x7;   // Extract bits 16-18 for regB
    int destReg = instruction & 0x7;        // Extract bits 0-2 for destReg
    int offsetField = instruction & 0xFFFF; // Extract bits 0-15 for offsetField

    // printf("Opcode: %d\n", opcode);
    // printf("RegA: %d\n", regA);
    // printf("RegB: %d\n", regB);
    // printf("DestReg: %d\n", destReg);
    // printf("OffsetField: %d\n", offsetField);

    struct instructionFields fields =
        {
            .opcode = opcode,
            .regA = regA,
            .regB = regB,
            .destReg = destReg,
            .offsetField = offsetField};
    return fields;
}

int exec(stateType *statePtr, struct instructionFields fields)
{
    switch (fields.opcode)
    {
    case ADD:
        statePtr->reg[fields.destReg] = statePtr->reg[fields.regA] + statePtr->reg[fields.regB];
        statePtr->pc += 1;
        break;
    case NOR:
        statePtr->reg[fields.destReg] = ~(statePtr->reg[fields.regA] | statePtr->reg[fields.regB]);
        statePtr->pc += 1;
        break;
    case LW:
    {
        int offset = convertNum(fields.offsetField);
        int address = statePtr->reg[fields.regA] + offset;
        if (address < 0 || address >= MEMORYSIZE)
        {
            fprintf(stderr, "Memory access out of bounds\n");
            exit(1);
        }
        statePtr->reg[fields.regB] = statePtr->mem[address];
        statePtr->pc += 1;
    }
    break;
    case SW:
    {
        int offset = convertNum(fields.offsetField);
        int address = statePtr->reg[fields.regA] + offset;

        if (address < 0 || address >= MEMORYSIZE)
        {
            fprintf(stderr, "Memory access out of bounds\n");
            exit(1);
        }
        statePtr->mem[address] = statePtr->reg[fields.regB];
        statePtr->pc += 1;
    }
    break;
    case BEQ:
    {
        int offset = convertNum(fields.offsetField);
        if (statePtr->reg[fields.regA] == statePtr->reg[fields.regB])
        {
            statePtr->pc += 1 + offset;
        }
        else
        {
            statePtr->pc += 1;
        }
    }
    break;
    case JALR:
    {
        int temp = statePtr->pc + 1;
        statePtr->pc = statePtr->reg[fields.regA];
        statePtr->reg[fields.regB] = temp;
    }
    break;
    case HALT:
            statePtr->pc += 1;
            statePtr->numInstructionsExecuted++;
        return 1;
        break;
    case NOOP:
        statePtr->pc += 1;
        break;
    default:
        fprintf(stderr, "Unknown opcode: %d\n", fields.opcode);
        exit(1);
    }
    statePtr->numInstructionsExecuted++;
    return 0;
}

int main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state = {0};
    FILE *filePtr;

    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }
    int loop = 0;
    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++)
    {
        if (state.numMemory >= MEMORYSIZE)
        {
            fprintf(stderr, "exceeded memory size\n");
            exit(2);
        }
        if (sscanf(line, "%x", state.mem + state.numMemory) != 1)
        {
            fprintf(stderr, "error in reading address %d\n", state.numMemory);
            exit(2);
        }
        printf("mem[ %d ] 0x%08X\n", state.numMemory, state.mem[state.numMemory]);
    }

    while (state.pc != state.numMemory)
    {

        loop++;
        if (loop > 1000)
        {
            fprintf(stderr, "infinite loop detected\n");
            exit(1);
        }
        if (state.pc < 0 || state.pc >= MEMORYSIZE)
        {
            fprintf(stderr, "pc out of bounds: %d\n", state.pc);
            exit(1);
        }
        int instruction = state.mem[state.pc];
        struct instructionFields fields = decoder(instruction);
        printState(&state);
        int returnval = exec(&state, fields);
        state.reg[0] = 0; // ensure reg 0 is always 0
        if (returnval == 1) // HALT encountered
        {
            break;
        }
    }
    printStats(&state);
    printState(&state);
    return (0);
}

/*
 * DO NOT MODIFY ANY OF THE CODE BELOW.
 */

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num)
{
    return num - ((num & (1 << 15)) ? 1 << 16 : 0);
}

/*
 * print end of run statistics like in the spec. **This is not required**,
 * but is helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 * DO NOT print "@@@" or "end state" in this function
 */
void printStats(stateType *statePtr)
{
    printf("machine halted\n");
    printf("total of %d instructions executed\n", statePtr->numInstructionsExecuted);
    printf("final state of machine:\n");
}

/*
 * Write any helper functions that you wish down here.
 */

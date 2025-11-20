/*
 * EECS 370, University of Michigan
 * Lab Problem - Ca(t)che the Bugs: Test output for Project 4
 * Instructions are found in the lab doc. See also the P4 Spec.
**/

#include <stdio.h>
#include <stdlib.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

//Use this when calling printAction. Do not modify the enumerated type below.
enum actionType
{
    cacheToProcessor,
    processorToCache,
    memoryToCache,
    cacheToMemory,
    cacheToNowhere
};

/*
 * Log the specifics of each cache action.
 *
 *DO NOT modify the content below.
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type)
{
    printf("$$$ transferring word [%d-%d] ", address, address + size - 1);

    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    }
    else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    }
    else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    }
    else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    }
    else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
    else {
        printf("Error: unrecognized action\n");
        exit(1);
    }
}

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

//You can modify any of main

int main(){
    stateType state;
    state.pc = 4;
    state.numMemory = 7; // = max(size of machine code file, largest writeback address + 1)
    state.mem[0] = 12648454;
    state.mem[1] = 8454167;
    state.mem[2] = 8454174;
    state.mem[3] = 25165824;
    state.mem[4] = state.mem[5] = state.mem[6] = state.mem[7] = 0;
    for(int i = 0; i<NUMREGS; i++) state.reg[i] = 0;
    printAction(0, 4, memoryToCache);
    printAction(0, 1, cacheToProcessor);
    printAction(4, 4, memoryToCache);
    printAction(6, 1, processorToCache);
    printAction(1, 1, cacheToProcessor);
    printAction(4, 4, cacheToMemory);
    printAction(20, 4, memoryToCache);
    printAction(23, 1, cacheToProcessor);
    printAction(2, 1, cacheToProcessor);
    printAction(20, 4, cacheToNowhere);
    printAction(28, 4, memoryToCache);
    printAction(30, 1, cacheToProcessor);
    printAction(3, 1, cacheToProcessor);
    printState(&state);
    printf("$$$ Main memory words accessed: 20\n");
}
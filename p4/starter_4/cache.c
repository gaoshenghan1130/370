/*
 * EECS 370, University of Michigan
 * Project 4: LC-2K Cache Simulator
 * Instructions are found in the project spec.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256

// **Note** this is a preprocessor macro. This is not the same as a function.
// Powers of 2 have exactly one 1 and the rest 0's, and 0 isn't a power of 2.
#define is_power_of_2(val) (val && !(val & (val - 1)))

/*
 * Accesses 1 word of memory.
 * addr is a 16-bit LC2K word address.
 * write_flag is 0 for reads and 1 for writes.
 * write_data is a word, and is only valid if write_flag is 1.
 * If write flag is 1, mem_access does: state.mem[addr] = write_data.
 * The return of mem_access is state.mem[addr].
 */
extern int mem_access(int addr, int write_flag, int write_data);

/*
 * Returns the number of times mem_access has been called.
 */
extern int get_num_mem_accesses(void);

// Use this when calling printAction. Do not modify the enumerated type below.
enum actionType {
  cacheToProcessor,
  processorToCache,
  memoryToCache,
  cacheToMemory,
  cacheToNowhere
};

/* You may add or remove variables from these structs */
typedef struct blockStruct {
  int data[MAX_BLOCK_SIZE];
  int dirty;
  int lruLabel;
  int tag;
  int valid;
} blockStruct;

typedef struct cacheStruct {
  blockStruct blocks[MAX_CACHE_SIZE];
  int blockSize;
  int numSets;
  int blocksPerSet;
  // add any variables for end-of-run stats
} cacheStruct;

/* Global Cache variable */
cacheStruct cache;

void printAction(int, int, enum actionType);
void printCache(void);

/*
 * Set up the cache with given command line parameters. This is
 * called once in main(). You must implement this function.
 */
void cache_init(int blockSize, int numSets, int blocksPerSet) {
  if (blockSize <= 0 || numSets <= 0 || blocksPerSet <= 0) {
    printf("error: input parameters must be positive numbers\n");
    exit(1);
  }
  if (blocksPerSet * numSets > MAX_CACHE_SIZE) {
    printf("error: cache must be no larger than %d blocks\n", MAX_CACHE_SIZE);
    exit(1);
  }
  if (blockSize > MAX_BLOCK_SIZE) {
    printf("error: blocks must be no larger than %d words\n", MAX_BLOCK_SIZE);
    exit(1);
  }
  if (!is_power_of_2(blockSize)) {
    printf("warning: blockSize %d is not a power of 2\n", blockSize);
  }
  if (!is_power_of_2(numSets)) {
    printf("warning: numSets %d is not a power of 2\n", numSets);
  }
  printf("Simulating a cache with %d total lines; each line has %d words\n",
         numSets * blocksPerSet, blockSize);
  printf("Each set in the cache contains %d lines; there are %d sets\n",
         blocksPerSet, numSets);

  /********************* Initialize Cache *********************/

  cache.blockSize = blockSize;
  cache.numSets = numSets;
  cache.blocksPerSet = blocksPerSet;
  for (int i = 0; i < numSets * blocksPerSet; ++i) {
    cache.blocks[i].dirty = 0;
    cache.blocks[i].lruLabel = 0;
    cache.blocks[i].tag = 0;
    cache.blocks[i].valid = 0;
    for (int j = 0; j < blockSize; ++j) {
      cache.blocks[i].data[j] = 0;
    }
  }

  return;
}

/*
 * Access the cache. This is the main part of the project,
 * and should call printAction as is appropriate.
 * It should only call mem_access when absolutely necessary.
 * addr is a 16-bit LC2K word address.
 * write_flag is 0 for reads (fetch/lw) and 1 for writes (sw).
 * write_data is a word, and is only valid if write_flag is 1.
 * The return of mem_access is undefined if write_flag is 1.
 * Thus the return of cache_access is undefined if write_flag is 1.
 */
int cache_access(int addr, int write_flag, int write_data) {
  int blockOffset = addr / cache.blockSize; // block number in address space
  int set = blockOffset % cache.numSets;    // set index
  int tag = blockOffset / cache.numSets;    // tag
  int offset = addr % cache.blockSize;      // word offset inside block

  // 1) Search for hit in the set
  int hitWay = -1;
  for (int i = 0; i < cache.blocksPerSet; ++i) {
    int idx = set * cache.blocksPerSet + i;
    if (cache.blocks[idx].valid && cache.blocks[idx].tag == tag) {
      hitWay = i;
      break;
    }
  }
  
  // If hit
  if (hitWay != -1) {
    int idx = set * cache.blocksPerSet + hitWay;
    if (write_flag == 0) {
      // READ hit
      // update LRU: accessed -> 0, others ++ (only valid ones)
      for (int i = 0; i < cache.blocksPerSet; ++i) {
        int id = set * cache.blocksPerSet + i;
        if (!cache.blocks[id].valid)
          continue;
        if (i == hitWay)
          cache.blocks[id].lruLabel = 0;
        else
          cache.blocks[id].lruLabel++;
      }
      printAction(addr, 1, cacheToProcessor);
      return cache.blocks[idx].data[offset];
    } else {
      // WRITE hit: write into cache, mark dirty
      cache.blocks[idx].data[offset] = write_data;
      cache.blocks[idx].dirty = 1;
      for (int i = 0; i < cache.blocksPerSet; ++i) {
        int id = set * cache.blocksPerSet + i;
        if (!cache.blocks[id].valid)
          continue;
        if (i == hitWay)
          cache.blocks[id].lruLabel = 0;
        else
          cache.blocks[id].lruLabel++;
      }
      printAction(addr, 1, processorToCache);
      return 0; // undefined return for write, return 0 for convenience
    }
  }

  // MISS: need to pick a victim way in the set
  int victimWay = -1;
  // 1) prefer an invalid block
  for (int i = 0; i < cache.blocksPerSet; ++i) {
    int idx = set * cache.blocksPerSet + i;
    if (!cache.blocks[idx].valid) {
      victimWay = i;
      break;
    }
  }
  // 2) if none invalid, pick LRU (largest lruLabel)
  if (victimWay == -1) {
    int maxLRU = -1;
    for (int i = 0; i < cache.blocksPerSet; ++i) {
      int idx = set * cache.blocksPerSet + i;
      if (!cache.blocks[idx].valid)
        continue; // defensive
      if (cache.blocks[idx].lruLabel > maxLRU) {
        maxLRU = cache.blocks[idx].lruLabel;
        victimWay = i;
      }
    }
    // victimWay must be set now (there is at least one valid block if no
    // invalid found)
  }

  int victimIdx = set * cache.blocksPerSet + victimWay;
   int evictTag = cache.blocks[victimIdx].tag;
  int evictAddr = (evictTag * cache.numSets + set) * cache.blockSize;

  // If victim is valid and dirty -> write back whole block to memory
  if (cache.blocks[victimIdx].valid && cache.blocks[victimIdx].dirty) {
   
    printAction(evictAddr, cache.blockSize, cacheToMemory);
    for (int j = 0; j < cache.blockSize; ++j) {
      mem_access(evictAddr + j, 1, cache.blocks[victimIdx].data[j]);
    }
  } else if (cache.blocks[victimIdx].valid && !cache.blocks[victimIdx].dirty) {
    // valid but not dirty -> evict to nowhere
    printAction(evictAddr, cache.blockSize, cacheToNowhere);
  }

  // Load the whole block from memory (block-aligned address)
  int blockStartAddr = addr - offset; // or (tag * numSets + set) * blockSize
  printAction(blockStartAddr, cache.blockSize, memoryToCache);
  for (int j = 0; j < cache.blockSize; ++j) {
    cache.blocks[victimIdx].data[j] = mem_access(blockStartAddr + j, 0, 0);
  }

  // Set metadata for the loaded block
  cache.blocks[victimIdx].tag = tag;
  cache.blocks[victimIdx].valid = 1;
  cache.blocks[victimIdx].dirty = 0; // not dirty immediately after load

  // Now handle read-miss vs write-miss
  if (write_flag == 0) {
    // READ miss: return the requested word
    // Update LRU: victim becomes MRU (0), others ++
    for (int i = 0; i < cache.blocksPerSet; ++i) {
      int id = set * cache.blocksPerSet + i;
      if (!cache.blocks[id].valid)
        continue;
      if (i == victimWay)
        cache.blocks[id].lruLabel = 0;
      else
        cache.blocks[id].lruLabel++;
    }
    printAction(addr, 1, cacheToProcessor);
    return cache.blocks[victimIdx].data[offset];
  } else {
    // WRITE miss: write-allocate policy
    cache.blocks[victimIdx].data[offset] = write_data;
    cache.blocks[victimIdx].dirty = 1;

    // Update LRU
    for (int i = 0; i < cache.blocksPerSet; ++i) {
      int id = set * cache.blocksPerSet + i;
      if (!cache.blocks[id].valid)
        continue;
      if (i == victimWay)
        cache.blocks[id].lruLabel = 0;
      else
        cache.blocks[id].lruLabel++;
    }
    printAction(addr, 1, processorToCache);
    return 0;
  }
}
/*
 * print end of run statistics like in the spec. **This is not required**,
 * but is very helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 * DO NOT print $$$ in this function
 */
void printStats(void) {
  printf("End of run statistics:\n");
  return;
}

/*
 * Log the specifics of each cache action.
 *
 *DO NOT modify the content below.
 * address is the starting word address of the range of data being
 *transferred. size is the size of the range of data being transferred. type
 *specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type) {
  printf("$$$ transferring word [%d-%d] ", address, address + size - 1);

  if (type == cacheToProcessor) {
    printf("from the cache to the processor\n");
  } else if (type == processorToCache) {
    printf("from the processor to the cache\n");
  } else if (type == memoryToCache) {
    printf("from the memory to the cache\n");
  } else if (type == cacheToMemory) {
    printf("from the cache to the memory\n");
  } else if (type == cacheToNowhere) {
    printf("from the cache to nowhere\n");
  } else {
    printf("Error: unrecognized action\n");
    exit(1);
  }
}

/*
 * Prints the cache based on the configurations of the struct
 * This is for debugging only and is not graded, so you may
 * modify it, but that is not recommended.
 */
void printCache(void) {
  int blockIdx;
  int decimalDigitsForWaysInSet =
      (cache.blocksPerSet == 1) ? 1
                                : (int)ceil(log10((double)cache.blocksPerSet));
  printf("\ncache:\n");
  for (int set = 0; set < cache.numSets; ++set) {
    printf("\tset %i:\n", set);
    for (int block = 0; block < cache.blocksPerSet; ++block) {
      blockIdx = set * cache.blocksPerSet + block;
      if (cache.blocks[set * cache.blocksPerSet + block].valid) {
        printf("\t\t[ %0*i ] : ( V:T | D:%c | LRU:%-*i | T:%i )\n\t\t%*s{",
               decimalDigitsForWaysInSet, block,
               (cache.blocks[blockIdx].dirty) ? 'T' : 'F',
               decimalDigitsForWaysInSet, cache.blocks[blockIdx].lruLabel,
               cache.blocks[blockIdx].tag, 7 + decimalDigitsForWaysInSet, "");
        for (int index = 0; index < cache.blockSize; ++index) {
          printf(" 0x%08X", cache.blocks[blockIdx].data[index]);
        }
        printf(" }\n");
      } else {
        printf("\t\t[ %0*i ] : ( V:F )\n\t\t%*s{     }\n",
               decimalDigitsForWaysInSet, block, 7 + decimalDigitsForWaysInSet,
               "");
      }
    }
  }
  printf("end cache\n");
}

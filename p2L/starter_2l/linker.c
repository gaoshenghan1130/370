/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

static inline void printHexToFile(FILE *, int);
static FILE *processInputFiles(int argc, char *argv[], char *inFileStrs[], int numFiles, FILE *outFilePtr, FILE *inFilePtr);

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry
{
	char label[7];
	char location;
	unsigned int offset; // absolute line number in final executable
};

struct RelocationTableEntry
{
	unsigned int file;
	unsigned int offset; // line number in text or data section
	char inst[6];
	char label[7];
};

struct FileData
{
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	unsigned int textStartingLine; // in final executable
	unsigned int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles
{
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	int text[MAXSIZE * MAXFILES];
	int data[MAXSIZE * MAXFILES];
	SymbolTableEntry symbolTable[MAXSIZE * MAXFILES];
	RelocationTableEntry relocTable[MAXSIZE * MAXFILES];
	int currentDataLine;
};

void initFileData(CombinedFiles *file, FileData files[], int numFiles);
void loadSymbolandRelocations(CombinedFiles *file, FileData files[], int numFiles);

void processRelocations(CombinedFiles *file);

void printOutputFile(CombinedFiles *file, FILE *outFilePtr);

char *inFileStr, *outFileStr;
FILE *inFilePtr, *outFilePtr;
unsigned int i, j;
FileData files[MAXFILES];

int main(int argc, char *argv[])
{
	outFilePtr = processInputFiles(argc, argv, NULL, 0, outFilePtr, inFilePtr);
	CombinedFiles combinedFile;
	initFileData(&combinedFile, files, argc - 2);

	processRelocations(&combinedFile);

	printOutputFile(&combinedFile, outFilePtr);

	fclose(outFilePtr);
	return 0;
} // main

static FILE *
processInputFiles(int argc, char *argv[], char *inFileStrs[], int numFiles, FILE *outFilePtr, FILE *inFilePtr)
{
	if (argc <= 2 || argc > 8)
	{
		printf("error: usage: %s <MAIN-object-file> ... <object-file> ... <output-exe-file>, with at most 5 object files\n",
			   argv[0]);
		exit(1);
	}

	outFileStr = argv[argc - 1];

	outFilePtr = fopen(outFileStr, "w");
	if (outFilePtr == NULL)
	{
		printf("error in opening %s\n", outFileStr);
		exit(2);
	}

	// read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; ++i)
	{
		inFileStr = argv[i + 1];

		inFilePtr = fopen(inFileStr, "r");

		if (inFilePtr == NULL)
		{
			printf("error in opening %s\n", inFileStr);
			exit(3);
		}

		char line[MAXLINELENGTH];
		unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
			   &textSize, &dataSize, &symbolTableSize, &relocationTableSize);

		files[i].textSize = textSize;
		files[i].dataSize = dataSize;
		files[i].symbolTableSize = symbolTableSize;
		files[i].relocationTableSize = relocationTableSize;

		// read in text section
		int instr;
		for (j = 0; j < textSize; ++j)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = strtol(line, NULL, 0);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < dataSize; ++j)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = strtol(line, NULL, 0);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		unsigned int addr;
		for (j = 0; j < symbolTableSize; ++j)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
				   label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < relocationTableSize; ++j)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
				   &addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file = i;
		}
		fclose(inFilePtr);
	}

	return outFilePtr;
}

void initFileData(CombinedFiles *file, FileData files[], int numFiles)
{
	file->textSize = 0;
	file->dataSize = 0;
	file->symbolTableSize = 0;
	file->relocationTableSize = 0;
	for (i = 0; i < numFiles; ++i)
	{
		file->textSize += files[i].textSize;
		file->dataSize += files[i].dataSize;
		// file->symbolTableSize += files[i].symbolTableSize;  this is not correct, as some symbols may be undefined
		// file->relocationTableSize += files[i].relocationTableSize;
	}
	for (i = 0; i < numFiles; ++i)
	{
		files[i].textStartingLine = (i == 0) ? 0 : files[i - 1].textStartingLine + files[i - 1].textSize;
		files[i].dataStartingLine = (i == 0) ? file->textSize : files[i - 1].dataStartingLine + files[i - 1].dataSize;
	}
	loadSymbolandRelocations(file, files, numFiles);
}

void loadSymbolandRelocations(CombinedFiles *file, FileData files[], int numFiles)
{
	// load file contents into combined file
	for (i = 0; i < numFiles; ++i)
	{
		// copy text section
		for (j = 0; j < files[i].textSize; ++j)
		{
			file->text[files[i].textStartingLine + j] = files[i].text[j];
		}
		// copy data section
		for (j = 0; j < files[i].dataSize; ++j)
		{
			file->data[files[i].dataStartingLine + j - file->textSize] = files[i].data[j];
		}
	}
	for (i = 0; i < numFiles; ++i)
	{
		// load symbol table
		for (j = 0; j < files[i].symbolTableSize; ++j)
		{
			// check if symbol already exists
			if (strcmp(files[i].symbolTable[j].label, "Stack") == 0)
			{
				exit(4); // Stack symbol is reserved
			}
			int exists = 0;
			for (unsigned int k = 0; k < file->symbolTableSize; ++k)
			{
				if (strcmp(file->symbolTable[k].label, files[i].symbolTable[j].label) == 0)
				{
					exists = 1;
					// check for multiple definitions
					if (file->symbolTable[k].location != 'U' && files[i].symbolTable[j].location != 'U')
					{
						printf("Error: multiple definitions of symbol %s\n", files[i].symbolTable[j].label);
						exit(5);
					}
					// if defined in new file, update location and offset
					if (file->symbolTable[k].location == 'U' && files[i].symbolTable[j].location != 'U')
					{
						file->symbolTable[k].location = files[i].symbolTable[j].location;
						if (files[i].symbolTable[j].location == 'T')
						{
							file->symbolTable[k].offset = files[i].symbolTable[j].offset + files[i].textStartingLine;
						}
						else if (files[i].symbolTable[j].location == 'D')
						{
							file->symbolTable[k].offset = files[i].symbolTable[j].offset + files[i].dataStartingLine;
						}
					}
				}
			}
			// if not exists, add to combined symbol table
			if (!exists)
			{
				strcpy(file->symbolTable[file->symbolTableSize].label, files[i].symbolTable[j].label);
				file->symbolTable[file->symbolTableSize].location = files[i].symbolTable[j].location;
				file->symbolTable[file->symbolTableSize].offset = files[i].symbolTable[j].offset +
																  (files[i].symbolTable[j].location == 'T' ? files[i].textStartingLine : 0) +
																  (files[i].symbolTable[j].location == 'D' ? files[i].dataStartingLine : 0);
				file->symbolTableSize++;
			}
		}
		// load relocation table
		for (j = 0; j < files[i].relocationTableSize; ++j)
		{
			file->relocTable[file->relocationTableSize] = files[i].relocTable[j];
			file->relocationTableSize++;
		}
	}
	// debug print
	for (i = 0; i < file->symbolTableSize; ++i)
	{
		if (file->symbolTable[i].location == 'U' && strcmp(file->symbolTable[i].label, "Stack") != 0)
		{
			printf("Error: undefined symbol %s\n", file->symbolTable[i].label);
			exit(6);
		}
	}
}

void processRelocations(CombinedFiles *file)
{
	for (i = 0; i < file->relocationTableSize; ++i)
	{
		RelocationTableEntry *reloc = &file->relocTable[i];
		int symbolOffset = -1;
		// find symbol in symbol table, most likely a global symbol
		for (j = 0; j < file->symbolTableSize; ++j)
		{
			if (strcmp(file->symbolTable[j].label, reloc->label) == 0)
			{
				symbolOffset = file->symbolTable[j].offset; // adjust for data section
				break;
			}
		}

		if (symbolOffset == -1) // the symbol is local
		{
			if (isupper(reloc->label[0]))
			{
				printf("Error: undefined symbol %s\n", reloc->label);
				exit(7);
			}

			if (reloc->inst[0] == '.') // .fill
			{
				symbolOffset = files[reloc->file].dataStartingLine - files[reloc->file].textStartingLine + files[reloc->file].dataSize; // relative to data section so subtract textSize
				printf("Symbol %s is local, offset %d\n", reloc->label, symbolOffset);
			}
			else // instruction
			{
				symbolOffset = files[reloc->file].dataStartingLine - files[reloc->file].textSize; // relative to data section so subtract textSize
				printf("Symbol %s is local, offset %d\n", reloc->label, symbolOffset);
			}
		}

		// replace instruction at offset
		int instLine = 0;
		if (strcmp(reloc->inst, "jalr") == 0 || strcmp(reloc->inst, "halt") == 0 || strcmp(reloc->inst, "noop") == 0)
		{
			printf("Error: cannot relocate instruction %s\n", reloc->inst);
			exit(8);
		}
		else if (strcmp(reloc->inst, "beq") == 0)
		{
			instLine = reloc->offset + files[reloc->file].textStartingLine;
			int originalInst = file->text[instLine];
			int opcode = (originalInst >> 22) & 0x3FF;
			int regA = (originalInst >> 19) & 0x7;
			int regB = (originalInst >> 16) & 0x7;
			int offsetField = originalInst & 0xFFFF;
			if (offsetField & 0x8000) // sign extend
			{
				offsetField |= 0xFFFF0000;
			}
			if (isupper(reloc->inst[0]))
			{
				printf("Error: cannot relocate instruction %s\n", reloc->inst);
				exit(9);
			}
			int newOffset = symbolOffset + offsetField;
			if (newOffset < -32768 || newOffset > 32767)
			{
				printf("Error: offset out of range for instruction %s at line %d\n", reloc->inst, instLine);
				exit(10);
			}
			int newInst = (opcode << 22) | (regA << 19) | (regB << 16) | (newOffset & 0xFFFF);
			file->text[instLine] = newInst;
		}
		else if (strcmp(reloc->inst, "lw") == 0 || strcmp(reloc->inst, "sw") == 0)
		{
			instLine = reloc->offset + files[reloc->file].textStartingLine;
			int originalInst = file->text[instLine];
			int opcode = (originalInst >> 22) & 0x3FF;
			int regA = (originalInst >> 19) & 0x7;
			int regB = (originalInst >> 16) & 0x7;
			int offsetField = originalInst & 0xFFFF;
			int newOffset = 0;
			if (offsetField & 0x8000) // sign extend
			{
				offsetField |= 0xFFFF0000;
			}

			newOffset = symbolOffset + offsetField;
			printf("With symbolOffset %d and original offset %d\n", symbolOffset, offsetField);
			printf("Relocating lw/sw at line %d to new offset %d\n", instLine, newOffset);

			if (newOffset < -32768 || newOffset > 32767)
			{
				printf("Error: offset out of range for instruction %s at line %d\n", reloc->inst, instLine);
				exit(11);
			}
			int newInst = (opcode << 22) | (regA << 19) | (regB << 16) | (newOffset & 0xFFFF);
			file->text[instLine] = newInst;
		}
		else if (strcmp(reloc->inst, ".fill") == 0)
		{
			int originalData = file->data[reloc->offset + files[reloc->file].dataStartingLine - file->textSize]; // relative to data section so subtract textSize
			instLine = reloc->offset + files[reloc->file].dataStartingLine - file->textSize; // relative to data section so subtract textSize
			int newData = symbolOffset + originalData;
			file->data[instLine] = newData;
			printf("Data at line %d is now %d\n", instLine, file->data[instLine]);
		}
		else
		{
			printf("Error: unknown instruction %s\n", reloc->inst);
			exit(12);
		}
	}
}

static inline void
printHexToFile(FILE *outFilePtr, int word)
{
	fprintf(outFilePtr, "0x%08X\n", word);
}

void printOutputFile(CombinedFiles *file, FILE *outFilePtr)
{
	// Print text section
	for (i = 0; i < file->textSize; ++i)
	{
		printHexToFile(outFilePtr, file->text[i]);
	}

	// Print data section
	for (i = 0; i < file->dataSize; ++i)
	{
		printHexToFile(outFilePtr, file->data[i]);
	}
}
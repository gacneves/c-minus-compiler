#ifndef _BINARY_H_
#define _BINARY_H_

#include "assembly.h"

typedef struct{
    int opCode, rd, rs, rt, IMM;
    Type type;
} BinInstruction;

typedef struct BinaryListRec{
    BinInstruction bin;
    struct BinaryListRec * next; 
} * BinaryList;

void binaryGen(InstructionList instList, char * setDst, char * path);

/* Hard Disk organization */
#define PROCESSESS_TABLE_TRACK 0
#define PROCESSESS_TABLE_SECTOR 0
#define OPERATING_SYSTEM_TRACK 0
#define OPERATING_SYSTEM_SECTOR 47
#define MAX_OPERATING_SYSTEM_SIZE 2000
#define SECTOR_SIZE 2048
#define NUMBER_OF_REGS_IN_CONTEXT_EXCHANGE 18
int nextAvailableTrack;

/* For compiling programs */
#define MAX_NO_PROGRAMS 15

typedef struct ProcInfo{
    int id, size, location;
} ProcInfo;

int program_size_count;
int proc_no;
ProcInfo * proc_info;

/** For creating the process table
 * 
 * Process table organization: Located at the beginning of the HD (first track), the i represents the process in the table ([0:14])
 * Sector 0 = Number of processess in the hard disk
 * Sector i + 1 = Process ID
 * Sector i + 2 = Process size (quantity of instructions)
 * Sector i + 3 = Process location in hard disk
 * Sector 46 = Next available track to allocate a process
 * Sector 47 ~ 2047 = Operating system
*/
void generateProcessTable(char * path);

#endif
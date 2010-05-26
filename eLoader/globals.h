#ifndef GLOBALS_H
#define GLOBALS_H

/*
Globals do not get allocated correctly in the HBL environment.
for this reason, we create a "fake" structure holding all of them
in a safe memory zone. If you need globals, it is better to have them here
*/

#define GLOBALS_ADDR 0x11000 //This is in the scratchpad!!!
#include "sdk.h"
#include "malloc.h"
#include "modmgr.h"
#include "tables.h"

typedef struct 
{
    //uids
    SceUID hbl_block_uid;
    u32 hbl_block_addr;
    //hook.c
    char* module_chdir; //cwd of the currently running module
    int pllfreq; // currentpll frequency
    int cpufreq; //current cpu frequency
    int busfreq; //current bus frequency
    int chdir_ok; //1 if sceIoChdir is correctly estimated, 0 otherwise
    //settings.c
    int override_sceIoMkdir;
    char *  hb_folder;
    //malloc.c
    u32 nblocks; //Number of allocated blocks
    HBLMemBlock block[MAX_ALLOCS]; /* Blocks */
    //modmgr.c
    HBLModTable mod_table;
    //tables.c
    HBLNIDTable nid_table;
    HBLLibTable library_table;
    
} tGlobals;

//gets a pointer to the global variables
tGlobals * get_globals();

//inits global variables. This needs to be called once and only once, preferably at the start of the HBL
void init_globals();

#endif


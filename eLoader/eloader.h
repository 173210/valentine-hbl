#ifndef ELOADER
#define ELOADER

#include "sdk.h"

// Max libraries to consider
#define MAX_LIBRARIES 40

// Max exported functions per library
# define MAX_LIBRARY_EXPORTS 155

// Size of NID-to-call table
#define NID_TABLE_SIZE 600

// Invalid/unknown NID
#define INVALID_NID 0xFFFFFFFF

// Maximum attempts to reestimate a syscall
#define MAX_REESTIMATE_ATTEMPTS 5

// HBL stubs address
#define HBL_STUBS_START 0x10000

// Fixed loading address for relocatables
#define PRX_LOAD_ADDRESS 0x08900000

//
//Files locations
//
#define HBL_ROOT "ms0:/hbl/"
#define HBL_BIN "hbl.bin"
// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT"GAME/EBOOT.PBP"
#define ELF_PATH HBL_ROOT"GAME/eboot.elf"
#define HBL_PATH HBL_ROOT HBL_BIN
#define KDUMP_PATH "ef0:/kmem.dump" // Always dump to the internal flash (fixes issue 43)
#define HBL_CONFIG "hbl_config.txt"

// Path for NID libraries
#define LIB_PATH HBL_ROOT"libs"
#define LIB_EXTENSION ".nids"

//
// Switches
//

//Comment the following line to avoid reestimation
#define REESTIMATE_SYSCALL

//Comment to disable HBL LoadModule system
#define LOAD_MODULE
 
//Comment to disable the function Subinterrupthandler Cleanup
#define SUB_INTR_HANDLER_CLEANUP 
 
 
extern u32 gp;
extern u32* entry_point;

// Should receive a file path (plain ELFs or EBOOT.PBP)
void run_eboot(const char *path, int is_eboot);

#endif

#ifndef ELOADER
#define ELOADER

#include "sdk.h"

// MIPS opcodes
#define JR_RA_OPCODE 0x03E00008
#define NOP_OPCODE 0x00000000

// GAME addresses (should move to external configuration files)
#define MEMORY_PARTITION_POINTER 0x08B3C11C

// Fixed loading address for relocatables
#define PRX_LOAD_ADDRESS 0x08900000

// To distinguish between SYSCALL and JUMP stubs
#define SYSCALL_MASK_IMPORT 0x01000000
#define SYSCALL_MASK_RESOLVE 0xFFF00000

// Macros to construct call and jump instructions
#define MAKE_CALL(f) (0x0c000000 | (((u32)(f) >> 2)  & 0x03ffffff))
#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff))

// Macros to deal with $gp register
#define GET_GP(gp) asm volatile ("move %0, $gp\n" : "=r" (gp))
#define SET_GP(gp) asm volatile ("move $gp, %0\n" :: "r" (gp))

// Macro to get the syscall number
#define GET_SYSCALL_NUMBER(sys) ((u32)(sys) >> 6)
// Macro to form syscal instruction
#define MAKE_SYSCALL(n) (0x03ffffff & (((u32)(n) << 6) | 0x0000000c))

// Max chars on library name
#define MAX_LIBRARY_NAME_LENGTH 30

// Max libraries to consider
#define MAX_LIBRARIES 40

// Max exported functions per library
# define MAX_LIBRARY_EXPORTS 155

// Size of NID-to-call table
#define NID_TABLE_SIZE 0x200

// Maximum attempts to reestimate a syscall
#define MAX_REESTIMATE_ATTEMPTS 15

//
//Files locations
//
#define HBL_ROOT "ms0:/hbl/"

// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT"GAME/EBOOT.PBP"
#define ELF_PATH HBL_ROOT"GAME/eboot.elf"
#define MENU_PATH HBL_ROOT"menu.bin" // menu
#define HBL_PATH HBL_ROOT"hbl.bin"
#define KDUMP_PATH HBL_ROOT"kmem.dump"

// Path for NID libraries
#define LIB_PATH HBL_ROOT"libs"
#define LIB_EXTENSION ".nids"

//
// Switches
//

//Comment the following line if you don't want to hook thread creation
//#define FAKE_THREADS

//comment the following line if you don't want to return to the menu when leaving a game (reported to crash)
//#define RETURN_TO_MENU_ON_EXIT

//Comment the following line to avoid unloading Labo
#define UNLOAD_MODULE

//Comment the following line to avoid reestimation
#define REESTIMATE_SYSCALL

//Choose free memory method (only one :P)
//#define AB5000_FREEMEM
#define DAVEE_FREEMEM

//Comment to disable HBL LoadModule system
#define LOAD_MODULE

extern u32 gp;
extern u32* entry_point;
//extern u32 hbsize;

void runThread(SceSize args, void *argp);

// Should receive a file path (plain ELFs or EBOOT.PBP)
void start_eloader(const char *path, int is_eboot);


#endif

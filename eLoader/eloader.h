#ifndef ELOADER
#define ELOADER

#include "sdk.h"

// MIPS opcodes
#define JR_RA_OPCODE 0x03E00008
#define NOP_OPCODE 0x00000000

// GAME addresses (should move to external configuration files)
//#define MEMORY_PARTITION_POINTER 0x08B3C11C

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

// Invalid/unknown NID
#define INVALID_NID 0xFFFFFFFF

// Maximum attempts to reestimate a syscall
#define MAX_REESTIMATE_ATTEMPTS 5

// HBL stubs address
#define HBL_STUBS_START 0x10000

//
//Files locations
//
#define HBL_ROOT "ms0:/hbl/"
#define HBL_BIN "hbl.bin"
// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT"GAME/EBOOT.PBP"
#define ELF_PATH HBL_ROOT"GAME/eboot.elf"
#define MENU_PATH HBL_ROOT"menu.bin" // menu
#define HBL_PATH HBL_ROOT HBL_BIN
#define KDUMP_PATH HBL_ROOT"kmem.dump"
#define HBL_CONFIG "hbl_config.txt"

// Path for NID libraries
#define LIB_PATH HBL_ROOT"libs"
#define LIB_EXTENSION ".nids"

//
// Switches
//


//Comment the following line if you don't want to hook thread creation
#define HOOK_THREADS

//comment the following line if you don't want to return to the menu when leaving a game (reported to crash)
//#define RETURN_TO_MENU_ON_EXIT

//Comment the following line to avoid reestimation
#define REESTIMATE_SYSCALL

//Comment to disable HBL LoadModule system
#define LOAD_MODULE

/*
 * Avoiding syscall estimations
 * The following override some major functions to avoid syscall estimations
 * Currently (until we can get 100% syscall estimation working ?) this increases
 * compatibility a LOT, but this alsow slows down some specific homebrews
 * and increases the size of HBL.
 * In the future if we can't achieve perfect syscall estimates, 
 * we will want to move these settings in a settings file, on a per homebrew basis
 * (see Noobz eLoader cfg file for reference on this)
 */

//this one might make emulators slow as it maps peekbufferpositive to readbufferpositive
//Comment the following line to avoid overriding sceCtrlPeekBufferPositive
#define HOOK_PEEKBUFFERPOSITIVE

//Comment the following line to avoid overriding sceAudio*
#define HOOK_AUDIOFUNCTIONS

//Comment the following line to avoid overriding scePower*
#define HOOK_POWERFUNCTIONS

//Comment the following line to avoid overriding sceIo* (happens only if sceIoChdir fails)
#define HOOK_CHDIR_AND_FRIENDS
 
extern u32 gp;
extern u32* entry_point;

// Should receive a file path (plain ELFs or EBOOT.PBP)
void start_eloader(const char *path, int is_eboot);


#endif

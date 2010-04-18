#ifndef ELOADER
#define ELOADER

#include "sdk.h"
#include "scratchpad.h"

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

// Macro to get the syscall number
#define GET_SYSCALL_NUMBER(sys) ((u32)(sys) >> 6)
// Macro to form syscal instruction
#define MAKE_SYSCALL(n) (0x03ffffff & (((u32)(n) << 6) | 0x0000000c))

// Size of NID-to-call table
#define NID_TABLE_SIZE 0x200

// Maximum attempts to reestimate a syscall
#define MAX_REESTIMATE_ATTEMPTS 10

// Struct holding all NIDs imported by the game and their respective jump/syscalls
typedef struct
{
	u32 nid;	// NID
	u32 call;	// Syscall/jump associated to the NID
} tNIDResolver;

// Max chars on library name
#define MAX_LIBRARY_NAME_LENGTH 30

// Max libraries for HBL
#define MAX_LIBRARIES 40

// Max exported functions per library
# define MAX_LIBRARY_EXPORTS 155

typedef enum
{
	SYSCALL_MODE = 0, 
	JUMP_MODE = 1
} tCallingMode;

// Struct holding info to help syscall estimation
// This struct is for each library imported by the game
typedef struct
{
	char library_name[MAX_LIBRARY_NAME_LENGTH];	// Library name
	tCallingMode calling_mode;					// Defines how library exports are called
	unsigned int num_library_exports;			// Number of exported functions in library
	unsigned int num_known_exports;				// Number of known exported functions (exports we know the syscall of)
	u32 lowest_syscall;							// Lowest syscall number found
	u32 lowest_nid;								// NID associated to lowest syscall
	unsigned int lowest_index;					// Lowest NID index in .nids file
} tSceLibrary;

//
//Files locations
//
#define HBL_ROOT "ms0:/hbl/"

// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT"GAME/EBOOT.PBP"
#define ELF_PATH HBL_ROOT"GAME/eboot.elf"
#define MENU_PATH HBL_ROOT"menu.bin" // menu
#define HBL_PATH HBL_ROOT"hbl.bin"

// Path for NID libraries
#define LIB_PATH HBL_ROOT"libs"
#define LIB_EXTENSION ".nids"

//
// Switches
//

//Comment the following line if you don't want wololo's crappy Fake Ram mechanism
#define FAKEMEM 1

//comment the following line if you don't want to return to the menu when leaving a game (reported to crash)
//#define RETURN_TO_MENU_ON_EXIT

//Comment the following line to avoid unloading Labo
//#define UNLOAD_MODULE

//Comment the following line to avoid reestimation
#define REESTIMATE_SYSCALL




// Auxiliary structure to help with syscall estimation
extern tSceLibrary library_table[MAX_LIBRARIES];

// Returns a pointer to the library descriptor
tSceLibrary* get_library_entry(const char* library_name);

// Should receive a file path (plain ELFs or EBOOT.PBP)
void start_eloader(char *eboot_path, int is_eboot);


#endif

#ifndef ELOADER_TABLES
#define ELOADER_TABLES

#include <hbl/mod/elf.h>

// Struct holding all NIDs imported by the game and their respective jump/syscalls
typedef struct
{
	int nid;	// NID
	int call;	// Syscall/jump associated to the NID
	int lib;	// Index to the library descriptor tSceLibrary
} tNIDResolver;

typedef enum
{
	SYSCALL_MODE = 0,
	JUMP_MODE = 1
} tCallingMode;

// Struct holding info to help syscall estimation
// This struct is for each library imported by the game
typedef struct
{
	char name[MAX_LIB_NAME_LEN];	// Library name
	tCallingMode calling_mode;	// Defines how library exports are called
	int num_lib_exports;		// Number of exported functions in library
	int num_known_exports;		// Number of known exported functions (exports we know the syscall of)
	int lowest_syscall;		// Lowest syscall number found
	int lowest_nid;			// NID associated to lowest syscall
	int lowest_index;		// Lowest NID index nids_table
	int highest_syscall;		// Highest syscall number found
	int gap;			// Offset between the syscall for the highest and the lowest nid index in 6.20
} tSceLibrary;

// Adds a new library
int add_lib(const tSceLibrary lib);

// Adds NID entry to nid_table
int add_nid(int nid, int call, int lib);

int add_stub(const tStubEntry *stub);

// Returns nid_table index where the call is found, -1 if not found
int get_call_index(int call);

// Returns lib_table index where the library is found, -1 if not found
int get_lib_index(const char *name);

// Returns nid_table index where the nid is found, -1 if not found
int get_nid_index(int nid);

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
// Return index in nid_table for closest higher known NID
int get_higher_known_nid(int index, int nid);

// Return index in nid_table for closest lower known NID
int get_lower_known_nid(int index, int nid);

/*
 * Retrieves highest known syscall of the previous library,
 * and lowest known syscall of the next library, to get some
 * rough boundaries of where current library's syscalls should be
 * returns 1 on success, 0 on failure
*/
int get_syscall_boundaries(int lib_index, int *low, int *high);
#endif
#endif

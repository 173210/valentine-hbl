#ifndef ELOADER_TABLES
#define ELOADER_TABLES

#include <hbl/modmgr/elf.h>

// Struct holding all NIDs imported by the game and their respective jump/syscalls
typedef struct
{
	int nid;	// NID
	int call;	// Syscall/jump associated to the NID
} tNIDResolver;

// Adds NID entry to nid_table
int add_nid(int nid, int call);

int add_stub(const tStubEntry *stub);

// Returns nid_table index where the nid is found, -1 if not found
int get_nid_index(int nid);

#endif

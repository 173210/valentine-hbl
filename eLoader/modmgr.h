#ifndef ELOADER_MODMGR
#define ELOADER_MODMGR

#include "sdk.h"

// Maximum modules that can be loaded
#define MAX_MODULES 5
#define MOD_ID_START 0x10000000

// Module states
typedef enum
{
	LOADED = 0,
	RUNNING = 1,
	STOPPED = 2
}HBLModState;

// Module information
typedef struct
{
	unsigned int id;		// Module ID given by HBL
	HBLModState state;		// Current module state
	unsigned int type;		// Static or reloc
	unsigned long size;		// Allocated size
	void* text_addr;		// Text address (useful?)
	void* text_entry;		// Entry point
	void* libstub_addr;		// .lib.stub section address
	void* gp;				// Global pointer
	char path[256];			// Path
} HBLModInfo;

// Loaded modules
typedef struct
{
	unsigned int num_loaded_mod;			// Loaded modules
	HBLModInfo table[MAX_MODULES];	// List of loaded modules info struct
} HBLModTable;

// Loads a module to memory
SceUID load_module(SceUID elf_file, const char* path, void* addr, SceOff offset);

// Start an already loaded module
SceUID start_module(SceUID modid);

#endif


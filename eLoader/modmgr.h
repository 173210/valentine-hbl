#ifndef ELOADER_MODMGR
#define ELOADER_MODMGR

#include "sdk.h"

// Maximum modules that can be loaded
#define MAX_MODULES 10
#define MOD_ID_START 0x10000000

// Module states
typedef enum
{
	LOADED = 0,
	RUNNING = 1,
	STOPPED = 2
} HBLModState;

// Module information
typedef struct
{
	SceUID id;				// Module ID given by HBL
	HBLModState state;		// Current module state
	unsigned int type;		// Static or reloc
	unsigned long size;		// Allocated size
	void* text_addr;		// Text address (useful?)
	void* text_entry;		// Entry point
	void* libstub_addr;		// .lib.stub section address
	void* gp;				// Global pointer
	char path[256];			// Path (do we need this?)
} HBLModInfo;

// Loaded modules
typedef struct
{
	unsigned int num_loaded_mod;			// Loaded modules
	HBLModInfo table[MAX_MODULES];			// List of loaded modules info struct
	unsigned int num_utility;				// Loaded utility modules
	unsigned int utility[MAX_MODULES]; 		// List of ID for utility modules loaded
} HBLModTable;

// Loads a module to memory
SceUID load_module(SceUID elf_file, const char* path, void* addr, SceOff offset);

// Start an already loaded module
SceUID start_module(SceUID modid);

//inits module loading system
void* init_load_module();

// Returns UID for a given module name
SceUID find_module_by_name(const char* mod_name);

// Loads and registers exports from an utility module
int load_export_utility_module(int mod_id, const char* lib_name , void **pexport_out );

// Returns true if utility module ID is already loaded, false otherwise
int is_utility_loaded(unsigned int mod_id);

// Adds an entry for loaded utility module
// Returns index of insertion
int add_utility_to_table(unsigned int id);

#endif


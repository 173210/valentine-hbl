#ifndef ELOADER_MODMGR
#define ELOADER_MODMGR

#include <common/sdk.h>

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
	SceUID id;		// Module ID given by HBL
	HBLModState state;	// Current module state
	void* text_entry;	// Entry point
	void* gp;		// Global pointer
	char path[256];		// Path (do we need this?)
} HBLModInfo;

// Loads a module to memory
SceUID load_module(SceUID fd, const char *path, void *addr, SceOff off);

// Start an already loaded module
// This will overwrite gp register
SceUID start_module(SceUID modid);

// Returns UID for a given module name
SceUID find_module_by_path(const char *modpath);

void unload_modules();

// Loads and registers exports from an utility module
SceLibraryEntryTable *load_export_util(const char* lib);

#endif


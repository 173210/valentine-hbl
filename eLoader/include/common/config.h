#ifndef ELOADER_CONFIG
#define ELOADER_CONFIG

// This defines data about .config file used by loader and HBL own stubs resolver

#include <common/stubs/tables.h>
#include <common/path.h>
#include <common/sdk.h>
#include <exploit_config.h>

// Imports file
#if VITA >= 180
#define IMPORTS_PATH HBL_ROOT "IMPORTS.DAT"
#else
#define IMPORTS_PATH HBL_ROOT "config/imports.config"
#endif

// Defining a library and its imports
typedef struct {
	char lib_name[MAX_LIB_NAME_LEN];	// Library name
	unsigned int num_imports;		// Number of NIDs imported from library
	SceOff nids_off;			// Offset in file for NIDs imported
} tImportedLib;

typedef struct {
	int num_lib_stubs;
	int num_libs;
	int num_nids;
} tCfgInfo;

// Imports file structure:
// u32 				num_libstubs			Number of .lib.stub addresses
// u32				num_libraries			Number of libraries imported
// u32				num_nids_total			Number of NIDs imported
// u32				libstubs[num_libstubs]		Array of .lib.stub addresses
// tImportedLib			imported_library[num_libraries]	Array of tImportedLib structs
//								Game .lib.stub address should be first
// u32				nids[num_nids]			Array of actual NIDs imported IN THE SAME ORDER
// 								as eloader stubs from lower to higher memory.
// 								Check sdk_hbl.S to be sure about it!
// IMPORTANT: NIDs from first library must be the first ones in the NID array

#define CFG_INFO_OFF 0
#define CFG_LIB_STUB_OFF (CFG_INFO_OFF + sizeof(tCfgInfo))

// Config interface, all functions return 0 on success, error code otherwise

// Initializes config file
int cfg_init();

// Closes config interface
int cfg_close();

// Returns number of libraries
int cfg_num_libs();

// Returns first library descriptor
int cfg_first_lib(tImportedLib *importedlib);

// Returns next library
int cfg_next_lib(tImportedLib *importedlib);

// Returns number of nids imported
int cfg_num_nids();

// Get first nid
int cfg_first_nid(int *nid);

// Returns next NID
int cfg_next_nid(int *nid);

#endif

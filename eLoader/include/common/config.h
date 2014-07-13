#ifndef ELOADER_CONFIG
#define ELOADER_CONFIG

// This defines data about .config file used by loader and HBL own stubs resolver

#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <hbl/eloader.h>

// Imports file
# define IMPORTS_PATH HBL_ROOT"config/imports.config"

// Defining a library and its imports
typedef struct
{
	char lib_name[MAX_LIB_NAME_LEN];	// Library name
	unsigned int num_imports;					// Number of NIDs imported from library
	SceOff nids_off;							// Offset in file for NIDs imported
} tImportedLib;

// Imports file structure:
// u32 				num_libstubs					Number of .lib.stub addresses
// u32				num_libraries					Number of libraries imported
// u32				num_nids_total					Number of NIDs imported
// u32				libstubs[num_libstubs]			Array of .lib.stub addresses
// tImportedLib	imported_library[num_libraries]	Array of tImportedLib structs
//													Game .lib.stub address should be first
// u32				nids[num_nids]					Array of actual NIDs imported IN THE SAME ORDER
// 													as eloader stubs from lower to higher memory.
// 													Check sdk_hbl.S to be sure about it!
// IMPORTANT: NIDs from first library must be the first ones in the NID array

#define NUM_LIBSTUB_OFF 0x0
#define NUM_LIBRARIES_OFF 0x4
#define NUM_NIDS_OFF 0x8
#define LIBSTUB_OFF 0xC

// Config interface, all functions return 0 on success, error code otherwise

// Initializes config file
int cfg_init(void);

// Closes config interface
int cfg_close();

// Returns how many .lib.stub addresses are referenced
int cfg_num_lib_stub(int *num_lib_stub);

// Returns first .lib.stub address
int cfg_first_lib_stub(int *lib_stub);

// Returns next .lib.stub address
int cfg_next_lib_stub(tStubEntry **lib_stub);

// Returns number of nids imported
int cfg_num_nids_total(int *num_nids_total);

// Returns number of libraries
int cfg_num_libs(int *num_libs);

// Returns first library descriptor
int cfg_first_library(tImportedLib *importedlib);

// Returns next library
int cfg_next_lib(tImportedLib *importedlib);

// Get first nid
int cfg_first_nid(int *nid);

// Returns next NID
int cfg_next_nid(int *nid);

// Returns offset of nid array
int cfg_nids_off(SceOff *off);

// Returns Nth NID
int cfg_seek_nid(int index, int *nid);

#endif

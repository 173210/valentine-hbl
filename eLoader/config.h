#ifndef ELOADER_CONFIG
#define ELOADER_CONFIG

// This defines data about .config file used by loader and HBL own stubs resolver

#include "sdk.h"
#include "eloader.h"

// Imports file
# define IMPORTS_PATH "ms0:/config/imports.config"

// Defining a library and its imports
typedef struct
{
	char library_name[MAX_LIBRARY_NAME_LENGTH];	// Library name
	unsigned int num_imports;					// Number of NIDs imported from library
	SceOff nids_offset;							// Offset in file for NIDs imported
} tImportedLibrary;

// Imports file structure:
// u32 				lib_stub						Game's .lib.stub address
// u32				num_nids_total					Number of NIDs imported
// u32				num_libraries					Number of libraries imported
// tImportedLibrary	imported_library[num_libraries]	Array of tImportedLibrary structs
// u32				nids[num_nids]					Array of actual NIDs imported IN THE SAME ORDER 
// 													as eloader stubs from lower to higher memory.
// 													Check sdk_hbl.S to be sure about it!
// IMPORTANT: NIDs from first library must be the first ones in the NID array

#define LIBSTUB_OFFSET 0
#define NUM_NIDS_TOTAL_OFFSET 4
#define NUM_LIBRARIES_OFFSET 8
#define LIBRARY_DESCRIPTOR_OFFSET 12

// Config interface, all functions return 0 on success, error code otherwise

// Initializes config file
int config_initialize(void);

// Closes config interface
int config_close();

// Returns .lib.stub address
int config_lib_stub(u32* plib_stub);

// Returns number of nids imported
int config_num_nids_total(unsigned int* pnum_nids_total);

// Returns number of libraries
int config_num_libraries(unsigned int* pnum_libraries);

// Returns first library descriptor
int config_first_library(tImportedLibrary* library_descriptor);

// Returns next library
int config_next_library(tImportedLibrary* plibrary_descriptor);

// Get first nid
int config_first_nid(u32* nid);

// Returns next NID
int config_next_nid(u32* nid);

// Returns offset of nid array
int config_nids_offset(SceOff* poffset);

// Returns Nth NID
int config_seek_nid(int index, u32* pnid);

#endif
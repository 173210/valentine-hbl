#include "config.h"

/* LOCAL GLOBALS :( */

// File descriptor
SceUID config_file = -1;

// Temp storage for not having to access the file again
int num_libraries = -1;

// Temp storage for not having to calculate again
SceOff nids_offset = -1;

/* INTERFACE IMPLEMENTATION */

// Initialize config_file
int config_initialize(void)
{
	config_file = sceIoOpen(IMPORTS_PATH, PSP_O_RDONLY, 0777);
	return config_file;
}

// Gets a u32 from offset from config
int config_u32(u32* buffer, SceOff offset)
{
	int ret = 0;

	if (buffer != NULL)
	{	
		if (offset >= 0)
			if ((ret = sceIoLseek(config_file, offset, PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(config_file, buffer, sizeof(u32));		
	}
	
	return ret;	
}

// Returns .lib.stub address
int config_lib_stub(u32* plib_stub)
{	
	return config_u32(plib_stub, LIBSTUB_OFFSET);	
}

// Returns number of nids imported
int config_num_nids_total(unsigned int* pnum_nids_total)
{	
	return config_u32(pnum_nids_total, NUM_NIDS_TOTAL_OFFSET);
}

// Returns number of libraries
int config_num_libraries(unsigned int* pnum_libraries)
{	
	int ret;
	
	ret = config_u32(&num_libraries, NUM_LIBRARIES_OFFSET);
	
	if ((ret >= 0) && (pnum_libraries != NULL))
		*pnum_libraries = num_libraries;
	
	return ret;
}

// Returns next library
int config_next_library(tImportedLibrary* plibrary_descriptor)
{
	int ret = 0;

	ret = fgetsz(config_file, &(plibrary_descriptor->library_name));

	if (ret == 0)
		return -1;
	
	ret = sceIoRead(config_file, &(plibrary_descriptor->num_imports), sizeof(unsigned int));

	if (ret >= 0)
		ret = sceIoRead(config_file, &(plibrary_descriptor->nids_offset), sizeof(SceOff));

	/*
#ifdef DEBUG
	write_debug("**LIBRARY DESCRIPTOR READ**", plibrary_descriptor->library_name, strlen(plibrary_descriptor->library_name));
	write_debug(NULL, &(plibrary_descriptor->num_imports), sizeof(unsigned int));
	write_debug(NULL, &(plibrary_descriptor->nids_offset), sizeof(SceOff));
#endif
	*/
	
	return ret;
}

// Returns first library descriptor
int config_first_library(tImportedLibrary* plibrary_descriptor)
{
	int ret;
	
	if ((ret = sceIoLseek(config_file, LIBRARY_DESCRIPTOR_OFFSET, PSP_SEEK_SET)) < 0)
		return ret;
	else
		return config_next_library(plibrary_descriptor);
}

// Returns offset of nid array
int config_nids_offset(SceOff* poffset)
{
	int ret = 0;
	tImportedLibrary first_lib;
	
	if (nids_offset < 0)
	{
		ret = config_first_library(&first_lib);

		if (ret >= 0)
			nids_offset = first_lib.nids_offset;			
	}

	if (poffset != NULL)
		*poffset = nids_offset;

	return ret;			
}

// Get first nid
int config_first_nid(u32* nid)
{
	int ret = 0;

	// Get NIDs array offset
	config_nids_offset(NULL);

	/*
#ifdef DEBUG
	write_debug("**NIDS OFFSET**", &nids_offset, sizeof(nids_offset));
#endif
	*/
	
	if ((ret = sceIoLseek(config_file, nids_offset, PSP_SEEK_SET)) >= 0)
		ret = sceIoRead(config_file, nid, sizeof(u32));

	return ret;	
}

// Returns next NID
int config_next_nid(u32* nid)
{
	return sceIoRead(config_file, nid, sizeof(u32));	
}

// Returns Nth NID
int config_seek_nid(int index, u32* pnid)
{
	int ret;

	if (nids_offset < 0)
		config_nids_offset(NULL);

	if((ret = sceIoLseek(config_file, nids_offset + (index * sizeof(u32)), PSP_SEEK_SET)) >= 0)
		ret = sceIoRead(config_file, pnid, sizeof(u32));

	return ret;
}

// Closes config interface
int config_close()
{
	int ret = 0;
	
	if (config_file >= 0)
	{
		ret = sceIoClose(config_file);
		config_file = 0;
	}

	return ret;
}

#include "debug.h"
#include "config.h"
#include "utils.h"


/* LOCAL GLOBALS :( */

// File descriptor
SceUID config_file = -1;

// Temp storage for not having to access the file again
int num_libstubs = -1;
int num_libraries = -1;
int num_nids = -1;

// Temp storage for not having to calculate again
SceOff nids_offset = -1;

/* INTERFACE IMPLEMENTATION */

// Initialize config_file
int config_initialize()
{
    u32 firmware_v = getFirmwareVersion();
	// DEBUG_PRINT(" config_initialize ", NULL, 0);
    char buffer[512];
    strcpy(buffer, IMPORTS_PATH);
    if (firmware_v == 500)
        strcat(buffer, "_50x");
    else if (firmware_v == 550)
        strcat(buffer, "_55x");
    else if (firmware_v == 570)
        strcat(buffer, "_570");
    else if (firmware_v >= 600)
        strcat(buffer, "_6xx");
        
    if (!file_exists(buffer))
        strcpy(buffer, IMPORTS_PATH);
   
    LOGSTR1("Config file:%s\n", buffer);
    
	config_file = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
	return config_file;
}

// Gets a u32 from offset from config
int config_u32(u32* buffer, SceOff offset)
{
	int ret = 0;

	//DEBUG_PRINT(" config_u32 ", &offset, sizeof(offset));

	if (buffer != NULL)
	{	
		if (offset >= 0)
			if ((ret = sceIoLseek(config_file, offset, PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(config_file, buffer, sizeof(u32));		
	}

	//DEBUG_PRINT(" VALUE READ ", buffer, sizeof(u32));
	
	return ret;	
}

// Returns how many .lib.stub addresses are referenced
int config_num_lib_stub(unsigned int* pnum_lib_stub)
{
	int ret = 0;
	
	//DEBUG_PRINT(" config_num_lib_stub ", NULL, 0);

	if (num_libstubs < 0)
		ret = config_u32(&num_libstubs, NUM_LIBSTUB_OFFSET);

	if ((ret >= 0) && (pnum_lib_stub != NULL))
		*pnum_lib_stub = num_libstubs;

	//DEBUG_PRINT(" VALUE OF num_libstubs ", &num_libstubs, sizeof(num_libstubs));

	return ret;
}

// Returns first .lib.stub address
int config_first_lib_stub(u32* plib_stub)
{
	int ret = 0;

	//DEBUG_PRINT(" config_first_lib_stub ", NULL, 0);

	if (plib_stub != NULL)
		ret = config_u32(plib_stub, LIBSTUB_OFFSET);

	return ret;
}

// Returns next .lib.stub address
int config_next_lib_stub(u32* plib_stub)
{
	//DEBUG_PRINT(" config_next_lib_stub ", NULL, 0);
	
	if (plib_stub != NULL)
		return sceIoRead(config_file, plib_stub, sizeof(u32));	
	else
		return 0;
}

// Returns number of nids imported
int config_num_nids_total(unsigned int* pnum_nids_total)
{
	int ret = 0;

	//DEBUG_PRINT(" config_num_nids_total ", NULL, 0);
	
	if (num_nids < 0)
		ret = config_u32(&num_nids, NUM_NIDS_OFFSET);

	if ((ret >= 0) && (pnum_nids_total != NULL))
		*pnum_nids_total = num_nids;

	//DEBUG_PRINT(" VALUE OF num_nids ", &num_nids, sizeof(num_nids));

	return ret;
}

// Returns number of libraries
int config_num_libraries(unsigned int* pnum_libraries)
{	
	int ret = 0;

	//DEBUG_PRINT(" config_num_libraries ", NULL, 0);

	if (num_libraries < 0)
		ret = config_u32(&num_libraries, NUM_LIBRARIES_OFFSET);

	if ((ret >= 0) && (pnum_libraries != NULL))
		*pnum_libraries = num_libraries;

	//DEBUG_PRINT(" VALUE OF num_libraries ", &num_libraries, sizeof(num_libraries));
	
	return ret;
}

// Returns next library
int config_next_library(tImportedLibrary* plibrary_descriptor)
{
	int ret = 0;

	//DEBUG_PRINT(" config_next_library ", NULL, 0);

	if (plibrary_descriptor != NULL)
	{
		ret = fgetsz(config_file, &(plibrary_descriptor->library_name));

		if (ret == 0)
			return -1;
	
		ret = sceIoRead(config_file, &(plibrary_descriptor->num_imports), sizeof(unsigned int));

		if (ret >= 0)
			ret = sceIoRead(config_file, &(plibrary_descriptor->nids_offset), sizeof(SceOff));
	}

	//DEBUG_PRINT(" VALUE OF NEXT LIBRARY ", plibrary_descriptor, sizeof(tImportedLibrary));
	
	return ret;
}

// Returns first library descriptor
int config_first_library(tImportedLibrary* plibrary_descriptor)
{
	int ret = 0;

	//DEBUG_PRINT(" config_first_library ", NULL, 0);

	if (plibrary_descriptor != NULL)
	{
		ret = 1;
		if (num_libstubs < 0)
			ret = config_num_lib_stub(NULL);

		if (ret >= 0)
		{
			ret = sceIoLseek(config_file, (int)LIBSTUB_OFFSET + (num_libstubs * sizeof(u32)), PSP_SEEK_SET);
			if (ret >= 0)
				ret = config_next_library(plibrary_descriptor);
		}
	}

	//DEBUG_PRINT(" VALUE OF FIRST LIBRARY ", plibrary_descriptor, sizeof(tImportedLibrary));
	
	return ret;
}

// Returns offset of nid array
int config_nids_offset(SceOff* poffset)
{
	int ret = 0;
	tImportedLibrary first_lib;

	//DEBUG_PRINT(" config_nids_offset ", NULL, 0);
	
	if (nids_offset < 0)
	{
		ret = config_first_library(&first_lib);

		if (ret >= 0)
			nids_offset = first_lib.nids_offset;			
	}

	if (poffset != NULL)
		*poffset = nids_offset;

	//DEBUG_PRINT(" OFFSET FIRST NID ", &nids_offset, sizeof(SceOff));

	return ret;			
}

// Get first nid
int config_first_nid(u32* pnid)
{
	int ret = 0;

	//DEBUG_PRINT(" config_first_nid ", NULL, 0);

	if (pnid != NULL)
	{
		ret = 1;
		if (nids_offset < 0)
			ret = config_nids_offset(NULL);

		if (ret >= 0)
		{	
			if ((ret = sceIoLseek(config_file, nids_offset, PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(config_file, pnid, sizeof(u32));
		}
	}

	//DEBUG_PRINT(" VALUE OF FIRST NID ", pnid, sizeof(u32));

	return ret;	
}

// Returns next NID
int config_next_nid(u32* pnid)
{
	//DEBUG_PRINT(" config_next_nid ", NULL, 0);
	
	if (pnid != NULL)
		return sceIoRead(config_file, pnid, sizeof(u32));

	return 0;
}

// Returns Nth NID
int config_seek_nid(int index, u32* pnid)
{
	int ret = 0;

	//DEBUG_PRINT(" config_seek_nid ", NULL, 0);

	if (pnid != NULL)
	{	
		ret = 1;
		if (nids_offset < 0)
			ret = config_nids_offset(NULL);

		if (ret >= 0)
			if((ret = sceIoLseek(config_file, nids_offset + (index * sizeof(u32)), PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(config_file, pnid, sizeof(u32));
	}

	//DEBUG_PRINT(" VALUE OF SEEKED NID ", pnid, sizeof(u32));

	return ret;
}

// Closes config interface
int config_close()
{
	int ret = 0;

	// DEBUG_PRINT(" config_close ", NULL, 0);
	
	if (config_file >= 0)
	{
		ret = sceIoClose(config_file);
		config_file = 0;
	}

	return ret;
}

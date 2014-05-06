#include <common/sdk.h>
#include <common/debug.h>
#include <common/config.h>
#include <common/utils.h>
#include <exploit_config.h>

// This file is in charge of loading the imports.config files from VHBL, not the vhbl "settings" which is another (manually edited) file

/* LOCAL GLOBALS :( */

// File descriptor
SceUID g_config_file = -1;

// Temp storage for not having to access the file again
int g_num_libstubs = -1;
int g_num_libraries = -1;
int g_num_nids = -1;

// Temp storage for not having to calculate again
SceOff g_nids_offset = -1;

/* INTERFACE IMPLEMENTATION */

// Initialize config_file
int config_initialize()
{
#if VITA >= 180
  const char * file_path = IMPORTS_PATH"imports.dat";
#else
    char file_path[512];

    int i = 0;
    u32 firmware_v = getFirmwareVersion();

    //We try to open a lib file base on the version of the firmware as precisely as possible,
    //then fallback to less precise versions. for example,try in this order:
    // libs_503, libs_50x, libs_5xx, libs

	do
	{
		switch (i)
		{
			case 0:
				mysprintf2(file_path, "%s_%d", (u32)IMPORTS_PATH, firmware_v);
				break;
			case 1:
				mysprintf2(file_path, "%s_%dx", (u32)IMPORTS_PATH, firmware_v / 10);
				break;
			case 2:
				mysprintf2(file_path, "%s_%dxx", (u32)IMPORTS_PATH, firmware_v / 100);
				break;
			case 3:
				mysprintf1(file_path, "%s", (u32)IMPORTS_PATH);
				break;
		}
		i++;
	}
	while ((i < 4) && !file_exists(file_path));
#endif

    LOGSTR1("Config file:%s\n", (u32) file_path);

	g_config_file = sceIoOpen(file_path, PSP_O_RDONLY, 0777);
	return g_config_file;
}

// Gets a int from offset from config
int config_u32(u32* buffer, SceOff offset)
{
	int ret = -1;

	if (buffer != NULL)
	{
		if (offset >= 0)
			if ((ret = sceIoLseek(g_config_file, offset, PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(g_config_file, buffer, sizeof(u32));
	}

	return ret;
}

// Returns how many .lib.stub addresses are referenced
int config_num_lib_stub(unsigned int* pnum_lib_stub)
{
	int ret = 0;

	if (g_num_libstubs < 0)
		ret = config_u32((u32*)&g_num_libstubs, NUM_LIBSTUB_OFFSET);

	if ((ret >= 0) && (pnum_lib_stub != NULL))
		*pnum_lib_stub = g_num_libstubs;

	return ret;
}

// Returns first .lib.stub address
int config_first_lib_stub(u32* plib_stub)
{
	int ret = 0;

	if (plib_stub != NULL)
		ret = config_u32(plib_stub, LIBSTUB_OFFSET);

	return ret;
}

// Returns next .lib.stub address
int config_next_lib_stub(u32* plib_stub)
{
	if (plib_stub != NULL)
		return sceIoRead(g_config_file, plib_stub, sizeof(u32));
	else
		return 0;
}

// Returns number of nids imported
int config_num_nids_total(unsigned int* pnum_nids_total)
{
	int ret = 0;

	if (g_num_nids < 0)
		ret = config_u32((u32*)&g_num_nids, NUM_NIDS_OFFSET);

    if (ret < 0)
    {
        LOGSTR1("ERROR Getting total number of nids: 0x%08lX\n", ret);
        return ret;
    }

	if (!pnum_nids_total)
    {
        return -1;
    }

    *pnum_nids_total = g_num_nids;

	return ret;
}

// Returns number of libraries
int config_num_libraries(unsigned int* pnum_libraries)
{
	int ret = 0;

	//DEBUG_PRINT(" config_num_libraries ", NULL, 0);

	if (g_num_libraries < 0)
		ret = config_u32((u32*)&g_num_libraries, NUM_LIBRARIES_OFFSET);

	if ((ret >= 0) && (pnum_libraries != NULL))
		*pnum_libraries = g_num_libraries;

	return ret;
}

// Returns next library
int config_next_library(tImportedLibrary* plibrary_descriptor)
{
	int ret = 0;

	if (plibrary_descriptor != NULL)
	{
		ret = fgetsz(g_config_file, plibrary_descriptor->library_name);

		if (ret == 0)
			return -1;

		ret = sceIoRead(g_config_file, &(plibrary_descriptor->num_imports), sizeof(unsigned int));

		if (ret >= 0)
			ret = sceIoRead(g_config_file, &(plibrary_descriptor->nids_offset), sizeof(SceOff));
	}

	return ret;
}

// Returns first library descriptor
int config_first_library(tImportedLibrary* plibrary_descriptor)
{
	int ret = 0;

	if (plibrary_descriptor != NULL)
	{
		ret = 1;
		if (g_num_libstubs < 0)
			ret = config_num_lib_stub(NULL);

		if (ret >= 0)
		{
			ret = sceIoLseek(g_config_file, (int)LIBSTUB_OFFSET + (g_num_libstubs * sizeof(u32)), PSP_SEEK_SET);
			if (ret >= 0)
				ret = config_next_library(plibrary_descriptor);
		}
	}

	return ret;
}

// Returns offset of nid array
int config_nids_offset(SceOff* poffset)
{
	int ret = 0;
	tImportedLibrary first_lib;

	if (g_nids_offset < 0)
	{
		ret = config_first_library(&first_lib);

		if (ret >= 0)
			g_nids_offset = first_lib.nids_offset;
	}

	if (poffset != NULL)
		*poffset = g_nids_offset;

	return ret;
}

// Get first nid
int config_first_nid(u32* pnid)
{
	int ret = 0;

	if (pnid != NULL)
	{
		ret = 1;
		if (g_nids_offset < 0)
			ret = config_nids_offset(NULL);

		if (ret >= 0)
		{
			if ((ret = sceIoLseek(g_config_file, g_nids_offset, PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(g_config_file, pnid, sizeof(u32));
		}
	}

	return ret;
}

// Returns next NID
int config_next_nid(u32* pnid)
{
	if (pnid != NULL)
		return sceIoRead(g_config_file, pnid, sizeof(u32));

	return 0;
}

// Returns Nth NID
int config_seek_nid(int index, u32* pnid)
{
	int ret = 0;

	if (pnid != NULL)
	{
		ret = 1;
		if (g_nids_offset < 0)
			ret = config_nids_offset(NULL);

		if (ret >= 0)
			if((ret = sceIoLseek(g_config_file, g_nids_offset + (index * sizeof(u32)), PSP_SEEK_SET)) >= 0)
				ret = sceIoRead(g_config_file, pnid, sizeof(u32));
	}

	return ret;
}

// Closes config interface
int config_close()
{
	int ret = 0;

	if (g_config_file >= 0)
	{
		ret = sceIoClose(g_config_file);
		g_config_file = -1;
	}

	return ret;
}

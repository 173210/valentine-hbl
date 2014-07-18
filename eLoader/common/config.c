#include <common/utils/string.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/config.h>
#include <common/utils.h>
#include <exploit_config.h>

// This file is in charge of loading the imports.config files from VHBL, not the vhbl "settings" which is another (manually edited) file

/* LOCAL GLOBALS :( */

// File descriptor
SceUID g_cfg_fd = -1;

// Temp storage for not having to access the file again
int g_num_libstubs = -1;
int g_num_libs = -1;
int g_num_nids = -1;

// Temp storage for not having to calculate again
SceOff g_nids_off = -1;

/* INTERFACE IMPLEMENTATION */

// Initialize cfg_file
int cfg_init()
{
#if VITA >= 180
	const char *file = IMPORTS_PATH"IMPORTS.DAT";

	g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
#else
	char file[512];
	int fw_ver = get_fw_ver();

	//We try to open a lib file base on the version of the firmware as precisely as possible,
	//then fallback to less precise versions. for example,try in this order:
	// libs_503, libs_50x, libs_5xx, libs

	_sprintf(file, "%s_%d", (int)IMPORTS_PATH, fw_ver);
	g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	if (g_cfg_fd < 0) {
		_sprintf(file, "%s_%dx", (int)IMPORTS_PATH, fw_ver / 10);
		g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

		if (g_cfg_fd < 0) {
			_sprintf(file, "%s_%dxx", (int)IMPORTS_PATH, fw_ver / 100);
			g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

			if (g_cfg_fd < 0) {
				strcpy(file, IMPORTS_PATH);
				g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
			}
		}
	}
#endif

	LOGSTR("Config file:%s\n", (int)file);

	g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	return g_cfg_fd;
}

// Gets a int from offset from config
int cfg_int(int *buf, SceOff off)
{
	int ret;

	if (buf == NULL || off < 0)
		return -1;

	ret = sceIoLseek(g_cfg_fd, off, PSP_SEEK_SET);
	if (ret >= 0)
		ret = sceIoRead(g_cfg_fd, buf, sizeof(int));

	return ret;
}

// Returns how many .lib.stub addresses are referenced
int cfg_num_lib_stub(int *num_lib_stub)
{
	int ret = 0;

	if (g_num_libstubs < 0)
		ret = cfg_int(&g_num_libstubs, NUM_LIBSTUB_OFF);

	if ((ret >= 0) && (num_lib_stub != NULL))
		*num_lib_stub = g_num_libstubs;

	return ret;
}

// Returns first .lib.stub address
int cfg_first_lib_stub(int *lib_stub)
{
	return cfg_int(lib_stub, LIBSTUB_OFF);
}

// Returns next .lib.stub pointer
int cfg_next_lib_stub(tStubEntry **lib_stub)
{
	return sceIoRead(g_cfg_fd, (void *)lib_stub, sizeof(tStubEntry *));
}

// Returns number of nids imported
int cfg_num_nids_total(int *num_nids_total)
{
	int ret = 0;

	if (g_num_nids < 0) {
		ret = cfg_int(&g_num_nids, NUM_NIDS_OFF);
		if (ret < 0) {
			LOGSTR("ERROR Getting total number of nids: 0x%08X\n", ret);
			return ret;
		}
	}

	if (num_nids_total != NULL)
		*num_nids_total = g_num_nids;

	return ret;
}

// Returns number of libraries
int cfg_num_libs(int *num_libs)
{
	int ret = 0;

	//DEBUG_PRINT(" cfg_num_libs ", NULL, 0);

	if (g_num_libs < 0)
		ret = cfg_int(&g_num_libs, NUM_LIBRARIES_OFF);

	if ((ret >= 0) && (num_libs != NULL))
		*num_libs = g_num_libs;

	return ret;
}

// Returns next library
int cfg_next_lib(tImportedLib *importedlib)
{
	int ret;
	char *p;

	if (importedlib == NULL)
		return -1;

	p = importedlib->lib_name;
	while(*p != '\0') {
		ret = sceIoRead(g_cfg_fd, p, sizeof(char));
		if (ret != sizeof(char))
			return ret;
	}

	ret = sceIoRead(g_cfg_fd, &(importedlib->num_imports), sizeof(int));
	if (ret >= 0)
		ret = sceIoRead(g_cfg_fd, &(importedlib->nids_off), sizeof(SceOff));

	return ret;
}

// Returns first library descriptor
int cfg_first_library(tImportedLib* importedlib)
{
	int ret = 0;

	if (importedlib == NULL)
		return -1;

	if (g_num_libstubs < 0)
		ret = cfg_num_lib_stub(NULL);

	if (ret >= 0) {
		ret = sceIoLseek(g_cfg_fd, (int)LIBSTUB_OFF + (g_num_libstubs * sizeof(int)), PSP_SEEK_SET);
		if (ret >= 0)
			ret = cfg_next_lib(importedlib);
	}

	return ret;
}

// Returns offset of nid array
int cfg_nids_off(SceOff* off)
{
	int ret = 0;
	tImportedLib importedlib;

	if (g_nids_off < 0) {
		ret = cfg_first_library(&importedlib);
		if (ret < 0)
			return -1;

		g_nids_off = importedlib.nids_off;
	}

	if (off != NULL)
		*off = g_nids_off;

	return ret;
}

// Get first nid
int cfg_first_nid(int *nid)
{
	int ret = 0;

	if (nid == NULL)
		return -1;

	if (g_nids_off < 0)
		ret = cfg_nids_off(NULL);

	if (ret >= 0) {
		ret = sceIoLseek(g_cfg_fd, g_nids_off, PSP_SEEK_SET);
		if (ret >= 0)
			ret = sceIoRead(g_cfg_fd, nid, sizeof(int));
	}

	return ret;
}

// Returns next NID
int cfg_next_nid(int *nid)
{
	return sceIoRead(g_cfg_fd, nid, sizeof(int));
}

// Returns Nth NID
int cfg_seek_nid(int index, int *nid)
{
	int ret = 0;

	if (nid == NULL)
		return -1;

	if (g_nids_off < 0)
		ret = cfg_nids_off(NULL);

	if (ret >= 0) {
		ret = sceIoLseek(g_cfg_fd, g_nids_off + (index * sizeof(int)), PSP_SEEK_SET);
		if(ret >= 0)
			ret = sceIoRead(g_cfg_fd, nid, sizeof(int));
	}

	return ret;
}

// Closes config interface
int cfg_close()
{
	int ret = 0;

	if (g_cfg_fd > 0) {
		ret = sceIoClose(g_cfg_fd);
		g_cfg_fd = -1;
	}

	return ret;
}

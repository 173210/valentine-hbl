#include <common/config.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <exploit_config.h>

// This file is in charge of loading the imports.config files from VHBL, not the vhbl "settings" which is another (manually edited) file

/* LOCAL GLOBALS :( */

// File descriptor
static SceUID g_cfg_fd = -1;

// Temp storage for not having to access the file again
static tCfgInfo cfg_info;

// Temp storage for not having to calculate again
static SceOff g_nids_off = -1;

/* INTERFACE IMPLEMENTATION */

// Initialize cfg_file
int cfg_init()
{
	int ret;
#if VITA >= 180
	const char file[] = IMPORTS_PATH;

	g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
#else
	char file[sizeof(IMPORTS_PATH) + 4] = IMPORTS_PATH "_";
	int fw_ver = globals->fw_ver;
	int i;

	for (i = 0; i < 3; i++) {
		fw_ver >>= 8;
		*(file + sizeof(IMPORTS_PATH "_") + i) = '0' + (fw_ver & 0xF);
	}

	for (i = 0; i < 3; i++) {
		g_cfg_fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
		if (g_cfg_fd > 0)
			break;

		*(file + sizeof(IMPORTS_PATH "_") + i) = 'x';
	}
#endif

	if (g_cfg_fd > 0) {
		LOG_PRINTF("Config file:%s\n", (int)file);

		ret = sceIoRead(g_cfg_fd, &cfg_info, sizeof(tCfgInfo));
		if (ret != sizeof(tCfgInfo)) {
			cfg_close();
			return ret;
		}
	}

	return g_cfg_fd;
}

// Closes config interface
int cfg_close()
{
	int ret;

	ret = sceIoClose(g_cfg_fd);
	if (!ret)
		g_cfg_fd = -1;

	return ret;
}

// Returns number of libraries
int cfg_num_libs()
{
	return cfg_info.num_libs;
}

// Returns next library
int cfg_next_lib(tImportedLib *importedlib)
{
	int ret;
	char *p;

	if (importedlib == NULL)
		return -1;

	p = importedlib->lib_name;
	do {
		ret = sceIoRead(g_cfg_fd, p, sizeof(char));
		if (ret != sizeof(char))
			return ret;
	} while(*p);

	ret = sceIoRead(g_cfg_fd, &(importedlib->num_imports), sizeof(int));
	if (ret >= 0)
		ret = sceIoRead(g_cfg_fd, &(importedlib->nids_off), sizeof(SceOff));

	return ret;
}

// Returns first library descriptor
int cfg_first_lib(tImportedLib* importedlib)
{
	int ret;

	ret = sceIoLseek(g_cfg_fd,
		CFG_LIB_STUB_OFF + (cfg_info.num_lib_stubs * 4),
		PSP_SEEK_SET);
	if (ret >= 0)
		ret = cfg_next_lib(importedlib);

	return ret;
}

// Returns offset of nid array
static int cfg_nids_off(SceOff* off)
{
	int ret = 0;
	tImportedLib importedlib;

	if (g_nids_off < 0) {
		ret = cfg_first_lib(&importedlib);
		if (ret < 0)
			return ret;

		g_nids_off = importedlib.nids_off;
	}

	if (off != NULL)
		*off = g_nids_off;

	return ret;
}

// Returns number of nids imported
int cfg_num_nids()
{
	return cfg_info.num_nids;
}

// Get first nid
int cfg_first_nid(int *nid)
{
	int ret;

	ret = g_nids_off < 0 ? cfg_nids_off(NULL) : 0;
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

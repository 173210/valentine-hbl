#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <hbl/eloader.h>
#include <hbl/mod/elf.h>
#include <hbl/stubs/hook.h>
#include <common/debug.h>
#include <hbl/mod/modmgr.h>
#include <common/malloc.h>
#include <hbl/mod/reloc.h>
#include <hbl/stubs/resolve.h>
#include <common/runtime_stubs.h>
#include <common/globals.h>
#include <svnversion.h>
#include <exploit_config.h>

int rescan_syscall(const char *mod_name, const char *lib_name );

// Return index in mod_table for module ID
int get_module_index(SceUID modid)
{
    	u32 i;

	for (i=0; i<globals->mod_table.num_loaded_mod; i++)
	{
		if (globals->mod_table.table[i].id == modid)
			return i;
	}

	return -1;
}


// Loads a module to memory
SceUID load_module(SceUID elf_file, const char* path, void* addr, SceOff offset)
{
	LOGSTR("\n\n->Entering load_module...\n");

	
	//LOGSTR("mod_table address: 0x%08X\n", mod_table);

	if (globals->mod_table.num_loaded_mod >= MAX_MODULES)
		return SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;

	LOGSTR("Reading ELF header...\n");
	// Read ELF header
	Elf32_Ehdr elf_hdr;
	sceIoRead(elf_file, &elf_hdr, sizeof(elf_hdr));

	// Check for module encryption
	if (strncmp(elf_hdr.e_ident, "~PSP", 4) == 0)
		return SCE_KERNEL_ERROR_ERROR;

	LOGELFHEADER(elf_hdr);

	// Loading module
	tStubEntry* pstub;
	unsigned int program_size, stubs_size = 0;
	unsigned int i = globals->mod_table.num_loaded_mod;
    strcpy(globals->mod_table.table[i].path, path);

	// Static ELF
	if(elf_hdr.e_type == (Elf32_Half) ELF_STATIC)
	{
		//LOGSTR("STATIC\n");

		globals->mod_table.table[i].type = ELF_STATIC;

		if(globals->mod_table.num_loaded_mod > 0)
			return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

		// Load ELF program section into memory
		elf_load_program(elf_file, offset, &elf_hdr, &program_size);

		// Locate ELF's .lib.stubs section
		stubs_size = elf_find_imports(elf_file, offset, &elf_hdr, &pstub);

		globals->mod_table.table[i].text_entry = (u32 *)elf_hdr.e_entry;
		globals->mod_table.table[i].gp = (void*)getGP(elf_file, offset, &elf_hdr);
	}

	// Relocatable ELF (PRX)
	else if(elf_hdr.e_type == (Elf32_Half) ELF_RELOC)
	{
		LOGSTR("RELOC\n");

		globals->mod_table.table[i].type = ELF_RELOC;

		LOGSTR("load_module -> Offset: 0x%08X\n", offset);

		// Load PRX program section
		if ((stubs_size = prx_load_program(elf_file, offset, &elf_hdr, &pstub, (u32*)&program_size, &addr)) == 0)
			return SCE_KERNEL_ERROR_ERROR;

		CLEAR_CACHE;

		LOGSTR("Before reloc -> Offset: 0x%08X\n", offset);
		//Relocate all sections that need to
		unsigned int ret = relocate_sections(elf_file, offset, &elf_hdr, addr);

		LOGSTR("Relocated entries: %d\n", ret);

		if (ret == 0)
		{
			LOGSTR("WARNING: no entries to relocate on a relocatable ELF\n");
		}

		// Relocate ELF entry point and GP register
		globals->mod_table.table[i].text_entry = (u32 *)((u32)elf_hdr.e_entry + (u32)addr);
        globals->mod_table.table[i].gp = (u32 *) (getGP(elf_file, offset, &elf_hdr) + (u32)addr);
	}

	// Unknown ELF type
	else
	{
        LOGSTR("Uknown ELF type: 0x%08X\n", elf_hdr.e_type);
		return SCE_KERNEL_ERROR_ERROR;
	}
	LOGSTR("resolve stubs\n");
	// Resolve ELF's stubs with game's stubs and syscall estimation
	unsigned int stubs_resolved = resolve_imports(pstub, stubs_size);

	if (stubs_resolved == 0)
    {
		LOGSTR("WARNING: no stubs found!!\n");
    }
	//LOGSTR("\nUpdating module table\n");

	globals->mod_table.table[i].id = MOD_ID_START + i;
	globals->mod_table.table[i].state = LOADED;
	globals->mod_table.table[i].size = program_size;
	globals->mod_table.table[i].text_addr = addr;
	globals->mod_table.table[i].libstub_addr = pstub;
	globals->mod_table.num_loaded_mod++;

	//LOGSTR("Module table updated\n");

	LOGSTR("\n->Actual number of loaded modules: %d\n", globals->mod_table.num_loaded_mod);
	LOGSTR("Last loaded module [%d]:\n", i);
	LOGMODENTRY(globals->mod_table.table[i]);

	CLEAR_CACHE;

	return globals->mod_table.table[i].id;
}

/*
// Thread that launches a module
void launch_module(int mod_index, void* dummy)
{
	void (*entry_point)(SceSize argc, void* argp) = globals->mod_table.table[mod_index].text_entry;
	entry_point(strlen(globals->mod_table.table[index].path) + 1, globals->mod_table.table[mod_index].path);
	sceKernelExitDeleteThread(0);
}
*/

// Starts an already loaded module
SceUID start_module(SceUID modid)
{
	LOGSTR("\n\n-->Starting module ID: 0x%08X\n", modid);

	
	if (globals->mod_table.num_loaded_mod == 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	int index = get_module_index(modid);

	if (index < 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	if (globals->mod_table.table[index].state == RUNNING)
		return SCE_KERNEL_ERROR_ALREADY_STARTED;

	LOGMODENTRY(globals->mod_table.table[index]);

	u32 gp_bak;

	GET_GP(gp_bak);
	SET_GP(globals->mod_table.table[index].gp);

	// Attempt at launching the module without thread creation (crashes on sceSystemMemoryManager ¿?)
	/*
	void (*launch_module)(int argc, char* argv) = globals->mod_table.table[index].text_entry;
	//launch_module(globals->mod_table.table[index].path + 1, globals->mod_table.table[index].path);
    launch_module(strlen(globals->mod_table.table[index].path) + 1, globals->mod_table.table[index].path);
	*/

    //The hook is called here to handle thread moniotoring
	SceUID thid = _hook_sceKernelCreateThread("hblmodule", globals->mod_table.table[index].text_entry, 0x30, 0x1000, 0xF0000000, NULL);

    char largbuff[512];
    memset(largbuff, 512, 0);
    strcpy(largbuff, globals->mod_table.table[index].path);
    int larglen = strlen(largbuff);

    /** menu code thanks to Noobz & Fanjita */
    /***************************************************************************/
	/* If this is the menu, then put the pointer to the menu API block into    */
	/* argv[1]                                                                 */
	/***************************************************************************/
	if (strcmp(largbuff, globals->menupath) == 0)
	{

		char * lptr = largbuff + larglen + 1;

		LOGSTR("lptr: %08X\n", (u32)lptr);

		_sprintf(lptr, "%08X", (u32)(&(globals->menu_api)));

		larglen += 9;

		LOGSTR("new larglen: %08X\n", larglen);

		/*************************************************************************/
		/* Now fill in some structure fields.                                    */
		/*************************************************************************/
		globals->menu_api.api_ver = 1;
		strcpy(globals->menu_api.ver_name, "Half Byte Loader R"SVNVERSION );
		strcpy(globals->menu_api.credits, "m0skit0,ab5000,wololo,davee,jjs");
		globals->menu_api.bg_fname = NULL;
		globals->menu_api.fname = globals->hb_fname;
	}

    LOGSTR("argv[0]: '%s'\n", (u32)largbuff);
    LOGSTR("argc:  %08X\n", (u32)larglen);

	if(thid >= 0)
	{
		LOGSTR("->MODULE MAIN THID: 0x%08X ", thid);
        //The hook is called here to handle thread monitoring
		thid = _hook_sceKernelStartThread(thid, larglen, (void *)largbuff);
		if (thid < 0)
		{
			LOGSTR(" HB Thread couldn't start. Error 0x%08X\n", thid);
			return thid;
		}
    }
	else
	{
        LOGSTR(" HB Thread couldn't be created. Error 0x%08X\n", thid);
		return thid;
	}

	globals->mod_table.table[index].state = RUNNING;

	SET_GP(gp_bak);

	return modid;
}

// Fills module name and return utility ID for a given utility library name
int get_utility_modname(const char* lib, char* name)
{
	// sceMpeg_library
	if (strcmp(lib, "sceMpeg") == 0)
	{
		strcpy(name, "sceMpeg_library");
	    return PSP_MODULE_AV_MPEGBASE;
	}

	// sceATRAC3plus_Library
	if (strcmp(lib, "sceAtrac3plus") == 0)
	{
		strcpy(name, "sceATRAC3plus_Library");
	    return PSP_MODULE_AV_ATRAC3PLUS;
	}

	// sceMp3_Library
	if (strcmp(lib, "sceMp3") == 0)
	{
		strcpy(name, "sceMp3_Library");
	    return PSP_MODULE_AV_MP3;
	}

	// sceNetInet_Library
	else if (strcmp(lib, "sceNetInet") == 0 ||
			 strcmp(lib, "sceNetInet_lib") == 0)
	{
		strcpy(name, "sceNetInet_Library");
		return PSP_MODULE_NET_INET;
	}

	// sceNetApctl_Library
	else if (strcmp(lib, "sceNetApctl") == 0 ||
	    	 strcmp(lib, "sceNetApctl_lib") == 0 ||
	    	 strcmp(lib, "sceNetApctl_internal_user") == 0 ||
	    	 strcmp(lib, "sceNetApctl_lib2") == 0)
	{
		strcpy(name, "sceNetApctl_Library");
		return PSP_MODULE_NET_INET;
	}

	// sceNetResolver_Library
	else if (strcmp(lib, "sceNetResolver") == 0)
	{
		strcpy(name, "sceNetResolver_Library");
		return PSP_MODULE_NET_INET;
	}

	// sceNet_Library
	else if (strcmp(lib, "sceNet") == 0 ||
	    	 strcmp(lib, "sceNet_lib") == 0)
	{
		strcpy(name, "sceNet_Library");
		return PSP_MODULE_NET_COMMON;
	}

	// sceNetAdhoc_Library
	else if (strcmp(lib, "sceNetAdhoc") == 0 ||
	    	 strcmp(lib, "sceNetAdhoc_lib") == 0)
	{
		strcpy(name, "sceNetAdhoc_Library");
		return PSP_MODULE_NET_ADHOC;
	}

	// sceNetAdhocctl_Library
	else if (strcmp(lib, "sceNetAdhocctl") == 0 ||
	    	 strcmp(lib, "sceNetAdhocctl_lib") == 0)
	{
		strcpy(name, "sceNetAdhocctl_Library");
		return PSP_MODULE_NET_ADHOC;
	}

	// sceNetAdhocMatching_Library
	else if (strcmp(lib, "sceNetAdhocMatching") == 0)
	{
		strcpy(name, "sceNetAdhocMatching_Library");
		return PSP_MODULE_NET_ADHOC;
	}

	// sceNetAdhocDownload_Library
	else if (strcmp(lib, "sceNetAdhocDownload") == 0)
	{
		strcpy(name, "sceNetAdhocDownload_Library");
		return PSP_MODULE_NET_ADHOC;
	}

	// sceNetAdhocDiscover_Library
	else if (strcmp(lib, "sceNetAdhocDiscover") == 0)
	{
		strcpy(name, "sceNetAdhocDiscover_Library");
		return PSP_MODULE_NET_ADHOC;
	}

	//SceHttp_Library
	else if (strcmp(lib, "sceHttp") == 0)
	{
		strcpy(name, "SceHttp_Library");
		return PSP_MODULE_NET_HTTP;
	}
/*
	else if (strcmp(lib, "sceSsl") == 0
		|| strcmp(lib, "sceSsl_lib") == 0 )
	{
		strcpy(name, "sceSsl_Module");
		return PSP_MODULE_NET_SSL;
	}
*/
	// Not found
	else
		return -1;
}

// Returns pointer to first export entry for a given module name and library
tExportEntry* find_module_exports_by_name(const char* mod_name, const char* lib_name)
{
	// Search for module name
	char* name_found = memfindsz(mod_name, (void*)GAME_MEMORY_START, (unsigned int)GAME_MEMORY_SIZE);

	if (name_found == NULL)
	{
		LOGSTR("->ERROR: could not find module %s\n", (u32)mod_name);
		return NULL;
	}

	// Search for library name next to module name (1 KiB size enough I guess)
	name_found = memfindsz(lib_name, name_found, 0x400);

	if (name_found == NULL)
	{
		LOGSTR("->ERROR: could not find library name %s\n", (u32)lib_name);
		return NULL;
	}

	// Search for pointer to library name close to library name
	u32* export = memfindint((int)name_found, (void *)(name_found - 0x400), 0x400);

	return (tExportEntry*)export;
}

// Returns true if utility module ID is already loaded, false otherwise
int is_utility_loaded(unsigned int mod_id)
{
	
	unsigned int i;
	for (i=0; i<globals->mod_table.num_utility; i++)
	{
		if (globals->mod_table.utility[i] == mod_id)
			return 1;
		else if (globals->mod_table.utility[i] == 0)
			break;
	}

	return 0;
}

// Adds an entry for loaded utility module
// Returns index of insertion
int add_utility_to_table(unsigned int id)
{
	
	// Check if max utilities allowed is reached
	if (globals->mod_table.num_utility >= MAX_MODULES)
		return -1;

	globals->mod_table.utility[globals->mod_table.num_utility] = id;
	globals->mod_table.num_utility++;
	return globals->mod_table.num_utility - 1;
}

int load_util(int module)
{
	LOGSTR("Loading 0x%08X\n", module);

#ifdef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
#ifdef USE_NET_MODULE_LOAD_FUNCTION
	if (module <= PSP_MODULE_NET_SSL)
		return sceUtilityLoadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);
	else
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	if (module == PSP_MODULE_USB_PSPCM)
		return sceUtilityLoadUsbModule(PSP_USB_MODULE_PSPCM);
	else if (module <= PSP_MODULE_USB_GPS)
		return sceUtilityLoadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);
	else
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	if (module <= PSP_MODULE_AV_G729)
		return sceUtilityLoadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);
	else
#endif
	return SCE_KERNEL_ERROR_ERROR;
#else
	return sceUtilityLoadModule(module);
#endif
}

int unload_util(int module)
{
	LOGSTR("Unloading 0x%08X\n", module);

#ifdef USE_EACH_UTILITY_MODULE_UNLOAD_FUNCTION
#ifdef USE_NET_MODULE_UNLOAD_FUNCTION
	if (module <= PSP_MODULE_NET_SSL)
		return sceUtilityUnloadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);
	else
#endif
#ifdef USE_USB_MODULE_UNLOAD_FUNCTION
	if (module == PSP_MODULE_USB_PSPCM)
		return sceUtilityUnloadUsbModule(PSP_USB_MODULE_PSPCM);
	else if (module <= PSP_MODULE_USB_GPS)
		return sceUtilityUnloadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);
	else
#endif
#ifdef USE_AV_MODULE_UNLOAD_FUNCTION
	if (module <= PSP_MODULE_AV_G729)
		return sceUtilityUnloadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);
	else
#endif
	return SCE_KERNEL_ERROR_ERROR;
#else
		return sceUtilityUnloadModule(module);
#endif
}

// See also resolve.c:162
// Loads and registers exports from an utility module
int load_export_utility_module(int mod_id, const char* lib_name , void **pexport_out )
{
	LOGSTR("Loading utility module for library %s\n", (u32)lib_name);

		//force load PSP_MODULE_AV_AVCODEC if we request a specific audio module
	if (mod_id > PSP_MODULE_AV_AVCODEC && mod_id <= PSP_MODULE_AV_G729) {
		LOGSTR("Force-Loading AVCODEC\n");
#ifdef USE_AV_MODULE_LOAD_FUNCTION
		sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
#else
		sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC);
#endif
	}

	if( mod_id == PSP_MODULE_NET_HTTP) {
		LOGSTR("Force-Loading HTTP\n");
#ifdef USE_NET_MODULE_LOAD_FUNCTION
		sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
		sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
		sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI);
		sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP);
#else
		sceUtilityLoadModule(PSP_MODULE_NET_COMMON);
		sceUtilityLoadModule(PSP_MODULE_NET_INET);
		sceUtilityLoadModule(PSP_MODULE_NET_PARSEURI);
		sceUtilityLoadModule(PSP_MODULE_NET_PARSEHTTP);
#endif
	}

    int ret = load_util(mod_id);
    if (ret <0)
        return ret;

	//SceUID utility_id;
	char mod_name[MAX_MODULE_NAME_LENGTH];

	if (get_utility_modname(lib_name, mod_name) < 0)
	{
		LOGSTR("Unknown/unsupported utility 0x%08X\n", mod_id);
		return -1;
	}

	// Get module exports
	tExportEntry* pexports = find_module_exports_by_name(mod_name, lib_name);

	if (pexports == NULL)
	{
		LOGSTR("->ERROR: could not find module exports for %s\n", (u32)mod_name);
		return -1;
	}

	//If mod_id is PSP_MODULE_NET_INET or PSP_MODULE_NET_COMMON, add jump addr to nid table, because it is used by net_term() function. (hook.c)
	//But other jump addr of module won't add to prevent "TABLE IS FULL" error.
	if( mod_id != PSP_MODULE_NET_INET
		&& mod_id != PSP_MODULE_NET_COMMON )
	{
		*(tExportEntry **)pexport_out = pexports;
		return 0;
	}

	*(tExportEntry **)pexport_out = NULL;

	LOGSTR("Number of export functions: %d\n", (u16)pexports->num_functions);
	LOGSTR("Pointer to exports: 0x%08X\n", (u32)pexports->exports_pointer);

	u32* pnids = (u32*)pexports->exports_pointer;
	u32* pfunctions = (u32*)((u32)pexports->exports_pointer + (u16)pexports->num_functions * 4);

	LOGSTR("Pointer to functions: 0x%08X\n", (u32)pfunctions);

	// Insert new library on library table
	tSceLibrary newlib;
	memset(&newlib, 0, sizeof(newlib));

	strcpy(newlib.name, lib_name);
	newlib.calling_mode = JUMP_MODE;
	newlib.num_lib_exports = pexports->num_functions;
	newlib.num_known_exports = pexports->num_functions;

	int lib_index = add_library_to_table(newlib);
	if (lib_index < 0)
	{
		LOGSTR("->WARNING: could not add library to table\n");
		return lib_index;
	}

	// Insert NIDs on NID table
	int i;
	for (i=0; i<(u16)pexports->num_functions; i++)
	{
		LOGSTR("NID %d: 0x%08X Function: 0x%08X\n", i, pnids[i], pfunctions[i]);
		add_nid_to_table(pnids[i], MAKE_JUMP(pfunctions[i]), lib_index);
	}

	return 0;
}

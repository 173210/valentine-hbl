#include <hbl/eloader.h>
#include <hbl/mod/elf.h>
#include <hbl/stubs/hook.h>
#include <hbl/stubs/syscall.h>
#include <common/debug.h>
#include <hbl/mod/modmgr.h>
#include <common/malloc.h>
#include <common/lib.h>
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
    tGlobals * g = get_globals();
	u32 i;

	for (i=0; i<g->mod_table.num_loaded_mod; i++)
	{
		if (g->mod_table.table[i].id == modid)
			return i;
	}

	return -1;
}


// Loads a module to memory
SceUID load_module(SceUID elf_file, const char* path, void* addr, SceOff offset)
{
	LOGSTR0("\n\n->Entering load_module...\n");

	tGlobals * g = get_globals();

	//LOGSTR1("mod_table address: 0x%08lX\n", mod_table);
	
	if (g->mod_table.num_loaded_mod >= MAX_MODULES)
		return SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;

	LOGSTR0("Reading ELF header...\n");
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
	unsigned int i = g->mod_table.num_loaded_mod;
    strcpy(g->mod_table.table[i].path, path); 
	
	// Static ELF
	if(elf_hdr.e_type == (Elf32_Half) ELF_STATIC)
	{
		//LOGSTR0("STATIC\n");

		g->mod_table.table[i].type = ELF_STATIC;
		
		if(g->mod_table.num_loaded_mod > 0)
			return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

		// Load ELF program section into memory
		elf_load_program(elf_file, offset, &elf_hdr, &program_size);		
	
		// Locate ELF's .lib.stubs section
		stubs_size = elf_find_imports(elf_file, offset, &elf_hdr, &pstub);
		
		g->mod_table.table[i].text_entry = (u32 *)elf_hdr.e_entry;
		g->mod_table.table[i].gp = (void*)getGP(elf_file, offset, &elf_hdr);
	}

	// Relocatable ELF (PRX)
	else if(elf_hdr.e_type == (Elf32_Half) ELF_RELOC)
	{
		LOGSTR0("RELOC\n");

		g->mod_table.table[i].type = ELF_RELOC;

		LOGSTR1("load_module -> Offset: 0x%08lX\n", offset);
		
		// Load PRX program section
		if ((stubs_size = prx_load_program(elf_file, offset, &elf_hdr, &pstub, (u32*)&program_size, &addr)) == 0)
			return SCE_KERNEL_ERROR_ERROR;

		CLEAR_CACHE;

		LOGSTR1("Before reloc -> Offset: 0x%08lX\n", offset);
		//Relocate all sections that need to
		unsigned int ret = relocate_sections(elf_file, offset, &elf_hdr, addr);

		LOGSTR1("Relocated entries: %d\n", ret);
		
		if (ret == 0)
		{
			LOGSTR0("WARNING: no entries to relocate on a relocatable ELF\n");
		}
		
		// Relocate ELF entry point and GP register
		g->mod_table.table[i].text_entry = (u32 *)((u32)elf_hdr.e_entry + (u32)addr);
        g->mod_table.table[i].gp = (u32 *) (getGP(elf_file, offset, &elf_hdr) + (u32)addr);		
	}

	// Unknown ELF type
	else
	{
        LOGSTR1("Uknown ELF type: 0x%08lX\n", elf_hdr.e_type);
		return SCE_KERNEL_ERROR_ERROR;
	}
	LOGSTR0("resolve stubs\n");
	// Resolve ELF's stubs with game's stubs and syscall estimation
	unsigned int stubs_resolved = resolve_imports(pstub, stubs_size);

	if (stubs_resolved == 0)
    {
		LOGSTR0("WARNING: no stubs found!!\n");
    }
	//LOGSTR0("\nUpdating module table\n");

	g->mod_table.table[i].id = MOD_ID_START + i;
	g->mod_table.table[i].state = LOADED;
	g->mod_table.table[i].size = program_size;
	g->mod_table.table[i].text_addr = addr;
	g->mod_table.table[i].libstub_addr = pstub;		
	g->mod_table.num_loaded_mod++;

	//LOGSTR0("Module table updated\n");

	LOGSTR1("\n->Actual number of loaded modules: %d\n", g->mod_table.num_loaded_mod);
	LOGSTR1("Last loaded module [%d]:\n", i);
	LOGMODENTRY(g->mod_table.table[i]);

	CLEAR_CACHE;
	
	return g->mod_table.table[i].id;
}

/*
// Thread that launches a module
void launch_module(int mod_index, void* dummy)
{
	void (*entry_point)(SceSize argc, void* argp) = g->mod_table.table[mod_index].text_entry;
	entry_point(strlen(g->mod_table.table[index].path) + 1, g->mod_table.table[mod_index].path);
	sceKernelExitDeleteThread(0);
}
*/

// Starts an already loaded module
SceUID start_module(SceUID modid)
{
	LOGSTR1("\n\n-->Starting module ID: 0x%08lX\n", modid);

	tGlobals * g = get_globals();
	
	if (g->mod_table.num_loaded_mod == 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	int index = get_module_index(modid);

	if (index < 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	if (g->mod_table.table[index].state == RUNNING)
		return SCE_KERNEL_ERROR_ALREADY_STARTED;

	LOGMODENTRY(g->mod_table.table[index]);

	u32 gp_bak;
	
	GET_GP(gp_bak);
	SET_GP(g->mod_table.table[index].gp);

	// Attempt at launching the module without thread creation (crashes on sceSystemMemoryManager ¿?)
	/*
	void (*launch_module)(int argc, char* argv) = g->mod_table.table[index].text_entry;
	//launch_module(g->mod_table.table[index].path + 1, g->mod_table.table[index].path);
    launch_module(strlen(g->mod_table.table[index].path) + 1, g->mod_table.table[index].path);
	*/

    //The hook is called here to handle thread moniotoring
	SceUID thid = _hook_sceKernelCreateThread("hblmodule", g->mod_table.table[index].text_entry, 0x30, 0x1000, 0, NULL);
    
    char largbuff[512];
    memset(largbuff, 512, 0);
    strcpy(largbuff, g->mod_table.table[index].path);
    int larglen = strlen(largbuff);

    /** menu code thanks to Noobz & Fanjita */
    /***************************************************************************/
	/* If this is the menu, then put the pointer to the menu API block into    */
	/* argv[1]                                                                 */
	/***************************************************************************/
	if (strcmp(largbuff, g->menupath) == 0)
	{

		char * lptr = largbuff + larglen + 1;

		LOGSTR1("lptr: %08lX\n", (u32)lptr);

		mysprintf1(lptr, "%08lX", (u32)(&(g->menu_api)));

		larglen += 9;

		LOGSTR1("new larglen: %08lX\n", larglen);

		/*************************************************************************/
		/* Now fill in some structure fields.                                    */
		/*************************************************************************/
		g->menu_api.APIVersion = 1;
		strcpy(g->menu_api.VersionName, "Half Byte Loader R"SVNVERSION );
		strcpy(g->menu_api.Credits, "m0skit0,ab5000,wololo,davee,jjs");
		g->menu_api.BackgroundFilename = NULL;
        g->menu_api.filename = g->hb_filename;
	}
    
    LOGSTR1("argv[0]: '%s'\n", (u32)largbuff);
    LOGSTR1("argc:  %08lX\n", (u32)larglen);
    
	if(thid >= 0)
	{
		LOGSTR1("->MODULE MAIN THID: 0x%08lX ", thid);
        //The hook is called here to handle thread monitoring
		thid = _hook_sceKernelStartThread(thid, larglen, (void *)largbuff);
		if (thid < 0)
		{
			LOGSTR1(" HB Thread couldn't start. Error 0x%08lX\n", thid);
			return thid;
		}
    }
	else 
	{
        LOGSTR1(" HB Thread couldn't be created. Error 0x%08lX\n", thid);
		return thid;
	}

	g->mod_table.table[index].state = RUNNING;
	
	SET_GP(gp_bak);

	return modid;
}

// Fills module name and return utility ID for a given utility library name
int get_utility_module_name(const char* lib, char* name)
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
		LOGSTR1("->ERROR: could not find module %s\n", (u32)mod_name);
		return NULL;
	}

	// Search for library name next to module name (1 KiB size enough I guess)
	name_found = memfindsz(lib_name, (char*)name_found, (unsigned int)0x400);

	if (name_found == NULL)
	{
		LOGSTR1("->ERROR: could not find library name %s\n", (u32)lib_name);
		return NULL;
	}

	// Search for pointer to library name close to library name
	u32* export = memfindu32((u32)name_found, (u32*)((u32)name_found - 0x400), (unsigned int)0x400);

	return (tExportEntry*)export;
}

// Returns true if utility module ID is already loaded, false otherwise
int is_utility_loaded(unsigned int mod_id)
{
	tGlobals *g = get_globals();

	unsigned int i;
	for (i=0; i<g->mod_table.num_utility; i++)
	{
		if (g->mod_table.utility[i] == mod_id)
			return 1;
		else if (g->mod_table.utility[i] == 0)
			break;
	}

	return 0;
}

// Adds an entry for loaded utility module
// Returns index of insertion
int add_utility_to_table(unsigned int id)
{
	tGlobals *g = get_globals();

	// Check if max utilities allowed is reached
	if (g->mod_table.num_utility >= MAX_MODULES)
		return -1;

	g->mod_table.utility[g->mod_table.num_utility] = id;
	g->mod_table.num_utility++;
	return g->mod_table.num_utility - 1;
}

// See also resolve.c:162
// Loads and registers exports from an utility module
int load_export_utility_module(int mod_id, const char* lib_name , void **pexport_out )
{
	LOGSTR1("Loading utility module for library %s\n", (u32)lib_name);

    //force load PSP_MODULE_AV_AVCODEC if we request a specific audio module
    if (mod_id > PSP_MODULE_AV_AVCODEC && mod_id <= PSP_MODULE_AV_G729)
    {
        LOGSTR0("Force-Loading AVCODEC\n");
        load_utility_module(PSP_MODULE_AV_AVCODEC);
    }

	if( mod_id == PSP_MODULE_NET_HTTP)
	{
        LOGSTR0("Force-Loading HTTP\n");
        load_utility_module(PSP_MODULE_NET_COMMON);
        load_utility_module(PSP_MODULE_NET_INET);
        load_utility_module(PSP_MODULE_NET_PARSEURI);
        load_utility_module(PSP_MODULE_NET_PARSEHTTP);
	}
 
    int ret = load_utility_module(mod_id);
    if (ret <0)
        return ret;
	
	//SceUID utility_id;
	char mod_name[MAX_MODULE_NAME_LENGTH];

	if (get_utility_module_name(lib_name, mod_name) < 0)
	{
		LOGSTR1("Unknown/unsupported utility 0x%08lX\n", mod_id);
		return -1;
	}

	// Get module exports
	tExportEntry* pexports = find_module_exports_by_name(mod_name, lib_name);

	if (pexports == NULL)
	{
		LOGSTR1("->ERROR: could not find module exports for %s\n", (u32)mod_name);
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

	LOGSTR1("Number of export functions: %d\n", (u16)pexports->num_functions);
	LOGSTR1("Pointer to exports: 0x%08lX\n", (u32)pexports->exports_pointer);

	u32* pnids = (u32*)pexports->exports_pointer;
	u32* pfunctions = (u32*)((u32)pexports->exports_pointer + (u16)pexports->num_functions * 4);

	LOGSTR1("Pointer to functions: 0x%08lX\n", (u32)pfunctions);

	// Insert new library on library table
	tSceLibrary newlib;
	memset(&newlib, 0, sizeof(newlib));
	
	strcpy(newlib.name, lib_name);
	newlib.calling_mode = JUMP_MODE;
	newlib.num_library_exports = pexports->num_functions;
	newlib.num_known_exports = pexports->num_functions;

	int lib_index = add_library_to_table(newlib);
	if (lib_index < 0)
	{
		LOGSTR0("->WARNING: could not add library to table\n");
		return lib_index;
	}

	// Insert NIDs on NID table
	int i;
	for (i=0; i<(u16)pexports->num_functions; i++)
	{
		LOGSTR3("NID %d: 0x%08lX Function: 0x%08lX\n", i, pnids[i], pfunctions[i]);
		add_nid_to_table(pnids[i], MAKE_JUMP(pfunctions[i]), lib_index);
	}

	return 0;
}

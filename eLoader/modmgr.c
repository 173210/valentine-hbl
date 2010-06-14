#include "eloader.h"
#include "elf.h"
#include "syscall.h"
#include "debug.h"
#include "modmgr.h"
#include "malloc.h"
#include "lib.h"
#include "reloc.h"
#include "resolve.h"
#include "globals.h"

// Loaded modules descriptor
//HBLModTable* mod_table = NULL;
//HBLModTable mod_table;

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
    tGlobals * g = get_globals();
	LOGSTR0("\n\n->Entering load_module...\n");

	//LOGSTR1("mod_table address: 0x%08lX\n", mod_table);
	
	if (g->mod_table.num_loaded_mod >= MAX_MODULES)
		return SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;

	LOGSTR0("Reading ELF header...\n");
	// Read ELF header
	Elf32_Ehdr elf_hdr;
	sceIoRead(elf_file, &elf_hdr, sizeof(elf_hdr));

	LOGELFHEADER(elf_hdr);
	
	// Loading module
	tStubEntry* pstub;
	unsigned int hbsize, program_size, stubs_size = 0;
	unsigned int i = g->mod_table.num_loaded_mod;
	
	// Static ELF
	if(elf_hdr.e_type == (Elf32_Half) ELF_STATIC)
	{
		//LOGSTR0("STATIC\n");

		g->mod_table.table[i].type = ELF_STATIC;
		
		if(g->mod_table.num_loaded_mod > 0)
			return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

		// Load ELF program section into memory
		hbsize = elf_load_program(elf_file, offset, &elf_hdr, &program_size);		
	
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
		if ((stubs_size = prx_load_program(elf_file, offset, &elf_hdr, &pstub, &program_size, &addr)) == 0)
			return SCE_KERNEL_ERROR_ERROR;

		sceKernelDcacheWritebackInvalidateAll();

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
		exit_with_log(" UNKNOWN ELF TYPE ", NULL, 0);
	}
	
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
	strcpy(g->mod_table.table[i].path, path);	
	g->mod_table.num_loaded_mod++;

	//LOGSTR0("Module table updated\n");

	LOGSTR1("\n->Actual number of loaded modules: %d\n", g->mod_table.num_loaded_mod);
	LOGSTR1("Last loaded module [%d]:\n", i);
	LOGMODENTRY(g->mod_table.table[i]);
  
	// No need for ELF file anymore
	sceIoClose(elf_file);

	sceKernelDcacheWritebackInvalidateAll();
	
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
    tGlobals * g = get_globals();
	LOGSTR1("\n\n-->Starting module ID: 0x%08lX\n", modid);
	
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
	launch_module(g->mod_table.table[index].path + 1, g->mod_table.table[index].path);
	*/

	SceUID thid = sceKernelCreateThread("hblmodule", g->mod_table.table[index].text_entry, 0x30, 0x1000, 0, NULL);

	if(thid >= 0)
	{
		LOGSTR1("->MODULE MAIN THID: 0x%08lX ", thid);
		thid = sceKernelStartThread(thid, strlen(g->mod_table.table[index].path) + 1, (void *)g->mod_table.table[index].path);
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

// TODO: fake function, only works with MP3 module on 5.00
// Returns pointer to first export entry for a given module name
tExportEntry* find_module_exports_by_name(const char* mod_name)
{
	LOGSTR1("-->find_module_by_name: %s\n", (u32)mod_name);

	/*
	char* name_found;

	name_found = memfindsz(mod_name, (void*)GAME_MEMORY_START, (unsigned int)GAME_MEMORY_SIZE);

	int mod_id = sceKernelGetModuleIdByAddress((void*)name_found);

	if (mod_id >= 0)
	{
	}
	*/

	// Just to avoid error on non-debug compilation
	if (mod_name == NULL) {}

	return (tExportEntry*)0x08805274;
}

// See also resolve.c:162
// Loads and registers exports from an utility module
int load_utility_module(int mod_id, const char* lib_name)
{
	LOGSTR0("Loading utility module\n");
	
	//SceUID utility_id;
	char modname[MAX_MODULE_NAME_LENGTH];

	// Get utility module name
	switch (mod_id)
	{
		case PSP_MODULE_AV_MP3:
			strcpy(modname, "sceMp3_Library");
			break;

		default:
			LOGSTR1("Unknown/unsupported utility 0x%08lX\n", mod_id);
			return -1;
	}

	// Load utility
	int ret;
	if ((ret = sceUtilityLoadModule(mod_id)) < 0)
	{
		LOGSTR2("ERROR 0x%08lX: unable to load utility 0x%08lX\n", ret, mod_id);
		return ret;
	}

	// TODO: register utility module loaded on HBL module table

	// Get module exports
	tExportEntry* pexports = find_module_exports_by_name(modname);

	LOGSTR1("Module name: %s\n", (u32)pexports->name);
	LOGSTR1("Exports size (in DWORDs): %d\n", (u8)pexports->size);
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
	tNIDResolver entry;
	entry.lib_index = lib_index;
	int i;
	for (i=0; i<(u16)pexports->num_functions; i++)
	{
		LOGSTR3("NID %d: 0x%08lX Function: 0x%08lX\n", i, pnids[i], pfunctions[i]);
		entry.nid = pnids[i];
		entry.call = MAKE_JUMP(pfunctions[i]);
		add_nid_to_table(entry.nid, entry.call, entry.lib_index);
	}

	return 0;
}

#include "eloader.h"
#include "elf.h"
#include "syscall.h"
#include "debug.h"
#include "modmgr.h"

// Loaded modules descriptor
HBLModTable* mod_table = NULL;
//HBLModTable mod_table;

// Return index in mod_table for module ID
int get_module_index(SceUID modid)
{
	int i;

	for (i=0; i<mod_table->num_loaded_mod; i++)
	{
		if (mod_table->table[i].id == modid)
			return i;
	}

	return -1;
}

// Initialize module loading data structures
void* init_load_module()
{
	mod_table = malloc(sizeof(HBLModTable));

	if (mod_table != NULL)
	{
		memset(mod_table, 0, sizeof(HBLModTable));
		LOGSTR1("Module table created @ 0x%08lX\n", mod_table);
	}

	return mod_table;
}

// Loads a module to memory
SceUID load_module(SceUID elf_file, const char* path, void* addr, SceOff offset)
{
	LOGSTR0("\n\n->Entering load_module...\n");

	//LOGSTR1("mod_table address: 0x%08lX\n", mod_table);
	
	if (mod_table->num_loaded_mod >= MAX_MODULES)
		return SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;

	LOGSTR0("Reading ELF header...\n");
	// Read ELF header
	Elf32_Ehdr elf_hdr;
	sceIoRead(elf_file, &elf_hdr, sizeof(elf_hdr));

	LOGELFHEADER(elf_hdr);
	
	// Loading module
	tStubEntry* pstub;
	unsigned int hbsize, program_size, stubs_size;
	unsigned int i = mod_table->num_loaded_mod;
	
	// Static ELF
	if(elf_hdr.e_type == (Elf32_Half) ELF_STATIC)
	{
		//LOGSTR0("STATIC\n");

		mod_table->table[i].type = ELF_STATIC;
		
		if(mod_table->num_loaded_mod > 0)
			return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

		// Load ELF program section into memory
		hbsize = elf_load_program(elf_file, offset, &elf_hdr, &program_size);		
	
		// Locate ELF's .lib.stubs section
		stubs_size = elf_find_imports(elf_file, offset, &elf_hdr, &pstub);
		
		mod_table->table[i].text_entry = (u32)elf_hdr.e_entry;
		mod_table->table[i].gp = (void*)getGP(elf_file, offset, &elf_hdr);
	}

	// Relocatable ELF (PRX)
	else if(elf_hdr.e_type == (Elf32_Half) ELF_RELOC)
	{
		LOGSTR0("RELOC\n");

		mod_table->table[i].type = ELF_RELOC;

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
		mod_table->table[i].text_entry = (u32)elf_hdr.e_entry + (u32)addr;
        mod_table->table[i].gp = getGP(elf_file, offset, &elf_hdr) + (u32)addr;		
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

	mod_table->table[i].id = MOD_ID_START + i;
	mod_table->table[i].state = LOADED;
	mod_table->table[i].size = program_size;
	mod_table->table[i].text_addr = addr;
	mod_table->table[i].libstub_addr = pstub;	
	strcpy(mod_table->table[i].path, path);	
	mod_table->num_loaded_mod++;

	//LOGSTR0("Module table updated\n");

	LOGSTR1("\n->Actual number of loaded modules: %d\n", mod_table->num_loaded_mod);
	LOGSTR1("Last loaded module [%d]:\n", i);
	LOGMODENTRY(mod_table->table[i]);
  
	// No need for ELF file anymore
	sceIoClose(elf_file);

	sceKernelDcacheWritebackInvalidateAll();
	
	return mod_table->table[i].id;
}

/*
// Thread that launches a module
void launch_module(int mod_index, void* dummy)
{
	void (*entry_point)(SceSize argc, void* argp) = mod_table->table[mod_index].text_entry;
	entry_point(strlen(mod_table->table[index].path) + 1, mod_table->table[mod_index].path);
	sceKernelExitDeleteThread(0);
}
*/

// Starts an already loaded module
SceUID start_module(SceUID modid)
{
	LOGSTR1("\n\n-->Starting module ID: 0x%08lX\n", modid);
	
	if (mod_table->num_loaded_mod == 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	int index = get_module_index(modid);

	if (index < 0)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	if (mod_table->table[index].state == RUNNING)
		return SCE_KERNEL_ERROR_ALREADY_STARTED;

	LOGMODENTRY(mod_table->table[index]);

	u32 gp_bak;
	
	GET_GP(gp_bak);
	SET_GP(mod_table->table[index].gp);

	// Attempt at launching the module without thread creation (crashes on sceSystemMemoryManager ¿?)
	/*
	void (*launch_module)(int argc, char* argv) = mod_table->table[index].text_entry;
	launch_module(mod_table->table[index].path + 1, mod_table->table[index].path);
	*/

	SceUID thid = sceKernelCreateThread("hblmodule", mod_table->table[index].text_entry, 0x30, 0x1000, 0, NULL);

	if(thid >= 0)
	{
		LOGSTR1("->MODULE MAIN THID: 0x%08lX ", thid);
		thid = sceKernelStartThread(thid, strlen(mod_table->table[index].path) + 1, (void *)mod_table->table[index].path);
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

	mod_table->table[index].state = RUNNING;
	
	SET_GP(gp_bak);

	return modid;
}

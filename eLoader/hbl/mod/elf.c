#include <common/sdk.h>
#include <hbl/mod/elf.h>
#include <common/debug.h>
#include <hbl/eloader.h>
#include <hbl/stubs/hook.h>
#include <common/malloc.h>
#include <common/utils.h>
#include <exploit_config.h>


//utility
// Allocates memory for homebrew so it doesn't overwrite itself
//This function is inly used in elf.c. Let's move it to malloc.c ONLY if other files start to need it
void * allocate_memory(u32 size, void* addr)
{
    int type = PSP_SMEM_Low;
    if (addr)
    {
        type = PSP_SMEM_Addr;
    }

    LOGSTR2("-->ALLOCATING MEMORY @ 0x%08lX size 0x%08lX... ", (u32)addr, size);
    //hook is used to monitor ram usage and free it on exit
    SceUID mem = _hook_sceKernelAllocPartitionMemory(2, "ELFMemory", type, size, addr);

	if (mem < 0)
	{
		LOGSTR1("FAILED: 0x%08lX\n", mem);
        return NULL;
	}

    LOGSTR0("OK\n");

	return sceKernelGetBlockHeadAddr(mem);
}


/*****************/
/* ELF FUNCTIONS */
/*****************/

// Returns 1 if header is ELF, 0 otherwise
int elf_check_magic(Elf32_Ehdr* pelf_header)
{
    if( (pelf_header->e_ident[EI_MAG0] == ELFMAG0) &&
        (pelf_header->e_ident[EI_MAG1] == ELFMAG1) &&
        (pelf_header->e_ident[EI_MAG2] == ELFMAG2) &&
        (pelf_header->e_ident[EI_MAG3] == ELFMAG3))
        return 1;
    else
        return 0;
}

// Loads static executable in memory using virtual address
// Returns total size copied in memory
unsigned int elf_load_program(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, unsigned int* size)
{
	Elf32_Phdr program_header;
	int excess;
	void *buffer;

	// Initialize size return value
	*size = 0;

	int i;
	for (i = 0; i < pelf_header->e_phnum; i++)
	{
		LOGSTR2("Reading program section %d of %d\n", i, pelf_header->e_phnum);

		// Read the program header
		sceIoLseek(elf_file, start_offset + pelf_header->e_phoff + (sizeof(Elf32_Phdr) * i), PSP_SEEK_SET);
		sceIoRead(elf_file, &program_header, sizeof(Elf32_Phdr));

		// Loads program segment at virtual address
		sceIoLseek(elf_file, start_offset + program_header.p_offset, PSP_SEEK_SET);
		buffer = (void *) program_header.p_vaddr;
		allocate_memory(program_header.p_memsz, buffer);
		sceIoRead(elf_file, buffer, program_header.p_filesz);

		// Sets the buffer pointer to end of program segment
		buffer = buffer + program_header.p_filesz;

		// Fills excess memory with zeroes
		excess = program_header.p_memsz - program_header.p_filesz;
		if(excess > 0)
			memset(buffer, 0, excess);

		*size += program_header.p_memsz;
	}

	return *size;
}

// Loads relocatable executable in memory using fixed address
// Loads address of first stub header in pstub_entry
// Returns number of stubs
unsigned int prx_load_program(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub_entry, u32* size, void** addr)
{
	Elf32_Phdr program_header;
	int excess;
    void * buffer;
	tModInfoEntry module_info;

	//LOGSTR1("prx_load_program -> Offset: 0x%08lX\n", start_offset);

	// Read the program header
	sceIoLseek(elf_file, start_offset + pelf_header->e_phoff, PSP_SEEK_SET);
	sceIoRead(elf_file, &program_header, sizeof(Elf32_Phdr));

	LOGELFPROGHEADER(program_header);

    // DEBUG_PRINT("Program Header:", &program_header, sizeof(Elf32_Phdr));
    
	// Check if kernel mode
	if ((unsigned int)program_header.p_paddr & 0x80000000)
		return 0;

	LOGSTR1("Module info @ 0x%08lX offset\n", (u32)start_offset + (u32)program_header.p_paddr);

	// Read module info from PRX
	sceIoLseek(elf_file, (u32)start_offset + (u32)program_header.p_paddr, PSP_SEEK_SET);
	sceIoRead(elf_file, &module_info, sizeof(tModInfoEntry));


    LOGMODINFO(module_info);
    
	// Loads program segment at fixed address
	sceIoLseek(elf_file, start_offset + (SceOff) program_header.p_offset, PSP_SEEK_SET);

	LOGSTR1("Address to allocate from: 0x%08lX\n", (u32)*addr);
    buffer = allocate_memory(program_header.p_memsz, *addr);
    *addr = buffer;

	if (!buffer)
	{
		LOGSTR0("Failed to allocate memory for the module\n");
		return 0;
	}

	LOGSTR1("Allocated memory address from: 0x%08lX\n", (u32) *addr);
	
	sceIoRead(elf_file, buffer, program_header.p_filesz);

	// Sets the buffer pointer to end of program segment
	buffer = buffer + program_header.p_filesz + 1;

	LOGSTR0("Zero filling\n");
	// Fills excess memory with zeroes
    *size = program_header.p_memsz;
	excess = program_header.p_memsz - program_header.p_filesz;
	if(excess > 0) {
		LOGSTR1("Zero filling: %d bytes\n", excess);
        memset(buffer, 0, excess);
	}

	*pstub_entry = (tStubEntry*)((u32)module_info.library_stubs + (u32)*addr);

	LOGSTR1("stub_entry address: 0x%08lX\n", (u32)*pstub_entry);

	// Return size of stubs
	return ((u32)module_info.library_stubs_end - (u32)module_info.library_stubs);
}

// Get index of section if you know the section name
int elf_get_section_index_by_section_name(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, char* section_name_to_find)
{
	#define SECTION_NAME_MAX_LENGTH 50

	Elf32_Shdr section_header;
	Elf32_Shdr section_header_names;
	char section_name[SECTION_NAME_MAX_LENGTH]; // Should be enough to hold the name
	unsigned int section_name_length = strlen(section_name_to_find);

	if (section_name_length > SECTION_NAME_MAX_LENGTH)
	{
		// Section name too long
		LOGSTR2("Section name to find is too long (strlen = %d, max = %d)\n", section_name_length, SECTION_NAME_MAX_LENGTH);
		return -1;
	}

	// Seek to the section name table
	sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + (pelf_header->e_shstrndx * sizeof(Elf32_Shdr)), PSP_SEEK_SET);
	sceIoRead(elf_file, &section_header_names, sizeof(Elf32_Shdr));

	// Read section names and compare them
	int i;
	for (i = 0; i < pelf_header->e_shnum; i++)
	{
		// Get name index from current section header
		sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + (i * sizeof(Elf32_Shdr)), PSP_SEEK_SET);
		sceIoRead(elf_file, &section_header, sizeof(Elf32_Shdr));

		// Seek to the name entry in the name table
		sceIoLseek(elf_file, start_offset + section_header_names.sh_offset + section_header.sh_name, PSP_SEEK_SET);

		// Read name in (is the section name length stored anywhere?)
		sceIoRead(elf_file, section_name, section_name_length);

		if (strncmp(section_name, section_name_to_find, section_name_length) == 0)
		{
			LOGSTR1("Found section index %d\n", i);
			return i;
		}
	}

	// Section not found
	LOGSTR0("ERROR: Section could not be found!\n");
	return -1;
}

// Get module GP
u32 getGP(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header)
{
	Elf32_Shdr section_header;
	tModInfoEntry module_info;
	int section_index = -1;

    section_index = elf_get_section_index_by_section_name(elf_file, start_offset, pelf_header, ".rodata.sceModuleInfo");
	if (section_index < 0)
	{
		// Module info not found in sections
		LOGSTR0("ERROR: section rodata.sceModuleInfo not found\n");
		return 0;
	}

	// Read in section header
    sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + (section_index * sizeof(Elf32_Shdr)), PSP_SEEK_SET);
	sceIoRead(elf_file, &section_header, sizeof(Elf32_Shdr));

	// Read in module info
    sceIoLseek(elf_file, start_offset + section_header.sh_offset, PSP_SEEK_SET);
    sceIoRead(elf_file, &module_info, sizeof(tModInfoEntry));

    LOGMODINFO(module_info);

    return (u32) module_info.gp;
}

// Copies the string pointed by table_offset into "buffer"
// WARNING: modifies file pointer. This behaviour MUST be changed
unsigned int elf_read_string(SceUID elf_file, Elf32_Off table_offset, char *buffer)
{
	int i;

	sceIoLseek(elf_file, table_offset, PSP_SEEK_SET);

	i = 0;
	do
	{
		sceIoRead(elf_file, &(buffer[i]), sizeof(char));
	} while(buffer[i++]);

	return i-1;
}

// Returns size and address (pstub) of ".lib.stub" section (imports)
unsigned int elf_find_imports(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub)
{
	Elf32_Half i;
	Elf32_Shdr sec_header;
	Elf32_Off strtab_offset;
	Elf32_Off cur_offset;
	char section_name[40];

	// Seek string table
	sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + pelf_header->e_shstrndx * sizeof(Elf32_Shdr), PSP_SEEK_SET);
	sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));
	strtab_offset = sec_header.sh_offset;

	// First section header
	cur_offset = pelf_header->e_shoff;

	// Browse all section headers
	for(i=0; i<pelf_header->e_shnum; i++)
	{
		// Get section header
		sceIoLseek(elf_file, start_offset + cur_offset, PSP_SEEK_SET);
		sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));

		// Get section name
		elf_read_string(elf_file,  start_offset + strtab_offset + sec_header.sh_name, section_name);

		// Check if it's ".lib.stub"
		if(!strcmp(section_name, ".lib.stub"))
		{
			// Return values
			*pstub = (tStubEntry*) sec_header.sh_addr;
			return sec_header.sh_size;
		}

		// Next section header
		cur_offset += sizeof(Elf32_Shdr);
	}

	// If we get here, no ".lib.stub" section found
	return 0;
}


// Extracts ELF from PBP,returns pointer to EBOOT File + fills offset
SceUID elf_eboot_extract_open(const char* eboot_path, SceOff *offset)
{
	SceUID eboot;
	*offset = 0;

	CLEAR_CACHE;

	eboot = sceIoOpen(eboot_path, PSP_O_RDONLY, 777);

	sceIoLseek(eboot, 0x20, PSP_SEEK_SET);
	sceIoRead(eboot, offset, sizeof(u32));
	sceIoLseek(eboot, *offset, PSP_SEEK_SET);

	return eboot;
}

#include "elf.h"
#include "eloader.h"
#include "debug.h"

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

// Returns !=0 if stub entry is valid, 0 if it's not
// Just checks if pointers are not NULL
int elf_check_stub_entry(tStubEntry* pentry)
{
	return ((pentry->library_name) &&
	(pentry->nid_pointer) &&
	(pentry->jump_pointer));
}

// Loads static executable in memory using virtual address
// Returns total size copied in memory
unsigned int elf_load_program(SceUID elf_file, u32 start_offset, Elf32_Ehdr* pelf_header, void (*alloc)(u32, void*))
{
	Elf32_Phdr program_header;
	int excess;
	void *buffer;

	// Read the program header
	sceIoLseek(elf_file, start_offset + pelf_header->e_phoff, PSP_SEEK_SET);
	sceIoRead(elf_file, &program_header, sizeof(Elf32_Phdr));
    
	// Loads program segment at virtual address
	sceIoLseek(elf_file, start_offset + program_header.p_offset, PSP_SEEK_SET);
	buffer = (void *) program_header.p_vaddr;
	alloc(program_header.p_memsz, buffer);
	sceIoRead(elf_file, buffer, program_header.p_filesz);

	// Sets the buffer pointer to end of program segment
	buffer = buffer + program_header.p_filesz + 1;

	// Fills excess memory with zeroes
	excess = program_header.p_memsz - program_header.p_filesz;
	if(excess > 0)
        memset(buffer, 0, excess);

	return program_header.p_memsz;
}

// Loads relocatable executable in memory using fixed address
// Loads address of first stub header in pstub_entry
// Returns number of stubs
unsigned int prx_load_program(SceUID elf_file, u32 start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub_entry, u32* size, void (*alloc)(u32, void*))
{
	Elf32_Phdr program_header;
	int excess;
	void *buffer;
	tModInfoEntry module_info;

	// Read the program header
	sceIoLseek(elf_file, start_offset + pelf_header->e_phoff, PSP_SEEK_SET);
	sceIoRead(elf_file, &program_header, sizeof(Elf32_Phdr));

    // DEBUG_PRINT("Program Header:", &program_header, sizeof(Elf32_Phdr));
    
	// Check if kernel mode
	if ((unsigned int)program_header.p_paddr & 0x80000000)
		return 0;

	// Read module info from PRX
	sceIoLseek(elf_file, start_offset + (u32)program_header.p_paddr, PSP_SEEK_SET);
	sceIoRead(elf_file, &module_info, sizeof(tModInfoEntry));

    LOGMODINFO(module_info);
    
	// Loads global pool over Valentine's global pool
    /*
	sceIoLseek(elf_file, start_offset + (u32)module_info.gp - 0x8000, PSP_SEEK_SET);
	buffer = (void *) 0x08B43B50 - 0x8000;
	sceIoRead(elf_file, buffer, 0x10000);        
    */
    
	// Loads program segment at fixed address
	sceIoLseek(elf_file, start_offset + program_header.p_offset, PSP_SEEK_SET);
	buffer = (void *) PRX_LOAD_ADDRESS;
	alloc(program_header.p_memsz, buffer);
	sceIoRead(elf_file, buffer, program_header.p_filesz);

	// Sets the buffer pointer to end of program segment
	buffer = buffer + program_header.p_filesz + 1;

	// Fills excess memory with zeroes
    size[0] = program_header.p_memsz;
	excess = program_header.p_memsz - program_header.p_filesz;
	if(excess > 0)
        memset(buffer, 0, excess);

	*pstub_entry = (u32)module_info.library_stubs + (u32)PRX_LOAD_ADDRESS;

	// Return size of stubs
	return ((u32)module_info.library_stubs_end - (u32)module_info.library_stubs);
}

// Get module GP
u32 getGP(SceUID elf_file, u32 start_offset, Elf32_Ehdr* pelf_header)
{
	Elf32_Phdr program_header;
	void *buffer;
	tModInfoEntry module_info;

	// Read the program header
	sceIoLseek(elf_file, start_offset + pelf_header->e_phoff, PSP_SEEK_SET);
	sceIoRead(elf_file, &program_header, sizeof(Elf32_Phdr));

    // DEBUG_PRINT("Program Header:", &program_header, sizeof(Elf32_Phdr));    

	// Read module info from PRX
	sceIoLseek(elf_file, start_offset + (u32)program_header.p_paddr, PSP_SEEK_SET);
	sceIoRead(elf_file, &module_info, sizeof(tModInfoEntry));

    // LOGMODINFO(module_info);
	
    return (u32) module_info.gp;
}

// Copies the string pointed by table_offset into "buffer"
// WARNING: modifies file pointer. This behaviour MUST be changed
unsigned int elf_read_string(SceUID elf_file, Elf32_Off table_offset, char *buffer)
{
	int i;
	char c;

	sceIoLseek(elf_file, table_offset, PSP_SEEK_SET);

	i = 0;
	do
	{
		sceIoRead(elf_file, &(buffer[i]), sizeof(char));
	} while(buffer[i++]);

	return i-1;
}

// Returns size and address (pstub) of ".lib.stub" section (imports)
unsigned int elf_find_imports(SceUID elf_file, u32 start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub)
{
	Elf32_Half i;
	Elf32_Shdr sec_header;
	Elf32_Off strtab_offset;
	Elf32_Off cur_offset;
	char section_name[40];
	unsigned int section_name_size;

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
		section_name_size = elf_read_string(elf_file,  start_offset + strtab_offset + sec_header.sh_name, section_name);

		// Check if it's ".lib.stub"
		if(!strcmp(section_name, ".lib.stub"))
		{
			// Return values
			*pstub = sec_header.sh_addr;
			return sec_header.sh_size;
		}

		// Next section header
		cur_offset += sizeof(Elf32_Shdr);
	}

	// If we get here, no ".lib.stub" section found
	return 0;
}


// Extracts ELF from PBP,returns pointer to EBOOT File + fills offset
SceUID elf_eboot_extract_open(const char* eboot_path, u32 *offset)
{
	SceUID eboot;
	*offset = 0;
	char buf[2048];
	unsigned int size = 0;

	sceKernelDcacheWritebackInvalidateAll();

	eboot = sceIoOpen(eboot_path, PSP_O_RDONLY, 777);

	sceIoLseek(eboot, 0x20, PSP_SEEK_SET);
	sceIoRead(eboot, offset, sizeof(u32));


	sceIoLseek(eboot, *offset, PSP_SEEK_SET);

	return eboot;
}

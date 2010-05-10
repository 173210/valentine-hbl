#include "debug.h"
#include "sdk.h"
#include "utils.h"
#include "tables.h"
#include "elf.h"
#include "modmgr.h"
   
// Globals for debugging
#ifdef DEBUG
	SceUID dbglog;
	u32 aux = 0;
#endif

void init_debug()
{
	SceUID fd;
	
	if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777)) >= 0)
	{
		sceIoClose(fd);
	}
}

void write_debug_newline (const char* description)
{
	write_debug(description, "\n", 1);
}

// Debugging log function
void write_debug(const char* description, void* value, unsigned int size)
{
	SceUID fd;
	
	if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777)) >= 0)
	{
		if (description != NULL)
		{
			sceIoWrite(PSPLINK_OUT, description, strlen(description));		
			sceIoWrite(fd, description, strlen(description));
		}
		if (value != NULL) 
		{
			sceIoWrite(PSPLINK_OUT, value, size);		
			sceIoWrite(fd, value, size);
		}
		sceIoClose(fd);
	}
}

void exit_with_log(const char* description, void* value, unsigned int size)
{
	write_debug(description, value, size);
	sceKernelExitGame();
}

void log_library(tSceLibrary lib)
{
	LOGSTR0("\n-->Library name: ");
	LOGSTR0(lib.name);
	LOGSTR0("\n");
	LOGSTR1("--Calling mode: %d\n", lib.calling_mode);
	LOGSTR1("--Total library exports: %d\n", lib.num_library_exports);
	LOGSTR1("--Known library exports: %d\n", lib.num_known_exports);
	LOGSTR2("--Lowest NID/SYSCALL:  0x%08lX/0x%08lX\n", lib.lowest_nid, lib.lowest_syscall);
	LOGSTR1("--Lowest index in file: %d\n", lib.lowest_index);
}

void log_modinfo(tModInfoEntry modinfo)
{
	LOGSTR0("\n->Module information:\n");
	LOGSTR1("Name: %s\n", modinfo.module_name);
	LOGSTR1("Version: 0x%08lX\n", modinfo.module_version);
	LOGSTR1("Attributes: 0x%08lX\n", modinfo.module_attributes);
	LOGSTR1("Lib entry: 0x%08lX\n", modinfo.library_entry);
	LOGSTR1("Lib stubs: 0x%08lX\n", modinfo.library_stubs);
}

void log_elf_header(Elf32_Ehdr eheader)
{
	LOGSTR0("\n->ELF header:\n");
	LOGSTR1("Type: 0x%08lX\n", eheader.e_type);
	LOGSTR1("Code entry: 0x%08lX\n", eheader.e_entry);
	LOGSTR1("Program header table offset: 0x%08lX\n", eheader.e_phoff);
	LOGSTR1("Program header size: 0x%08lX\n", eheader.e_phentsize);
	LOGSTR1("Number of program headers: 0x%08lX\n", eheader.e_phnum);
	LOGSTR1("Section header table offset: 0x%08lX\n", eheader.e_shoff);
	LOGSTR1("Section header size: 0x%08lX\n", eheader.e_shentsize);
	LOGSTR1("Number of section headers: 0x%08lX\n", eheader.e_shnum);
}

void log_mod_entry(HBLModInfo modinfo)
{
	LOGSTR0("\n->Module entry:\n");
	LOGSTR1("ID: 0x%08lX\n", modinfo.id);
	LOGSTR1("ELF type: 0x%08lX\n", modinfo.type);
	LOGSTR1("State: %d\n", modinfo.state);
	LOGSTR1("Size: 0x%08lX\n", modinfo.size);
	LOGSTR1("Text address: 0x%08lX\n", modinfo.text_addr);
	LOGSTR1("Entry point: 0x%08lX\n", modinfo.text_entry);
	LOGSTR1(".lib.stub address: 0x%08lX\n", modinfo.libstub_addr);
	LOGSTR1("GP: 0x%08lX\n", modinfo.gp);
	LOGSTR1("Path: %s\n", modinfo.path);
}

void log_program_header(Elf32_Phdr pheader)
{
	LOGSTR0("\n->Program header:\n");
	LOGSTR1("Segment type: 0x%08lX\n", pheader.p_type);
	LOGSTR1("Segment offset: 0x%08lX\n", pheader.p_offset);
	LOGSTR1("Virtual address for segment: 0x%08lX\n", pheader.p_vaddr);
	LOGSTR1("Physical address for segment: 0x%08lX\n", pheader.p_paddr);
	LOGSTR1("Segment image size in file: 0x%08lX\n", pheader.p_filesz);
	LOGSTR1("Segment image size in memory: 0x%08lX\n", pheader.p_memsz);
	LOGSTR1("Flags: 0x%08lX\n", pheader.p_flags);
	LOGSTR1("Alignment: 0x%08lX\n", pheader.p_align);
}

void log_elf_section_header(Elf32_Shdr shdr)
{	
	LOGSTR0("\n->Section header:\n");
	LOGSTR1("Name: %d\n", shdr.sh_name);
	LOGSTR1("Type: 0x%08lX\n", shdr.sh_type);
	LOGSTR1("Flags: 0x%08lX\n", shdr.sh_flags);
	LOGSTR1("Address: 0x%08lX\n", shdr.sh_addr);
	LOGSTR1("Offset in file: 0x%08lX\n", shdr.sh_offset);
	LOGSTR1("Size: 0x%08lX\n", shdr.sh_size);
}

// LOGSTRX implementations

void logstr0(const char* A)
{
    write_debug(A, NULL, 0);
}

void logstr1(const char* A, unsigned long B)			
{
    char buff[512];
    mysprintf1(buff, A, (unsigned long)B);
    write_debug(buff, NULL, 0);
}

void logstr2(const char* A, unsigned long B, unsigned long C)		
{
    char buff[512];
    mysprintf2(buff, A, (unsigned long)B, (unsigned long)C);
    write_debug(buff, NULL, 0);
}

void logstr3(const char* A, unsigned long B, unsigned long C, unsigned long D)		
{
    char buff[512];
    mysprintf3(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D);
    write_debug(buff, NULL, 0);
}

void logstr4(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E)
{
    char buff[512];
    mysprintf4(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E);
    write_debug(buff, NULL, 0);
}

void logstr5(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F)
{
    char buff[512];
    mysprintf8(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E, (unsigned long)F, 0, 0, 0);
    write_debug(buff, NULL, 0);
}

void logstr8(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I)
{
    char buff[512];
    mysprintf8(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E, (unsigned long)F, (unsigned long)G, (unsigned long)H, (unsigned long)I);
    write_debug(buff, NULL, 0);
}
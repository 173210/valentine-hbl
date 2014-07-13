#include <common/stubs/tables.h>
#include <common/debug.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/mod/elf.h>
#include <hbl/mod/modmgr.h>
#include <exploit_config.h>

#define DEBUG_PATH HBL_ROOT"DBGLOG"

void init_debug()
{
	SceUID fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
	sceIoClose(fd);
}

void write_debug_newline(const char *desc)
{
	write_debug(desc, "\n", 1);
}

// Debugging log function
void write_debug(const char *desc, void *val, size_t size)
{
	SceUID fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);

	if (desc != NULL) {
		int desc_len = strlen(desc);

		sceIoWrite(PSPLINK_OUT, desc, desc_len);
		sceIoWrite(fd, desc, desc_len);
	}
	if (val != NULL) {
		sceIoWrite(PSPLINK_OUT, val, size);
		sceIoWrite(fd, val, size);
	}

	sceIoClose(fd);
}

void exit_with_log(const char* desc, void* val, size_t size)
{
	write_debug(desc, val, size);
	sceKernelExitGame();
}

#ifdef DEBUG
void log_lib(tSceLibrary lib)
{
	logstr0("\n-->Library name: ");
	logstr0(lib.name);
	logstr0("\n");
	logstr1("--Calling mode: %d\n", lib.calling_mode);
	logstr1("--Total library exports: %d\n", lib.num_lib_exports);
	logstr1("--Known library exports: %d\n", lib.num_known_exports);
	logstr2("--Lowest NID/SYSCALL:  0x%08lX/0x%08lX\n", lib.lowest_nid, lib.lowest_syscall);
	logstr1("--Lowest index in file: %d\n", lib.lowest_index);
}

void log_modinfo(SceModuleInfo modinfo)
{
	logstr0("\n->Module information:\n");
	logstr1("Module name: %s\n", (int)modinfo.modname);
	logstr2("Module version: %d.%d\n", modinfo.modversion[0], modinfo.modversion[1]);
	logstr1("Module attributes: 0x%08lX\n", modinfo.modattribute);
	logstr1("Entry top: 0x%08lX\n", (int)modinfo.ent_top);
	logstr1("Stubs top: 0x%08lX\n", (int)modinfo.stub_top);
	logstr1("Stubs end: 0x%08lX\n", (int)modinfo.stub_end);
	logstr1("GP value: 0x%08lX\n", (int)modinfo.gp_value);
}

void log_elf_header(Elf32_Ehdr eheader)
{
	logstr0("\n->ELF header:\n");
	logstr1("Type: 0x%08lX\n", eheader.e_type);
	logstr1("Code entry: 0x%08lX\n", (int)eheader.e_entry);
	logstr1("Program header table offset: 0x%08lX\n", eheader.e_phoff);
	logstr1("Program header size: 0x%08lX\n", eheader.e_phentsize);
	logstr1("Number of program headers: 0x%08lX\n", eheader.e_phnum);
	logstr1("Section header table offset: 0x%08lX\n", eheader.e_shoff);
	logstr1("Section header size: 0x%08lX\n", eheader.e_shentsize);
	logstr1("Number of section headers: 0x%08lX\n", eheader.e_shnum);
}

void log_mod_entry(HBLModInfo modinfo)
{
	logstr0("\n->Module entry:\n");
	logstr1("ID: 0x%08lX\n", modinfo.id);
	logstr1("ELF type: 0x%08lX\n", modinfo.type);
	logstr1("State: %d\n", modinfo.state);
	logstr1("Size: 0x%08lX\n", modinfo.size);
	logstr1("Text address: 0x%08lX\n", (int)modinfo.text_addr);
	logstr1("Entry point: 0x%08lX\n", (int)modinfo.text_entry);
	logstr1(".lib.stub address: 0x%08lX\n", (int)modinfo.libstub_addr);
	logstr1("GP: 0x%08lX\n", (int) modinfo.gp);
	//logstr1("Path: %s\n", (u32) modinfo.path);
}

void log_program_header(Elf32_Phdr pheader)
{
	logstr0("\n->Program header:\n");
	logstr1("Segment type: 0x%08lX\n", pheader.p_type);
	logstr1("Segment offset: 0x%08lX\n", pheader.p_off);
	logstr1("Virtual address for segment: 0x%08lX\n", (int)pheader.p_vaddr);
	logstr1("Physical address for segment: 0x%08lX\n", (int)pheader.p_paddr);
	logstr1("Segment image size in file: 0x%08lX\n", pheader.p_filesz);
	logstr1("Segment image size in memory: 0x%08lX\n", pheader.p_memsz);
	logstr1("Flags: 0x%08lX\n", pheader.p_flags);
	logstr1("Alignment: 0x%08lX\n", pheader.p_align);
}

void log_elf_section_header(Elf32_Shdr shdr)
{
	logstr0("\n->Section header:\n");
	logstr1("Name: %d\n", shdr.sh_name);
	logstr1("Type: 0x%08lX\n", shdr.sh_type);
	logstr1("Flags: 0x%08lX\n", shdr.sh_flags);
	logstr1("Address: 0x%08lX\n", (int)shdr.sh_addr);
	logstr1("Offset in file: 0x%08lX\n", shdr.sh_offset);
	logstr1("Size: 0x%08lX\n", shdr.sh_size);
}
#endif


// LOGSTRX implementations

void logstr0(const char* A)
{
    write_debug(A, NULL, 0);
}

void logstr1(const char* A, unsigned long B)
{
    char debug_buff[512];
    mysprintf1(debug_buff, A, (unsigned long)B);
    write_debug(debug_buff, NULL, 0);
}

void logstr2(const char* A, unsigned long B, unsigned long C)
{
    char debug_buff[512];
    mysprintf2(debug_buff, A, (unsigned long)B, (unsigned long)C);
    write_debug(debug_buff, NULL, 0);
}

void logstr3(const char* A, unsigned long B, unsigned long C, unsigned long D)
{
    char debug_buff[512];
    mysprintf3(debug_buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D);
    write_debug(debug_buff, NULL, 0);
}

void logstr4(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E)
{
    char debug_buff[512];
    mysprintf4(debug_buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E);
    write_debug(debug_buff, NULL, 0);
}

void logstr5(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F)
{
    char debug_buff[512];
    mysprintf8(debug_buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E, (unsigned long)F, 0, 0, 0);
    write_debug(debug_buff, NULL, 0);
}

void logstr8(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I)
{
    char debug_buff[512];
    mysprintf8(debug_buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E, (unsigned long)F, (unsigned long)G, (unsigned long)H, (unsigned long)I);
    write_debug(debug_buff, NULL, 0);
}

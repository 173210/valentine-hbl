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
	logstr("\n-->Library name: ");
	logstr(lib.name);
	logstr("\n");
	logstr("--Calling mode: %d\n", lib.calling_mode);
	logstr("--Total library exports: %d\n", lib.num_lib_exports);
	logstr("--Known library exports: %d\n", lib.num_known_exports);
	logstr("--Lowest NID/SYSCALL:  0x%08X/0x%08X\n", lib.lowest_nid, lib.lowest_syscall);
	logstr("--Lowest index in file: %d\n", lib.lowest_index);
}

void log_modinfo(SceModuleInfo modinfo)
{
	logstr("\n->Module information:\n");
	logstr("Module name: %s\n", (int)modinfo.modname);
	logstr("Module version: %d.%d\n", modinfo.modversion[0], modinfo.modversion[1]);
	logstr("Module attributes: 0x%08X\n", modinfo.modattribute);
	logstr("Entry top: 0x%08X\n", (int)modinfo.ent_top);
	logstr("Stubs top: 0x%08X\n", (int)modinfo.stub_top);
	logstr("Stubs end: 0x%08X\n", (int)modinfo.stub_end);
	logstr("GP value: 0x%08X\n", (int)modinfo.gp_value);
}

void log_elf_header(Elf32_Ehdr eheader)
{
	logstr("\n->ELF header:\n");
	logstr("Type: 0x%08X\n", eheader.e_type);
	logstr("Code entry: 0x%08X\n", (int)eheader.e_entry);
	logstr("Program header table offset: 0x%08X\n", eheader.e_phoff);
	logstr("Program header size: 0x%08X\n", eheader.e_phentsize);
	logstr("Number of program headers: 0x%08X\n", eheader.e_phnum);
	logstr("Section header table offset: 0x%08X\n", eheader.e_shoff);
	logstr("Section header size: 0x%08X\n", eheader.e_shentsize);
	logstr("Number of section headers: 0x%08X\n", eheader.e_shnum);
}

void log_mod_entry(HBLModInfo modinfo)
{
	logstr("\n->Module entry:\n");
	logstr("ID: 0x%08X\n", modinfo.id);
	logstr("ELF type: 0x%08X\n", modinfo.type);
	logstr("State: %d\n", modinfo.state);
	logstr("Size: 0x%08X\n", modinfo.size);
	logstr("Text address: 0x%08X\n", (int)modinfo.text_addr);
	logstr("Entry point: 0x%08X\n", (int)modinfo.text_entry);
	logstr(".lib.stub address: 0x%08X\n", (int)modinfo.libstub_addr);
	logstr("GP: 0x%08X\n", (int) modinfo.gp);
	//logstr("Path: %s\n", (u32) modinfo.path);
}

void log_program_header(Elf32_Phdr pheader)
{
	logstr("\n->Program header:\n");
	logstr("Segment type: 0x%08X\n", pheader.p_type);
	logstr("Segment offset: 0x%08X\n", pheader.p_off);
	logstr("Virtual address for segment: 0x%08X\n", (int)pheader.p_vaddr);
	logstr("Physical address for segment: 0x%08X\n", (int)pheader.p_paddr);
	logstr("Segment image size in file: 0x%08X\n", pheader.p_filesz);
	logstr("Segment image size in memory: 0x%08X\n", pheader.p_memsz);
	logstr("Flags: 0x%08X\n", pheader.p_flags);
	logstr("Alignment: 0x%08X\n", pheader.p_align);
}

void log_elf_section_header(Elf32_Shdr shdr)
{
	logstr("\n->Section header:\n");
	logstr("Name: %d\n", shdr.sh_name);
	logstr("Type: 0x%08X\n", shdr.sh_type);
	logstr("Flags: 0x%08X\n", shdr.sh_flags);
	logstr("Address: 0x%08X\n", (int)shdr.sh_addr);
	logstr("Offset in file: 0x%08X\n", shdr.sh_offset);
	logstr("Size: 0x%08X\n", shdr.sh_size);
}
#endif


// LOGSTR implementation
void logstr(const char *fmt, ...)
{
	char debug_buf[512];
	va_list va;

	va_start(va, fmt);
	_vsprintf(debug_buf, fmt, va);
	va_end(va);
	write_debug(debug_buf, NULL, 0);
}


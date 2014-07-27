#include <common/stubs/tables.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/mod/elf.h>
#include <hbl/mod/modmgr.h>
#include <exploit_config.h>

#define DEBUG_PATH HBL_ROOT"DBGLOG"

void init_debug()
{
	globals->dbg_fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
}

void log_putc(int c)
{
	sceIoWrite(PSPLINK_OUT, &c, 1);
	sceIoWrite(globals->dbg_fd, &c, 1);
}

void log_puts(const char *s)
{
	size_t len = strlen(s);

	sceIoWrite(PSPLINK_OUT, s, len);
	sceIoWrite(globals->dbg_fd, s, len);
}

#ifdef DEBUG
void log_lib(tSceLibrary lib)
{
	log_printf("\n-->Library name: %s\n"
		"--Calling mode: %d\n"
		"--Total library exports: %d\n"
		"--Known library exports: %d\n"
		"--Lowest NID/SYSCALL:  0x%08X/0x%08X\n"
		"--Lowest index in file: %d\n",
		lib.name,
		lib.calling_mode,
		lib.num_lib_exports,
		lib.num_known_exports,
		lib.lowest_nid, lib.lowest_syscall,
		lib.lowest_index);
}

void log_modinfo(SceModuleInfo modinfo)
{
	log_printf("\n->Module information:\n"
		"Module name: %s\n"
		"Module version: %d.%d\n"
		"Module attributes: 0x%08X\n"
		"Entry top: 0x%08X\n"
		"Stubs top: 0x%08X\n"
		"Stubs end: 0x%08X\n"
		"GP value: 0x%08X\n",
		modinfo.modname,
		modinfo.modversion[0], modinfo.modversion[1],
		modinfo.modattribute,
		(int)modinfo.ent_top,
		(int)modinfo.stub_top,
		(int)modinfo.stub_end,
		(int)modinfo.gp_value);
}

void log_elf_header(Elf32_Ehdr eheader)
{
	log_printf("\n->ELF header:\n"
		"Type: 0x%08X\n"
		"Code entry: 0x%08X\n"
		"Program header table offset: 0x%08X\n"
		"Program header size: 0x%08X\n"
		"Number of program headers: 0x%08X\n"
		"Section header table offset: 0x%08X\n"
		"Section header size: 0x%08X\n"
		"Number of section headers: 0x%08X\n",
		eheader.e_type,
		(int)eheader.e_entry,
		eheader.e_phoff,
		eheader.e_phentsize,
		eheader.e_phnum,
		eheader.e_shoff,
		eheader.e_shentsize,
		eheader.e_shnum);
}

void log_mod_entry(HBLModInfo modinfo)
{
	log_printf("\n->Module entry:\n"
		"ID: 0x%08X\n"
		"ELF type: 0x%08X\n"
		"State: %d\n"
		"Size: 0x%08X\n"
		"Text address: 0x%08X\n"
		"Entry point: 0x%08X\n"
		".lib.stub address: 0x%08X\n"
		"GP: 0x%08X\n",
		modinfo.id,
		modinfo.type,
		modinfo.state,
		modinfo.size,
		(int)modinfo.text_addr,
		(int)modinfo.text_entry,
		(int)modinfo.libstub_addr,
		(int)modinfo.gp);
}

void log_program_header(Elf32_Phdr pheader)
{
	log_printf("\n->Program header:\n"
		"Segment type: 0x%08X\n"
		"Segment offset: 0x%08X\n"
		"Virtual address for segment: 0x%08X\n"
		"Physical address for segment: 0x%08X\n"
		"Segment image size in file: 0x%08X\n"
		"Segment image size in memory: 0x%08X\n"
		"Flags: 0x%08X\n"
		"Alignment: 0x%08X\n",
		pheader.p_type,
		pheader.p_off,
		(int)pheader.p_vaddr,
		(int)pheader.p_paddr,
		pheader.p_filesz,
		pheader.p_memsz,
		pheader.p_flags,
		pheader.p_align);
}

void log_elf_section_header(Elf32_Shdr shdr)
{
	log_printf("\n->Section header:\n"
		"Name: %d\n"
		"Type: 0x%08X\n"
		"Flags: 0x%08X\n"
		"Address: 0x%08X\n"
		"Offset in file: 0x%08X\n"
		"Size: 0x%08X\n",
		shdr.sh_name,
		shdr.sh_type,
		shdr.sh_flags,
		(int)shdr.sh_addr,
		shdr.sh_offset,
		shdr.sh_size);
}
#endif

// LOG_PRINTF implementation

void log_printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_vfprintf(log_putc, fmt, va);
	va_end(va);
}


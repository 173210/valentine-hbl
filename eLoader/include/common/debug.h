#ifndef ELOADER_DEBUG
#define ELOADER_DEBUG

#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <hbl/mod/elf.h>
#include <hbl/mod/modmgr.h>

#define STDOUT 1
#define PSPLINK_OUT 2

// Macro to make a BREAK instruction
#define MAKE_BREAK(n) ((((u32)n << 6) & 0x03ffffc0) | 0x0000000d)

//init the debug file
void init_debug();

// Write a string + data to debug path
void write_debug(const char* desc, void* val, size_t size);

// Write a debug string + carriage return to debug path
void write_debug_newline (const char *desc);

// Write debug and exit
void exit_with_log(const char *desc, void *val, size_t size);

void logstr(const char *fmt, ...);


#ifdef DEBUG
void log_lib(tSceLibrary lib);
void log_program_header(Elf32_Phdr pheader);
void log_modinfo(SceModuleInfo modinfo);
void log_elf_header(Elf32_Ehdr eheader);
void log_mod_entry(HBLModInfo modinfo);
void log_elf_section_header(Elf32_Shdr shdr);
#define DEBUG_PRINT(a,b,c) write_debug(a,b,c)
#define DEBUG_PRINT_NL(a) write_debug_newline(a)
#define LOGSTR(...) logstr(__VA_ARGS__)
#define LOGLIB(a) log_lib(a)
#define LOGMODINFO(a) log_modinfo(a)
#define LOGELFHEADER(a) log_elf_header(a)
#define LOGMODENTRY(a) log_mod_entry(a)
#define LOGELFPROGHEADER(a) log_program_header(a)
#define LOGELFSECHEADER(a) log_elf_section_header(a)

#else
#define DEBUG_PRINT(a,b,c) {}
#define DEBUG_PRINT_NL(a) {}
#define LOGSTR(...) {}
#define LOGLIB(a) {}
#define LOGMODINFO(a) {}
#define LOGELFHEADER(a) {}
#define LOGMODENTRY(a) {}
#define LOGELFPROGHEADER(a) {}
#define LOGELFSECHEADER(a) {}
#endif

#ifdef NID_DEBUG
#define NID_LOGSTR(...) logstr(__VA_ARGS__)

#else
#define NID_LOGSTR(...) {}
#endif

#endif

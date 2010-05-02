#ifndef ELOADER_DEBUG
#define ELOADER_DEBUG

#include "sdk.h"
#include "eloader.h"

#define DEBUG_PATH HBL_ROOT"dbglog"
#define STDOUT 1
#define PSPLINK_OUT 2

// Macro to make a BREAK instruction
#define MAKE_BREAK(n) ((((u32)n << 6) & 0x03ffffc0) | 0x0000000d)

#ifdef DEBUG
extern SceUID dbglog;
extern u32 aux;
#endif

//init the debug file
void init_debug();

// Write a string + data to debug path
void write_debug(const char* description, void* value, unsigned int size);

// Write a debug string + carriage return to debug path
void write_debug_newline (const char* description);

// Write debug and exit
void exit_with_log(const char* description, void* value, unsigned int size);

void logstr8(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I);
void logstr5(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F);
void logstr4(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E);
void logstr3(const char* A, unsigned long B, unsigned long C, unsigned long D);
void logstr2(const char* A, unsigned long B, unsigned long C);
void logstr1(const char* A, unsigned long B);
void logstr0(const char* A);

#ifdef DEBUG
#define DEBUG_PRINT(a,b,c) write_debug(a,b,c);
#define DEBUG_PRINT_NL(a) write_debug_newline(a);
#define LOGSTR8(a,b,c,d,e,f,g,h,i) logstr8(a,b,c,d,e,f,g,h,i);
#define LOGSTR5(a,b,c,d,e,f) logstr5(a,b,c,d,e,f);
#define LOGSTR4(a,b,c,d,e) logstr4(a,b,c,d,e);
#define LOGSTR3(a,b,c,d) logstr3(a,b,c,d);
#define LOGSTR2(a,b,c) logstr2(a,b,c);
#define LOGSTR1(a,b) logstr1(a,b);
#define LOGSTR0(a) logstr0(a);
#define LOGLIB(a) log_library(a);
#define LOGMODINFO(a) log_modinfo(a);
#define LOGELFHEADER(a) log_elf_header(a);
#define LOGMODENTRY(a) log_mod_entry(a);
#define LOGELFPROGHEADER(a) log_program_header(a);
#define LOGELFSECHEADER(a) log_elf_section_header(a);

#else
#define DEBUG_PRINT(a,b,c) {};
#define DEBUG_PRINT_NL(a) {};
#define LOGSTR8(a,b,c,d,e,f,g,h,i){};
#define LOGSTR5(a,b,c,d,e,f) {};
#define LOGSTR4(a,b,c,d,e) {};
#define LOGSTR3(a,b,c,d) {};
#define LOGSTR2(a,b,c) {};
#define LOGSTR1(a,b) {};
#define LOGSTR0(a) {};
#define LOGLIB(a) {};
#define LOGMODINFO(a) {};
#define LOGELFHEADER(a) {};
#define LOGMODENTRY(a) {};
#define LOGELFPROGHEADER(a) {};
#define LOGELFSECHEADER {};
#endif

#ifdef NID_DEBUG
#define NID_LOGSTR8(a,b,c,d,e,f,g,h,i) logstr8(a,b,c,d,e,f,g,h,i);
#define NID_LOGSTR5(a,b,c,d,e,f) logstr5(a,b,c,d,e,f);
#define NID_LOGSTR4(a,b,c,d,e) logstr4(a,b,c,d,e);
#define NID_LOGSTR3(a,b,c,d) logstr3(a,b,c,d);
#define NID_LOGSTR2(a,b,c) logstr2(a,b,c);
#define NID_LOGSTR1(a,b) logstr1(a,b);
#define NID_LOGSTR0(a) logstr0(a);

#else
#define NID_LOGSTR8(a,b,c,d,e,f,g,h,i){};
#define NID_LOGSTR5(a,b,c,d,e,f) {};
#define NID_LOGSTR4(a,b,c,d,e) {};
#define NID_LOGSTR3(a,b,c,d) {};
#define NID_LOGSTR2(a,b,c) {};
#define NID_LOGSTR1(a,b) {};
#define NID_LOGSTR0(a) {};
#define LOGLIB(a) {};
#endif

#endif
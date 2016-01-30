#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspge.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <psputility.h>
#include <pspumd.h>
#include <psptypes.h>
#include <pspimpose_driver.h>
#include <pspsuspend.h>
#include <psppower.h>

#ifndef VAL_SDK_H
#define VAL_SDK_H

#define _STROF(str) #str
#define STROF(str) _STROF(str)

#define MAJOR_VER 0
#define MINOR_VER 11
#define VER_STR STROF(MAJOR_VER) "." STROF(MINOR_VER)

/* Declare a module.  This must be specified in the source of a library or executable. */
#define HBL_MODULE_INFO(name, attributes, major_version, minor_version) \
	__asm__ (                                                       \
	"    .set push\n"                                               \
	"    .section .lib.ent.top, \"a\", @progbits\n"                 \
	"    .align 2\n"                                                \
	"    .word 0\n"                                                 \
	"    .global __lib_ent_top\n"                                   \
	"__lib_ent_top:\n"                                              \
	"    .section .lib.ent.btm, \"a\", @progbits\n"                 \
	"    .align 2\n"                                                \
	"    .global __lib_ent_bottom\n"                                \
	"__lib_ent_bottom:\n"                                           \
	"    .word 0\n"                                                 \
	"    .section .lib.stub.top, \"a\", @progbits\n"                \
	"    .align 2\n"                                                \
	"    .word 0\n"                                                 \
	"    .global __lib_stub_top\n"                                  \
	"__lib_stub_top:\n"                                             \
	"    .section .lib.stub.btm, \"a\", @progbits\n"                \
	"    .align 2\n"                                                \
	"    .global __lib_stub_bottom\n"                               \
	"__lib_stub_bottom:\n"                                          \
	"    .word 0\n"                                                 \
	"    .set pop\n"                                                \
	"    .text\n"                                                   \
	);                                                              \
	extern char __lib_ent_top[], __lib_ent_bottom[];                \
	extern char __lib_stub_top[], __lib_stub_bottom[];              \
	SceModuleInfo module_info                                       \
		__attribute__((section(".rodata.sceModuleInfo"),        \
			       aligned(16), unused)) = {                \
	  attributes, { minor_version, major_version }, name, 0, _gp,   \
	  __lib_ent_top, __lib_ent_bottom,                              \
	  __lib_stub_top, __lib_stub_bottom                             \
	}

// Typedefs
typedef unsigned char byte;

/*
 * Code
 */
#define REG_ZR (0)
#define REG_V0 (2)
#define REG_A0 (4)
#define REG_RA (31)

#define J_OPCODE (0x08000000)
#define JR_OPCODE (0x00000008)
#define LUI_OPCODE (0x3C000000)
#define SLL_OPCODE (0x00000000)
#define SYSCALL_OPCODE (0x0000000C)

#define J_ASM(t) (J_OPCODE | (u32)(t) >> 2)
#define JR_ASM(r) (JR_OPCODE | (u8)(r) << 21)
#define LUI_ASM(r, n) (LUI_OPCODE | (u8)(r) << 16 | (u16)(n))
#define SLL_ASM(d, t, s) (SLL_OPCODE | (u8)(t) << 16 | (u8)(d) << 11 | (u8)(s) << 6)
#define SYSCALL_ASM(c) (SYSCALL_OPCODE | (u32)(c) << 6)
#define NOP_ASM SLL_ASM(REG_ZR, REG_ZR, 0)

// This marks libraries that are not yet linked
#define SYSCALL_IMPORT_NOT_RESOLVED_YET 0x15

#define isImported(f) (((int *)(f))[1])

// Macro to get the syscall number
#define GET_SYSCALL_NUMBER(sys) ((u32)(sys) >> 6)

//
// Memory
//
#define GAME_MEMORY_START 0x08800000
#define GAME_MEMORY_SIZE 0x1800000

// Max chars on module name
#define MAX_MODULE_NAME_LENGTH 30

// Max chars on library name
#define MAX_LIB_NAME_LEN 30

/*
#ifndef ULONG
#define ULONG unsigned long
#endif
*/

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

//Should be defined somewhere in the sdk ???
typedef struct {
        int count;
        SceUID thread;
        int attr;
        int numWaitThreads;
        SceUID uid;
        int pad[3];
} SceLwMutexWorkarea;

int sceKernelDeleteLwMutex(SceLwMutexWorkarea *workarea);
int sceKernelExitGameWithStatus(int status);
SceUID sceKernelGetModuleIdByAddress(u32 address);

#endif


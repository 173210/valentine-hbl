#ifndef GLOBALS_H
#define GLOBALS_H

// We create a "fake" structure holding all of them in a safe memory zone.

#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <common/malloc.h>
#include <loader.h>

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
/* This can't be used twice on the same line so ensure if using in headers
 * that the headers are not included twice (by wrapping in #ifndef...#endif)
 * Note it doesn't cause an issue when used on same line of separate modules
 * compiled with gcc -combine -fwhole-program.  */
#define STATIC_ASSERT(e) \
	enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

// Max libraries to consider
#define MAX_LIBRARIES 64

// Size of NID-to-call table
#define NID_TABLE_SIZE 512

typedef struct
{
#ifndef VITA
	// firmware and model
	int module_sdk_version;
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	int psp_go;
	//tables.c
	int syscalls_known;
#endif
#endif
	int nid_num;
	tNIDResolver nid_table[NID_TABLE_SIZE];
	int lib_num;
	tSceLibrary lib_table[MAX_LIBRARIES];
	//malloc.c
	SceUID blockids[MAX_ALLOCS]; /* Blocks */
} tGlobals;

#define GLOBALS_ADDR 0x10200
#define globals ((tGlobals *)GLOBALS_ADDR)

//This should fail with a weird error at compile time if globals is too big
STATIC_ASSERT(GLOBALS_ADDR + sizeof(tGlobals) <= 0x14000);
STATIC_ASSERT(HBL_STUBS_START + NUM_HBL_IMPORTS * 2 * sizeof(int) <= GLOBALS_ADDR);

#endif


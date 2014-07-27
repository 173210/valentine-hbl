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


typedef struct
{
#ifndef VITA
	// firmware and model
	int fw_ver;
	int psp_model;
#else
	//tables.c
	int syscalls_known;
#endif
	HBLNIDTable nid_table;
	HBLLibTable lib_table;
	//malloc.c
	SceUID blockids[MAX_ALLOCS]; /* Blocks */
	SceUID dbg_fd;
} tGlobals;

#define GLOBALS_ADDR 0x10200
#define globals ((tGlobals *)GLOBALS_ADDR)

//This should fail with a weird error at compile time if globals is too big
STATIC_ASSERT(GLOBALS_ADDR + sizeof(tGlobals) <= 0x14000);
STATIC_ASSERT(HBL_STUBS_START + NUM_HBL_IMPORTS * 2 * sizeof(int) <= GLOBALS_ADDR);

//inits global variables. This needs to be called once and only once, preferably at the start of the HBL
void init_globals();

#endif


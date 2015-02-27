#ifndef GLOBALS_H
#define GLOBALS_H

// We create a "fake" structure holding all of them in a safe memory zone.

#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <config.h>

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
/* This can't be used twice on the same line so ensure if using in headers
 * that the headers are not included twice (by wrapping in #ifndef...#endif)
 * Note it doesn't cause an issue when used on same line of separate modules
 * compiled with gcc -combine -fwhole-program.  */
#define STATIC_ASSERT(e) \
	enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

#define MAX_OPEN_DIR_VITA 10

// Max libraries to consider
#define MAX_LIBRARIES 64

// Size of NID-to-call table
#define NID_TABLE_SIZE 512

typedef struct
{
	int chdir_ok; //1 if sceIoChdir is correctly estimated, 0 otherwise
	SceUID memSema; 		
 	SceUID thSema; 		
 	SceUID cbSema; 		
 	SceUID audioSema; 		
 	SceUID ioSema;
	int dirFix[MAX_OPEN_DIR_VITA][2];
	// firmware and model
	int isEmu;
	int module_sdk_version;
	int nid_num;
	tNIDResolver nid_table[NID_TABLE_SIZE];
	int lib_num;
	tSceLibrary lib_table[MAX_LIBRARIES];
} tGlobals;

#define GLOBALS_ADDR 0x10200
#define globals ((tGlobals *)GLOBALS_ADDR)

//This should fail with a weird error at compile time if globals is too big
STATIC_ASSERT(GLOBALS_ADDR + sizeof(tGlobals) <= 0x14000);

#endif


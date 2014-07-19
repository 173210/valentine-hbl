#ifndef GLOBALS_H
#define GLOBALS_H

/*
Globals do not get allocated correctly in the HBL environment.
for this reason, we create a "fake" structure holding all of them
in a safe memory zone. If you need globals, it is better to have them here
*/

#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <common/malloc.h>
#include <hbl/mod/modmgr.h>
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

#define GLOBALS_ADDR 0x10200

#define MAX_OS_ALLOCS 400
#define SIZE_THREAD_TRACKING_ARRAY 20
#define SIZE_IO_TRACKING_ARRAY 10
#define MAX_CALLBACKS 20
#define MAX_OPEN_DIR_VITA 10


/*****************************************************************************/
/* Menu API definition By noobz. We'll pass the address of this struct to the */
/* menu, and it can use it if it understands it.                             */
/*                                                                           */
/* We guarantee that any future revisions to the struct will increment       */
/* the version number, and just add new fields to the end.  Existing fields  */
/* must not be changed, for backwards-compatibility.                         */
/*****************************************************************************/
typedef struct
{
	long api_ver;
	char credits[32];
	char ver_name[32];
	char *bg_fname; // set to NULL to let menu choose.
	char *fname; // The menu will write the selected filename there
}	tMenuApi;

typedef struct
{
	// firmware and model
#ifndef VITA
	int fw_ver;
	int psp_model;
#endif
	//eLoader.c
	int menu_enabled;
	int exit_cb_called;
	//hook.c
	char *mod_chdir; //cwd of the currently running module
	int pllfreq; // currentpll frequency
	int cpufreq; //current cpu frequency
	int busfreq; //current bus frequency
	int chdir_ok; //1 if sceIoChdir is correctly estimated, 0 otherwise
	SceUID running_th[SIZE_THREAD_TRACKING_ARRAY];
	SceUID pending_th[SIZE_THREAD_TRACKING_ARRAY];
	SceUID exited_th[SIZE_THREAD_TRACKING_ARRAY];
	int num_pend_th;
	int num_run_th;
	int num_exit_th;
	SceUID openFiles[SIZE_IO_TRACKING_ARRAY];
	int numOpenFiles;
	SceUID osAllocs[MAX_OS_ALLOCS];
	int osAllocNum;
	SceKernelCallbackFunction cbfuncs[MAX_CALLBACKS];
	int cbids[MAX_CALLBACKS];
	SceKernelCallbackFunction exitcb;
	int cbcount;
	int calledexitcb;
	SceUID memSema;
	SceUID thSema;
	SceUID cbSema;
	SceUID audioSema;
	SceUID ioSema;
	int audio_th[8];
	int cur_ch_id;
#ifdef VITA
	int dirLen;
	int dirFix[MAX_OPEN_DIR_VITA][2];
#else
	//tables.c
	int syscalls_known;
#endif
	HBLNIDTable nid_table;
	HBLLibTable lib_table;
	//settings.c
	int override_sceIoMkdir;
	int override_sceCtrlPeekBufferPositive;
	int return_to_xmb_on_exit;
	u32 force_exit_buttons;
	//malloc.c
	u32 nblocks; //Number of allocated blocks
	SceUID blockids[MAX_ALLOCS]; /* Blocks */
	//modmgr.c
	HBLModTable mod_table;
	tMenuApi menu_api;
	char hb_fname[512];
	char menupath[128];
} tGlobals;

static tGlobals *globals = (tGlobals *)GLOBALS_ADDR;


//This should fail with a weird error at compile time if globals is too big
//We also have a runtime check so you can comment out this line if you don't understand its meaning
STATIC_ASSERT(GLOBALS_ADDR + sizeof(tGlobals) <= 0x14000);
STATIC_ASSERT(HBL_STUBS_START + NUM_HBL_IMPORTS * 2 * sizeof(int) <= GLOBALS_ADDR);

//inits global variables. This needs to be called once and only once, preferably at the start of the HBL
void init_globals();

// resets Vita's dir-fix globals
void reset_vita_dirs();

#endif


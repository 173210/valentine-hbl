#ifndef GLOBALS_H
#define GLOBALS_H

/*
Globals do not get allocated correctly in the HBL environment.
for this reason, we create a "fake" structure holding all of them
in a safe memory zone. If you need globals, it is better to have them here
*/

#include "loader.h"

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
  /* This can't be used twice on the same line so ensure if using in headers
   * that the headers are not included twice (by wrapping in #ifndef...#endif)
   * Note it doesn't cause an issue when used on same line of separate modules
   * compiled with gcc -combine -fwhole-program.  */
  #define STATIC_ASSERT(e) \
    enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

#define GLOBALS_ADDR 0x10200  //This is in the scratchpad!!!
#include "sdk.h"
#include "malloc.h"
#include "modmgr.h"
#include "tables.h"

#define MAX_OS_ALLOCS   400
#define SIZE_THREAD_TRACKING_ARRAY  20
#define MAX_CALLBACKS 20


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
	unsigned long        APIVersion;
	char       Credits[32];
	char       VersionName[32];
	char       *BackgroundFilename;   // set to NULL to let menu choose.
    char        * filename;   // The menu will write the selected filename there
}	tMenuApi;

typedef struct 
{
    //uids
    SceUID hbl_block_uid;
    u32 hbl_block_addr;
    //eLoader.c
    int menu_enabled;
	int exit_callback_called;
    //hook.c
    char* module_chdir; //cwd of the currently running module
    int pllfreq; // currentpll frequency
    int cpufreq; //current cpu frequency
    int busfreq; //current bus frequency
    int chdir_ok; //1 if sceIoChdir is correctly estimated, 0 otherwise
    SceUID runningThreads[SIZE_THREAD_TRACKING_ARRAY];
    SceUID pendingThreads[SIZE_THREAD_TRACKING_ARRAY];
    SceUID exitedThreads[SIZE_THREAD_TRACKING_ARRAY];
    u32 numPendThreads;
    u32 numRunThreads;
    u32 numExitThreads;
    SceUID osAllocs[MAX_OS_ALLOCS];
    u32    osAllocNum;
    SceKernelCallbackFunction callbackfuncs[MAX_CALLBACKS];
    int                       callbackids[MAX_CALLBACKS];
    SceKernelCallbackFunction exitcallback;
    int callbackcount;
    int calledexitcb;  
    SceUID memSema;
    SceUID thrSema;
    SceUID cbSema;
    SceUID audioSema;
    int audio_threads[8];
    int curr_channel_id;
    //settings.c
    int override_sceIoMkdir;
    int override_sceCtrlPeekBufferPositive;
    int return_to_xmb_on_exit;
    u32 force_exit_buttons;
    //malloc.c
    u32 nblocks; //Number of allocated blocks
    HBLMemBlock block[MAX_ALLOCS]; /* Blocks */
    //modmgr.c
    HBLModTable mod_table;
    tMenuApi menu_api;
    char hb_filename[512];
    char menupath[128];
    //tables.c
    HBLNIDTable nid_table;
    HBLLibTable library_table;
    int syscalls_known;
	int syscalls_from_p5;
} tGlobals;


//This should fail with a weird error at compile time if globals is too big
//We also have a runtime check so you can comment out this line if you don't understand its meaning
STATIC_ASSERT( (GLOBALS_ADDR + sizeof(tGlobals)) <= 0x14000);
STATIC_ASSERT (HBL_STUBS_START + NUM_HBL_IMPORTS*2*sizeof(u32) <= GLOBALS_ADDR);

//gets a pointer to the global variables
tGlobals * get_globals();

//inits global variables. This needs to be called once and only once, preferably at the start of the HBL
void init_globals();

#endif


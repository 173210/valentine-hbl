/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include <common/stubs/runtime.h>
#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/scr.h>
#include <common/config.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/malloc.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <exploit_config.h>
#include <loader.h>
#include <svnversion.h>

#include <loader/globals.c>
#include <loader/p5_stubs.c>

#ifdef DEACTIVATE_SYSCALL_ESTIMATION
#define DISABLE_KERNEL_DUMP
#endif

#define FIRST_LOG "Loader running\n"

#ifdef RESET_HOME_LANG
// Reset language and button assignment for the HOME screen to system defaults
static void resetHomeSettings()
{
	int lang;
	int button;

	// Get system language, default to English on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &lang) < 0)
		lang = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;

	// Get button assignment, default to X = Enter on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &button) < 0)
		button = 1; // X = Enter

	sceImposeSetLanguageMode(lang, button);
}
#endif


#ifdef GAME_PRELOAD_FREEMEM
static void PreloadFreeMem()
{
	unsigned i;

	int blockids[] = GAME_PRELOAD_FREEMEM;

	for(i = 0; i < sizeof(blockids) / sizeof(int); i++) {
		int ret = sceKernelFreePartitionMemory(*(SceUID *)blockids[i]);
		if (ret < 0)
			dbg_printf("--> ERROR 0x%08X FREEING PARTITON MEMORY ID 0x%08X\n", ret, *(SceUID *)blockids[i]);
	}
}
#endif

#ifdef DELETE_UNKNOWN_FPL
static void FreeFpl()
{
	SceUID uid;

	dbg_printf("loader.c: FreeFpl\n");

	for (uid =  0x03000000; uid < 0xE0000000; uid++)
        	if (sceKernelDeleteFpl(uid) >= 0) {
			dbg_printf("Succesfully Deleted FPL ID 0x%08X\n", uid);
			return;
        	}
}
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
static void CloseFiles()
{
	SceUID fd;
	int ret;

	dbg_printf("memory.c:CloseFiles\n");

	for (fd = 0; fd < 8; fd++) {
		ret = sceIoClose(fd);
		if (ret != SCE_KERNEL_ERROR_BADF)
			dbg_printf("tried closing file %d, result 0x%08X\n", fd, ret);
	}
}
#endif

static void (* run_eloader)();

// Loads HBL to memory
static void load_hbl(SceUID hbl_file)
{
	long file_size;
	int ret;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);

	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	// Allocate memory for HBL
#ifdef GAME_PRELOAD_FREEMEM
	PreloadFreeMem();
#endif
#ifdef FPL_EARLY_LOAD_ADDR_LIST
	//early memory cleanup to be able to load HBL at a convenient place
	dbg_printf("loader.c: PreloadFreeFPL\n");

	int uids[] = FPL_EARLY_LOAD_ADDR_LIST;

	for(i = 0; i < sizeof(uids) / sizeof(int); i++)
		if (sceKernelDeleteFpl(*(SceUID *)uids[i]) < 0)
			dbg_printf("--> ERROR 0x%08X Deleting FPL ID 0x%08X\n", ret, *(SceUID *)uids[i]);
#endif
#ifdef DONT_ALLOC_HBL_MEM
	run_eloader = (void *)HBL_LOAD_ADDR;
#else
	SceUID HBL_block = sceKernelAllocPartitionMemory(
		2, "Valentine", PSP_SMEM_Addr, file_size, (void *)HBL_LOAD_ADDR);
	if(HBL_block < 0) {
		scr_printf(" ERROR ALLOCATING HBL MEMORY 0x%08X\n", HBL_block);
		sceKernelExitGame();
	}
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
#endif

	// Load HBL to allocated memory
	ret = sceIoRead(hbl_file, (void *)run_eloader, file_size);
	if (ret < 0) {
		scr_printf(" ERROR READING HBL 0x%08X\n", ret);
		sceKernelExitGame();
	}

	sceIoClose(hbl_file);

	dbg_printf("HBL loaded to allocated memory @ 0x%08X\n", (int)run_eloader);

	dbg_printf("Resolving HBL stubs\n");
	resolve_stubs();

	// Commit changes to RAM
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
	sceKernelDcacheWritebackRange(run_eloader, file_size);
	sceKernelDcacheWritebackRange((void *)HBL_STUBS_START, NUM_HBL_IMPORTS * 2 * 4);
#else
	sceKernelDcacheWritebackAll();
#endif
#ifdef HOOK_sceKernelIcacheInvalidateAll_WITH_sceKernelIcacheInvalidateRange
	sceKernelIcacheInvalidateRange(run_eloader, file_size);
	sceKernelIcacheInvalidateRange((void *)HBL_STUBS_START, NUM_HBL_IMPORTS * 2 * 4);
#elif !defined(HOOK_sceKernelIcacheInvalidateAll_WITH_dummy)
	sceKernelIcacheInvalidateAll();
#endif
}

#ifndef DISABLE_KERNEL_DUMP
// Dumps kmem
static void get_kmem_dump()
{
	SceUID fd = sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_WRONLY, 0777);
	sceIoWrite(fd, (void*)0x08000000, 0x400000);
	sceIoClose(fd);
}
#endif

#if defined(DEBUG) || defined(FORCE_FIRST_LOG)
static void dbg_init()
{
	SceUID fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);

	sceIoWrite(PSPLINK_OUT, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));
	sceIoWrite(fd, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));

	sceIoClose(fd);
}
#endif

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID hbl_file;
	int num_nids;

#if defined(DEBUG) || defined(FORCE_FIRST_LOG)
	//reset the contents of the debug file;
	dbg_init();
#endif

	//init global variables
	init_globals();

	scr_init();
	scr_puts("Starting HBL R"SVNVERSION" http://code.google.com/p/valentine-hbl\n");
#ifdef DEBUG
#ifdef NID_DEBUG
	scr_puts("DEBUG version (+NIDS)\n");
#else
	scr_puts("DEBUG version\n");
#endif
#else
	scr_puts_col("DO NOT POST LOG FILES OR BUG REPORTS FOR THIS VERSION!!!\n", 0x000000FF);
#endif

#ifdef PRE_LOADER_EXEC
	PRE_LOADER_EXEC
#endif

#ifdef RESET_HOME_LANG
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeSettings();
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
	CloseFiles();
#endif

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	if (globals->psp_go) {
		scr_puts("PSP Go Detected\n");
#ifndef DISABLE_KERNEL_DUMP
		// If PSPGo on 6.20+, do a kmem dump
		if (globals->module_sdk_version >= 0x06020010)
			get_kmem_dump();
#endif
	}
#endif

	// Build NID table
	scr_puts("Building NIDs table with game\n");
	num_nids = p2_add_stubs();
	scr_puts("Building NIDs table with savedata utility\n");
	num_nids += p5_add_stubs();
	dbg_printf("NUM NIDS: %d\n", num_nids);

	if(num_nids <= 0) {
		scr_puts("No Nids ???\n");
		sceKernelExitGame();
	}

	hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777);
	if (hbl_file < 0) {
		scr_printf(" FAILED TO LOAD HBL 0x%08X\n", hbl_file);
		sceKernelExitGame();
	}

	dbg_printf("Loading HBL\n");
	load_hbl(hbl_file);
	dbg_printf("Running eLoader\n");
	run_eloader();
}

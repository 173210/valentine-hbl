/* Half Byte Loader loader :P */
/* This initializes and loads HBL on memory */

#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/cache.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <loader/freemem.h>
#include <loader/runtime.h>
#include <exploit_config.h>
#include <svnversion.h>

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

static void (* run_eloader)();

// Loads HBL to memory
static void load_hbl()
{
	SceUID hbl_file;
	int ret;

	hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777);
	if (hbl_file < 0) {
		scr_printf(" FAILED TO LOAD HBL 0x%08X\n", hbl_file);
		sceKernelExitGame();
	}

	// Allocate memory for HBL
#ifdef DONT_ALLOC_HBL_MEM
	run_eloader = (void *)HBL_LOAD_ADDR;
#else
	SceUID HBL_block = sceKernelAllocPartitionMemory(
		2, "Valentine", PSP_SMEM_Addr, HBL_SIZE, (void *)HBL_LOAD_ADDR);
	if(HBL_block < 0) {
		scr_printf(" ERROR ALLOCATING HBL MEMORY 0x%08X\n", HBL_block);
		sceKernelExitGame();
	}
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
#endif
	dbg_printf("Loading HBL...\n");
	// Load HBL to allocated memory
	ret = sceIoRead(hbl_file, (void *)run_eloader, HBL_SIZE);
	if (ret < 0) {
		scr_printf(" ERROR READING HBL 0x%08X\n", ret);
		sceKernelExitGame();
	}

	sceIoClose(hbl_file);

	dbg_printf("HBL loaded to allocated memory @ 0x%08X\n", (int)run_eloader);

	// Commit changes to RAM
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
	sceKernelDcacheWritebackRange(run_eloader, ret);
#else
	sceKernelDcacheWritebackAll();
#endif
	hblIcacheFillRange(run_eloader, (void *)run_eloader + ret);
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
//reset the contents of the debug file;
static void log_init()
{
	SceUID fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);

	sceIoWrite(PSPLINK_OUT, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));
	sceIoWrite(fd, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));

	sceIoClose(fd);
}
#endif

#ifdef DEBUG
static void dbg_init()
{
	tStubEntry *entry;
	int *hbl_stubs = (void *)(HBL_STUBS_ADDR | 0x40000000);
	int i, j;

	for (i = 0; i < HBL_STUBS_NUM; i++)
		hbl_stubs[i * 2] = BREAK_OPCODE(0);
	hblIcacheFillRange((void *)HBL_STUBS_ADDR, (void *)HBL_STUBS_ADDR + HBL_STUBS_NUM * 8);

	i = 0;
	for (entry = (tStubEntry *)0x08800000;
		(int)entry < 0x0A000000;
		entry = (tStubEntry *)((int)entry + 4)) {

		while (elf_check_stub_entry(entry)) {
			if (!strcmp(entry->lib_name, "IoFileMgrForUser"))
				for (j = 0; j < entry->stub_size; j++) {
					switch (((int *)entry->nid_p)[j]) {
						case 0x109F50BC:
							memcpy((void *)((int)sceIoOpen | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
						case 0x810C4BC3:
							memcpy((void *)((int)sceIoClose | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
						case 0x42EC03AC:
							memcpy((void *)((int)sceIoWrite | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
					}
					if (i >= 3)
						return;
				}

			entry++;
		}
	}

	hblIcacheFillRange(sceIoOpen, (void *)sceIoOpen + 8);
	hblIcacheFillRange(sceIoClose, (void *)sceIoClose + 8);
	hblIcacheFillRange(sceIoWrite, (void *)sceIoWrite + 8);
}
#endif

#ifdef HOOK_CHDIR_AND_FRIENDS
static int test_sceIoChdir()
{
#ifndef CHDIR_CRASH
	SceUID fd;

	sceIoChdir(HBL_ROOT);

	fd = sceIoOpen(HBL_BIN, PSP_O_RDONLY, 0777);
	if (fd > 0) {
		sceIoClose(fd);

		return 1;
	}
#endif
	return 0;
}
#endif

#ifndef VITA
// "cache" for the firmware version
// 0 means unknown

// cache for the psp model
// 0 means unknown
// 1 is PSPGO


// New method by neur0n to get the firmware version from the
// module_sdk_version export of sceKernelLibrary
// http://wololo.net/talk/viewtopic.php?f=4&t=128
static void get_module_sdk_version()
{
	int i, cnt;

	SceLibraryEntryTable *tbl = *(SceLibraryEntryTable **)
		(findstr("sceKernelLibrary", (void *)0x08800300, 0x00001000) + 32);

	cnt = tbl->vstubcount + tbl->stubcount;

	// dbg_printf("cnt is 0x%08X \n", cnt);

	for (i = 0; ((int *)tbl->entrytable)[i] != 0x11B97506; i++)
		if (i >= cnt) {
			dbg_printf("Warning: Cannot find module_sdk_version\n");
			return;
		}

	globals->module_sdk_version = *((int **)tbl->entrytable)[i + cnt];

	dbg_printf("Detected firmware version is 0x%08X\n", globals->module_sdk_version);
}

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
static void detect_psp_go()
{
	// This call will always fail, but with a different error code depending on the model
	// Check for "No such device" error
	globals->psp_go = sceIoOpen("ef0:/", 1, 0777) == SCE_KERNEL_ERROR_NODEV;
}

static void detect_syscall()
{
	int i;
	short syscalls_known_fw[] = SYSCALLS_KNOWN_FW;
	short syscalls_known_go_fw[] = SYSCALLS_KNOWN_GO_FW;

	short *fw_array;
	int fw_array_size;

	if (globals->psp_go) {
		fw_array = syscalls_known_go_fw;
		fw_array_size = sizeof(syscalls_known_go_fw) / sizeof(short);
	} else {
		fw_array = syscalls_known_fw;
		fw_array_size = sizeof(syscalls_known_fw) / sizeof(short);
	}

	for (i = 0; i < fw_array_size; i++) {
		if (fw_array[i] == globals->module_sdk_version) {
			globals->syscalls_known = 1;
			break;
		}
	}
}
#endif
#endif

static void hook_init()
{
#ifdef VITA
	int i, j;

	for (i = 0; i < MAX_OPEN_DIR_VITA; i++)
		for (j = 0; j < 2; j++)
 			globals->dirFix[i][j] = -1;
#endif
#ifdef HOOK_CHDIR_AND_FRIENDS
        globals->chdir_ok = test_sceIoChdir();
#endif
	globals->memSema = sceKernelCreateSema("hblmemsema", 0, 1, 1, 0);
	globals->thSema = sceKernelCreateSema("hblthSema", 0, 1, 1, 0);
	globals->cbSema = sceKernelCreateSema("hblcbsema", 0, 1, 1, 0);
	globals->audioSema = sceKernelCreateSema("hblaudiosema", 0, 1, 1, 0);
	globals->ioSema = sceKernelCreateSema("hbliosema", 0, 1, 1, 0);
}

int start_thread()
{
	// Free memory
	scr_puts("Freeing memory\n");
	free_game_memory();

#ifdef LOAD_MODULES_FOR_SYSCALLS
	scr_puts("Building NIDs table with utilities\n");
	load_utils();
	p2_add_stubs();
#ifdef DISABLE_UNLOAD_UTILITY_MODULES
	UnloadModules();
#else
	unload_utils();
#endif
#endif

	scr_puts("Building NIDs table with savedata utility\n");
	p5_add_stubs();

	scr_puts("Resolving HBL stubs\n");
	resolve_hbl_stubs();

	scr_puts("Initializing hook\n");
	hook_init();

#ifdef RESET_HOME_LANG
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeSettings();
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

	dbg_printf("Loading HBL\n");
	load_hbl();

	scr_puts("Running eLoader\n");
	run_eloader();

	return 0;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID thid;

#ifdef DEBUG
	dbg_init();
	log_init();
#endif

        memset(globals, 0, sizeof(tGlobals));
	p2_add_stubs();
	resolve_hbl_stubs();

#if !defined(DEBUG) && defined(FORCE_FIRST_LOG)
	log_init();
#endif
	
#ifndef VITA
	// Intialize firmware and model
	get_module_sdk_version();
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	detect_psp_go();

	// Select syscall estimation method
	detect_syscall();
#endif
#endif

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

	preload_free_game_memory();

	// Create and start eloader thread
	thid = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0xF0000000, NULL);

	if (thid < 0) {
		//scr_printf("Error starting HBL thread 0x%08X\n", thid);
		dbg_printf("Error starting HBL thread 0x%08X\n", thid);
		sceKernelExitGame();
	} else
		sceKernelStartThread(thid, 0, NULL);

	sceKernelExitDeleteThread(0);

	// Never executed (hopefully)
	while(1)
		sceKernelDelayThread(0xFFFFFFFF);
}

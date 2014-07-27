/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include <common/stubs/runtime.h>
#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/graphics.h>
#include <common/sdk.h>
#include <loader.h>
#include <common/debug.h>
#include <common/config.h>
#include <common/utils.h>
#include <hbl/eloader.h>
#include <common/malloc.h>
#include <common/globals.h>
#include <exploit_config.h>
#include <svnversion.h>

#ifdef VITA
#define DISABLE_KERNEL_DUMP
#endif

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
			LOGSTR("--> ERROR 0x%08X FREEING PARTITON MEMORY ID 0x%08X\n", ret, *(SceUID *)blockids[i]);
	}
}
#endif

#ifdef DELETE_UNKNOWN_FPL
static void FreeFpl()
{
	SceUID uid;

	LOGSTR("loader.c: FreeFpl\n");

	for (uid =  0x03000000; uid < 0xE0000000; uid++)
        	if (sceKernelDeleteFpl(uid) >= 0) {
			LOGSTR("Succesfully Deleted FPL ID 0x%08X\n", uid);
			return;
        	}
}
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
static void CloseFiles()
{
	SceUID fd;
	int ret;

	LOGSTR("memory.c:CloseFiles\n");

	for (fd = 0; fd < 8; fd++) {
		ret = sceIoClose(fd);
		if (ret != SCE_KERNEL_ERROR_BADF)
			LOGSTR("tried closing file %d, result 0x%08X\n", fd, ret);
	}
}
#endif

static void (* run_eloader)();

// Loads HBL to memory
static void load_hbl(SceUID hbl_file)
{
	long file_size;
	int ret;
	SceUID HBL_block;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);

	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));

	// Allocate memory for HBL
#ifdef GAME_PRELOAD_FREEMEM
	PreloadFreeMem();
#endif
#ifdef FPL_EARLY_LOAD_ADDR_LIST
	//early memory cleanup to be able to load HBL at a convenient place
	LOGSTR("loader.c: PreloadFreeFPL\n");

	int uids[] = FPL_EARLY_LOAD_ADDR_LIST;

	for(i = 0; i < sizeof(uids) / sizeof(int); i++)
		if (sceKernelDeleteFpl(*(SceUID *)uids[i]) < 0)
			LOGSTR("--> ERROR 0x%08X Deleting FPL ID 0x%08X\n", ret, *(SceUID *)uids[i]);
#endif


	HBL_block = sceKernelAllocPartitionMemory(
		2, "Valentine", PSP_SMEM_Addr, file_size + sizeof(tGlobals), (void *)HBL_LOAD_ADDR);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
	globals = (tGlobals *)((int)run_eloader + file_size);

	// Load HBL to allocated memory
	ret = sceIoRead(hbl_file, (void *)run_eloader, file_size);
	if (ret < 0)
		exit_with_log(" ERROR READING HBL ", &ret, sizeof(ret));

	sceIoClose(hbl_file);

	LOGSTR("HBL loaded to allocated memory @ 0x%08X\n", (int)run_eloader);

	// Commit changes to RAM
	CLEAR_CACHE;
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

// Initializes the savedata dialog loop, opens p5
static void p5_open_savedata(int mode)
{
	SceUtilitySavedataParam dialog = {
		.base = {
			.size = sizeof(SceUtilitySavedataParam),
			.language = 1,
			.graphicsThread = 16,
			.accessThread = 16,
			.fontThread = 16,
			.soundThread = 16
		},
		.mode = mode
	};

	sceUtilitySavedataInitStart(&dialog);

	// Wait for the dialog to initialize
	while (sceUtilitySavedataGetStatus() < 2)
#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
		sceKernelDelayThread(256);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
		sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
}

// Runs the savedata dialog loop
static void p5_close_savedata()
{
	int status;
	int last_status = -1;

	LOGSTR("entering savedata dialog loop\n");

	while(1) {
		status = sceUtilitySavedataGetStatus();

		if (status != last_status) {
			LOGSTR("status changed from %d to %d\n", last_status, status);
			last_status = status;
		}

		switch(status)
		{
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilitySavedataUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilitySavedataShutdownStart();
				break;

			case PSP_UTILITY_DIALOG_NONE:
				LOGSTR("dialog has shut down\n");
				return;
		}

#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
		sceKernelDelayThread(256);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
		sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
	}
}

static int p5_find_add_stubs(const char *libname, void *p, size_t size)
{
	int num = 0;
	tStubEntry *pentry = *(tStubEntry **)(memfindsz(libname, p, size) + 40);

	// While it's a valid stub header
	while (elf_check_stub_entry(pentry)) {
		if (pentry->import_flags == 0x11 || !pentry->import_flags)
			num += add_stub_to_table(pentry);

		// Next entry
		pentry++;
	}
	
	return num;
}

static int p5_add_stubs()
{
	int num;

#ifndef HOOK_sceKernelVolatileMemUnlock_WITH_dummy
	sceKernelVolatileMemUnlock(0);
#endif

	p5_open_savedata(PSP_UTILITY_SAVEDATA_AUTOLOAD);

	num = p5_find_add_stubs("sceVshSDAuto_Module", (void *)0x08410000, 0x00010000);

	p5_close_savedata();

	p5_open_savedata(PSP_UTILITY_SAVEDATA_SAVE);

	num += p5_find_add_stubs("scePaf_Module", (void *)0x084C0000, 0x00010000);
	num += p5_find_add_stubs("sceVshCommonUtil_Module", (void *)0x08760000, 0x00010000);
	num += p5_find_add_stubs("sceDialogmain_Module", (void *)0x08770000, 0x00010000);

	p5_close_savedata();
	cls();

	return num;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID hbl_file;
	int num_nids;

	cls();
	puts_scr("Starting HBL R"SVNVERSION" http://code.google.com/p/valentine-hbl");
#ifdef DEBUG
#ifdef NID_DEBUG
	puts_scr("DEBUG version (+NIDS)");
#else
	puts_scr("DEBUG version");
#endif
#else
	puts_scr_color("DO NOT POST LOG FILES OR BUG REPORTS FOR THIS VERSION!!!", 0x000000FF);
#endif

#ifdef PRE_LOADER_EXEC
	preLoader_Exec();
#endif

#ifdef RESET_HOME_LANG
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeSettings();
#endif

#if defined(FORCE_FIRST_LOG) || defined(DEBUG)
	// Some exploits need to "trigger" file I/O in order for HBL to work, not sure why
	logstr("Loader running\n");
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
	CloseFiles();
#endif

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));

	LOGSTR("Loading HBL\n");
	load_hbl(hbl_file);

	//reset the contents of the debug file;
	init_debug();

	//init global variables
	init_globals();

#ifndef VITA
	int fw_ver = get_fw_ver();
	switch (fw_ver)
	{
		case 0:
		case 1:
			puts_scr("Unknown Firmware :(");
			break;
		default:
			prtstr("Firmware %d.%dx detected", fw_ver / 100,  (fw_ver % 100) / 10);
			break;
	}

	if (getPSPModel() == PSP_GO) {
		puts_scr("PSP Go Detected");
#ifndef DISABLE_KERNEL_DUMP
		// If PSPGo on 6.20+, do a kmem dump
		if (get_fw_ver() >= 620)
			get_kmem_dump();
#endif
	}
#endif

	// Build NID table
	puts_scr("Building NIDs table with game");
	num_nids = p2_add_stubs();
	puts_scr("Building NIDs table with savedata utility");
	num_nids += p5_add_stubs();
	LOGSTR("NUM NIDS: %d\n", num_nids);

	if(num_nids <= 0)
		exit_with_log("No Nids ???", NULL, 0);

#if defined(DEBUG) && !defined(DEACTIVATE_SYSCALL_ESTIMATION)
	dump_lib_table();
#endif

	LOGSTR("Resolving HBL stubs\n");
	resolve_stubs();
	LOGSTR("HBL stubs copied, running eLoader\n");
	run_eloader();
}

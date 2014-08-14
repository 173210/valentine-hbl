#include <common/stubs/runtime.h>
#include <common/stubs/syscall.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/malloc.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/mod/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/utils/memory.h>
#include <hbl/utils/settings.h>
#include <hbl/eloader.h>
#include <exploit_config.h>

static int hbl_exit_cb_called = 0;
int hook_exit_cb_called = 0;
SceKernelCallbackFunction hook_exit_cb = NULL;

int num_pend_th = 0;
int num_run_th = 0;
int num_exit_th = 0;

// HBL entry point
// Needs path to ELF or EBOOT
static void run_eboot(SceUID fd, const char *path)
{
	SceOff off;
	SceUID mod_id;
	char cfg_path[256];

	scr_init();

	dbg_printf("EBOOT path: %s\n", (u32)path);

	//Load Game config overrides
	_sprintf(cfg_path, "%s" HBL_CONFIG, path);
	loadConfig(cfg_path);

	// Extracts ELF from PBP
	eboot_get_elf_off(fd, &off);

	dbg_printf("Loading module\n");

	//clean VRAM before running the homebrew (see : http://code.google.com/p/valentine-hbl/issues/detail?id=137 )
	//if the game does not import sceGeEdramGetAddr or sceGeEdramGetSize, it might be safer to hardcode those values.
	// I don't think they change based on each psp model
#ifdef FORCE_HARDCODED_VRAM_SIZE
	memset(sceGeEdramGetAddr(), 0, 0x00200000);
#else
	memset(sceGeEdramGetAddr(), 0, sceGeEdramGetSize());
#endif

	mod_id = load_module(fd, path, (void *)PRX_LOAD_ADDRESS, off);

	// No need for ELF file anymore
	sceIoClose(fd);

	if (mod_id < 0) {
		dbg_printf("ERROR 0x%08X loading main module\n", mod_id);
		sceKernelExitGame();
	}

	mod_id = start_module(mod_id);

	if (mod_id < 0) {
		dbg_printf("ERROR 0x%08X starting main module\n", mod_id);
		sceKernelExitGame();
	}
}

static void wait_for_eboot_end()
{
	// Sleep until all threads have exited.
	int lwait = 1;
	int exit_callback_timeout = 0;
	SceCtrlData pad;

	while(lwait) {
		//sceKernelWaitSema(gthSema, 1, 0);

		//Check for force exit to the menu
		if (force_exit_buttons) {
#ifdef HOOK_sceCtrlPeekBufferPositive_WITH_sceCtrlReadBufferPositive
			sceCtrlReadBufferPositive(&pad, 1);
#else
			sceCtrlPeekBufferPositive(&pad, 1);
#endif
			if (pad.Buttons == force_exit_buttons)
				exit_everything_but_me();
		}

		// Quit if the exit callback was called and has finished processing
		if (hbl_exit_cb_called == 2) {
			// Increment the time the homebrew is taking to exit
			exit_callback_timeout++;

			// Force quit if the timeout is reached or no exit callback was defined
			if (!hook_exit_cb || (exit_callback_timeout > 20))
				exit_everything_but_me();
		}

		lwait = num_run_th + num_pend_th;
		//sceKernelSignalSema(gthSema, 1);

		sceKernelDelayThread(65536);
	}

	scr_init();
	dbg_printf("Threads are dead\n");
}

static void cleanup(u32 num_lib)
{

        threads_cleanup();
	ram_cleanup();
	free_allmalloc_hbls();

	unload_modules();

	//cleanup globals
	globals->lib_num = num_lib; //reinit with only the initial libraries, removing the ones loaded outside
	hook_exit_cb_called = 0;
	hook_exit_cb = NULL;
}

static void ramcheck(int expected_free_ram) {
	int free_ram = sceKernelTotalFreeMemSize();

	if (expected_free_ram > free_ram)
		scr_printf("WARNING! Memory leak: %d -> %d\n",
			expected_free_ram, free_ram);
}

// HBL exit callback
int hbl_exit_callback()
{	
	dbg_printf("HBL Exit Callback Called\n");

	// Signal that the callback is being run now
	hbl_exit_cb_called = 1;

	if (hook_exit_cb) {
		dbg_printf("Call exit CB: %08X\n", (int)hook_exit_cb);
		hook_exit_cb_called = 1;
		hook_exit_cb(0, 0, NULL);
	}

	// Signal that the callback has finished
	hbl_exit_cb_called = 2;

	return 0;
}

// HBL callback thread
int callback_thread()
{
	int cbid, ret;

	//Setup HBL exit callback
	cbid = sceKernelCreateCallback("HBLexitcb", hbl_exit_callback, NULL);
	ret = sceKernelRegisterExitCallback(cbid);

	dbg_printf("Setup HBL Callback:\n  cbid=%08X\n  ret=%08X\n", cbid, ret);

	if (!ret)
#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
		_hook_sceKernelSleepThreadCB();
#else
		sceKernelSleepThreadCB();
#endif

	return ret;
}

// HBL main thread
int start_thread() //SceSize args, void *argp)
{
	SceUID fd;
	int exit = 0;
	int init_free;
	int thid;
    
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

	scr_puts("Resolving HBL stubs\n");
	resolve_stubs();
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
	sceKernelDcacheWritebackRange((void *)HBL_STUBS_START, NUM_HBL_IMPORTS * 2 * 4);
#else
	sceKernelDcacheWritebackAll();
#endif
#ifdef HOOK_sceKernelIcacheInvalidateAll_WITH_sceKernelIcacheInvalidateRange
	sceKernelIcacheInvalidateRange((void *)HBL_STUBS_START, NUM_HBL_IMPORTS * 2 * 4);
#elif !defined(HOOK_sceKernelIcacheInvalidateAll_WITH_dummy)
	sceKernelIcacheInvalidateAll();
#endif
#endif

	scr_puts("Initializing hook\n");
	init_hook();

	scr_puts("Creating callback thread\n");
	thid = sceKernelCreateThread("HBLexitcbthread", callback_thread, 0x11, 0xFA0, THREAD_ATTR_USER, NULL);
	if(thid > -1) {
		dbg_printf("Callback Thread Created\n  thid=%08X\n", thid);
		sceKernelStartThread(thid, 0, 0);
	}
	else {
		dbg_printf("Failed Callback Thread Creation\n  thid=%08X\n", thid);
		scr_puts("- Failed Callback Thread Creation\n");
	}

	scr_puts("Loading EBOOT\n");

	//Run the hardcoded eboot if it exists...
	fd = sceIoOpen(EBOOT_PATH, PSP_O_RDONLY, 777);
	if (fd > 0) {
		exit = 1;
		return_to_xmb_on_exit = 1;
		run_eboot(fd, EBOOT_PATH);
		//we wait infinitely here, or until exit callback is called
		while(!hbl_exit_cb_called)
			sceKernelDelayThread(65536);
	}

	//...otherwise launch the menu
	while (!exit) {
		init_free = sceKernelTotalFreeMemSize();

		//Load default config
		loadGlobalConfig();

		//run menu
		run_eboot(sceIoOpen(MENU_PATH, PSP_O_RDONLY, 777), MENU_PATH);
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (!strcmp("quit", hb_fname) || hbl_exit_cb_called)
			break;
		init_free = sceKernelTotalFreeMemSize();

		//re-Load default config
		loadGlobalConfig();
		dbg_printf("Config Loaded OK\n");
		dbg_printf("Eboot is: %s\n", hb_fname);

		//run homebrew
		run_eboot(sceIoOpen(hb_fname, PSP_O_RDONLY, 777), hb_fname);
		dbg_printf("Eboot Started OK\n");
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (hbl_exit_cb_called)
			break;
	}

	sceKernelExitGame();

	return 0;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID thid;

	scr_init();
	scr_puts("Starting HBL\n");
	// Create and start eloader thread
	thid = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0xF0000000, NULL);

	if(thid >= 0)
		thid = sceKernelStartThread(thid, 0, NULL);
	else {
		scr_printf("Error starting HBL thread 0x%08X\n", thid);
		sceKernelExitGame();
	}

	sceKernelExitDeleteThread(0);

	// Never executed (hopefully)
	while(1)
		sceKernelDelayThread(0xFFFFFFFF);
}

// Big thanks to people who share information !!!

#include <common/stubs/syscall.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/modmgr/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/eloader.h>
#include <hbl/settings.h>
#include <exploit_config.h>

PSP_MODULE_INFO("HBL", PSP_MODULE_USER, 0, 0);

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
#ifdef HOOK_sceKernelExitGame_WITH_sceKernelExitGameWithStatus
		sceKernelExitGameWithStatus(mod_id);
#else
		sceKernelExitGame();
#endif
	}

	mod_id = start_module(mod_id);

	if (mod_id < 0) {
		dbg_printf("ERROR 0x%08X starting main module\n", mod_id);
#ifdef HOOK_sceKernelExitGame_WITH_sceKernelExitGameWithStatus
		sceKernelExitGameWithStatus(mod_id);
#else
		sceKernelExitGame();
#endif
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
int module_start()
{
	SceUID fd;
	char path[260];
	int exit = 0;
	int init_free;
	int thid;

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
	//...otherwise launch the menu
	while (!exit) {
		init_free = sceKernelTotalFreeMemSize();

		//Load default config
		loadGlobalConfig();

		//run menu
		fd = sceIoOpen(EBOOT_PATH, PSP_O_RDONLY, 777);
		if (fd < 0) {
			dbg_printf("Failed Opening EBOOT.PBP: 0x%08X\n", fd);
			scr_printf("Failed Opening EBOOT.PBP: 0x%08X\n", fd);
			break;
		}
		run_eboot(fd, EBOOT_PATH);
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (!strcmp("quit", hb_fname) || hbl_exit_cb_called)
			break;
		strcpy(path, hb_fname);
		init_free = sceKernelTotalFreeMemSize();

		//re-Load default config
		loadGlobalConfig();
		dbg_printf("Config Loaded OK\n");
		dbg_printf("Eboot is: %s\n", path);

		//run homebrew
		run_eboot(sceIoOpen(path, PSP_O_RDONLY, 777), path);
		dbg_printf("Eboot Started OK\n");
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (hbl_exit_cb_called)
			break;
	}

#ifdef HOOK_sceKernelExitGame_WITH_sceKernelExitGameWithStatus
	sceKernelExitGameWithStatus(0);
#else
	sceKernelExitGame();
#endif

	return 0;
}

// Big thanks to people who share information !!!

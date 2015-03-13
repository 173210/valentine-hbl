#include <common/stubs/syscall.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/modmgr/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/eloader.h>
#include <hbl/settings.h>
#include <config.h>

PSP_MODULE_INFO("HBL", PSP_MODULE_USER, 0, 0);

static int hbl_exit_cb_called = 0;
int hook_exit_cb_called = 0;
SceKernelCallbackFunction hook_exit_cb = NULL;

int num_pend_th = 0;
int num_run_th = 0;
int num_exit_th = 0;

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

static int run_eboot(const char *path)
{
	SceUID fd, mod_id;
	SceOff off;
	char cfg_path[256];
	int ret;

	scr_init();

	if (path == NULL)
		ret = SCE_KERNEL_ERROR_ILLEGAL_ADDRESS;

	dbg_printf("EBOOT path: %s\n", path);

	fd = sceIoOpen(path, PSP_O_RDONLY, 777);
	if (fd < 0) {
		scr_printf("%s opening failed: 0x%08X\n", path, fd);
		return fd;
	}

	//Load Game config overrides
	_sprintf(cfg_path, "%s" HBL_CONFIG, path);
	loadConfig(cfg_path);

	// Extracts ELF from PBP
	eboot_get_elf_off(fd, &off);

	dbg_printf("%s: Loading module\n", __func__);

	//clean VRAM before running the homebrew (see : http://code.google.com/p/valentine-hbl/issues/detail?id=137 )
	//if the game does not import sceGeEdramGetAddr or sceGeEdramGetSize, it might be safer to hardcode those values.
	// I don't think they change based on each psp model
	memset(sceGeEdramGetAddr(), 0, isImported(sceGeEdramGetSize) ? sceGeEdramGetSize() : EDRAM_SIZE);

	mod_id = load_module(fd, path, (void *)PRX_LOAD_ADDRESS, off);

	dbg_printf("%s: Closing %s", __func__, path);
	ret = sceIoClose(fd);
	if (ret < 0) {
		dbg_printf("%s: Closing %s failed 0x%08X\n",
			__func__, path, ret);
		return ret;
	}

	if (mod_id < 0) {
		scr_printf("ERROR 0x%08X loading main module\n", mod_id);
		return mod_id;
	}

	ret = start_module(mod_id);
	if (ret < 0) {
		scr_printf("ERROR 0x%08X starting main module\n", mod_id);
		cleanup(globals->lib_num);
		return ret;
	}

	dbg_printf("%s: Success\n", __func__);

	return 0;
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
			if (isImported(sceCtrlPeekBufferPositive))
				sceCtrlPeekBufferPositive(&pad, 1);
			else if (isImported(sceCtrlReadBufferPositive))
				sceCtrlReadBufferPositive(&pad, 1);

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

static void ramcheck(int expected_free_ram) {
	int free_ram = hblKernelTotalFreeMemSize();

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

	if (!ret) {
		if (isImported(sceKernelSleepThreadCB))
			sceKernelSleepThreadCB();
		else if (isImported(sceKernelDelayThreadCB))
			_hook_sceKernelSleepThreadCB();
	}

	return ret;
}

// HBL main thread
int module_start()
{
	char path[260];
	int exit = 0;
	int init_free;
	int thid;

	scr_puts("Creating callback thread\n");
	thid = sceKernelCreateThread("HBLexitcbthread", callback_thread, 0x11, 0xFA0, THREAD_ATTR_USER, NULL);
	if(thid > -1) {
		dbg_printf("Callback thread created\n  thid=%08X\n", thid);
		sceKernelStartThread(thid, 0, 0);
	}
	else {
		scr_printf("- Failed callback thread creation: 0x%08X\n", thid);
	}
	//...otherwise launch the menu
	while (!exit) {
		init_free = hblKernelTotalFreeMemSize();

		scr_puts("Loading global configurations");
		loadGlobalConfig();

		scr_puts("Running " EBOOT_PATH);
		if (run_eboot(EBOOT_PATH)) {
			ramcheck(init_free);
			break;
		}
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (!strcmp("quit", hb_fname) || hbl_exit_cb_called)
			break;
		strcpy(path, hb_fname);
		init_free = hblKernelTotalFreeMemSize();

		scr_puts("Loading global configurations");
		loadGlobalConfig();

		//run homebrew
		scr_printf("Running %s", path);
		if (run_eboot(path)) {
			ramcheck(init_free);
			continue;
		}
		wait_for_eboot_end();

		cleanup(globals->lib_num);
		ramcheck(init_free);
		if (hbl_exit_cb_called)
			break;
	}

	scr_puts("Exiting\n");
	if (isImported(sceKernelExitGame))
		sceKernelExitGame();
	else if (isImported(sceKernelExitGameWithStatus))
		sceKernelExitGameWithStatus(0);

	return 0;
}

// Big thanks to people who share information !!!

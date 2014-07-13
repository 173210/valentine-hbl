#include <common/utils/graphics.h>
#include <common/sdk.h>
#include <hbl/eloader.h>
#include <common/debug.h>
#include <hbl/stubs/hook.h>
#include <common/utils.h>
#include <hbl/stubs/test.h>
#include <hbl/utils/settings.h>
#include <svnversion.h>
#include <common/malloc.h>
#include <common/runtime_stubs.h>
#include <hbl/utils/memory.h>
#include <hbl/mod/modmgr.h>
#include <common/globals.h>
#include <exploit_config.h>

// Additional globals initialization.
// some games do not import those, so loader.c (and therefore globals.c)
// cannot have them
void init_globals_2() {
    
    globals->memSema = sceKernelCreateSema("hblmemsema",0,1,1,0);
    globals->thSema = sceKernelCreateSema("hblthSema",0,1,1,0);
    globals->cbSema = sceKernelCreateSema("hblcbsema",0,1,1,0);
    globals->audioSema = sceKernelCreateSema("hblaudiosema",0,1,1,0);
	globals->ioSema = sceKernelCreateSema("hbliosema",0,1,1,0);
};

// HBL entry point
// Needs path to ELF or EBOOT
void run_eboot(const char *path, int is_eboot)
{
	SceUID elf_file;
	SceOff offset = 0;
	SceUID mod_id;

	cls();

	LOGSTR1("EBOOT path: %s\n", (u32)path);

    //Load Game config overrides
    char config_path[256];
    strcpy(config_path, path);
    int path_len = strlen(path) - strlen("EBOOT.PBP");
    config_path[path_len] = 0;
    strcat(config_path, HBL_CONFIG);
    loadConfig(config_path);

	// Extracts ELF from PBP
	if (is_eboot)
		elf_file = elf_eboot_extract_open(path, &offset);
	// Plain ELF
	else
		elf_file = sceIoOpen(path, PSP_O_RDONLY, 0777);

	LOGSTR0("Loading module\n");

    //clean VRAM before running the homebrew (see : http://code.google.com/p/valentine-hbl/issues/detail?id=137 )
    //if the game does not import sceGeEdramGetAddr or sceGeEdramGetSize, it might be safer to hardcode those values.
    // I don't think they change based on each psp model
#ifdef FORCE_HARDCODED_VRAM_SIZE
	memset(sceGeEdramGetAddr(), 0, 0x00200000);
#else
	memset(sceGeEdramGetAddr(), 0, sceGeEdramGetSize());
#endif

	mod_id = load_module(elf_file, path, (void*)PRX_LOAD_ADDRESS, offset);

	// No need for ELF file anymore
	sceIoClose(elf_file);

	if (mod_id < 0)
	{
		LOGSTR1("ERROR 0x%08lX loading main module\n", mod_id);
		EXIT;
	}

	mod_id = start_module(mod_id);

	if (mod_id < 0)
	{
		LOGSTR1("ERROR 0x%08lX starting main module\n", mod_id);
		EXIT;
	}

	return;
}

void wait_for_eboot_end()
{
  sceKernelDelayThread(2000000);

    /***************************************************************************/
  /* Sleep until all threads have exited.                                    */
  /***************************************************************************/
    int lwait = 1;
	int exit_callback_timeout = 0;
    SceCtrlData pad;
    while(lwait)
    {
        //sceKernelWaitSema(gthSema, 1, 0);

        //Check for force exit to the menu
        if (globals->force_exit_buttons)
        {
#ifdef HOOK_sceCtrlPeekBufferPositive_WITH_sceCtrlReadBufferPositive
            sceCtrlReadBufferPositive(&pad, 1);
#else
			sceCtrlPeekBufferPositive(&pad, 1);
#endif
            if (pad.Buttons == globals->force_exit_buttons)
            {
                exit_everything_but_me();
            }
        }

		// Quit if the exit callback was called and has finished processing
		if (globals->exit_cb_called == 2)
		{
			// Increment the time the homebrew is taking to exit
			exit_callback_timeout++;

			// Force quit if the timeout is reached or no exit callback was defined
			if (!globals->exitcb || (exit_callback_timeout > 20))
				exit_everything_but_me();
		}

        lwait = globals->num_run_th + globals->num_pend_th;
        //sceKernelSignalSema(gthSema, 1);

        sceKernelDelayThread(1000000);
	}
    cls();
    LOGSTR0("Threads are dead\n");
}

void cleanup(u32 num_lib)
{

        threads_cleanup();
    ram_cleanup();
    free_all_mallocs();


	#ifndef DISABLE_UNLOAD_UTILITY_MODULES
    //unload utility modules
    int i;
    int modid;
    u32 num_utility = globals->mod_table.num_utility;
	for (i=num_utility-1; i>=0; i--)
	{

		if ((modid = globals->mod_table.utility[i]) != 0)
		{
			//PSP_MODULE_AV_AVCODEC -> cast syscall of sceAudiocodec and sceVideocodec
			//PSP_MODULE_AV_MP3		-> On 6.20 OFW, libmp3 has a bug when unload it.
#ifndef VITA
			if ( ! ( modid == PSP_MODULE_AV_AVCODEC || (modid == PSP_MODULE_AV_MP3 && get_fw_ver() <= 620)) )
#endif
			{
	            LOGSTR1("UNLoad utility module id  0x%08lX \n", modid);
				int ret = unload_utility_module(modid);
	            if (ret < 0)
	            {
	                LOGSTR2("WARNING! error unloading module %d: 0x%08lX\n",globals->mod_table.utility[i], ret);
	                puts_scr("WARNING! ERROR UNLOADING UTILITY");
	                sceKernelDelayThread(1000000);
	            }
	            else
	            {
	                globals->mod_table.utility[i] = 0;
	                globals->mod_table.utility[i] = globals->mod_table.utility[globals->mod_table.num_utility-1];
	                globals->mod_table.utility[globals->mod_table.num_utility-1] = 0;
	                globals->mod_table.num_utility--;
	            }
	        }
		}
	}
	#endif
    //cleanup globals
    globals->mod_table.num_loaded_mod = 0;
    memset(&(globals->mod_table.table), 0, sizeof(HBLModInfo) * MAX_MODULES);
    globals->lib_table.num = num_lib; //reinit with only the initial libraries, removing the ones loaded outside
    //memset(&(globals->lib_table), 0, sizeof(HBLLibTable));
    globals->calledexitcb = 0;
    globals->exitcb = 0;

    return;
}

void ramcheck(int expected_free_ram) {
    int free_ram = sceKernelTotalFreeMemSize();
    if (expected_free_ram > free_ram && !is_utility_loaded(PSP_MODULE_AV_MP3)) //for now, we admit that mp3 utility needs to be loaded all the time...
    {
        LOGSTR2("WARNING! Memory leak: %d -> %d\n", expected_free_ram, free_ram);
        puts_scr("WARNING! MEMORY LEAK");
        sceKernelDelayThread(1000000);
    }
}

//Tests some basic functions
// then runs the menu eboot
void run_menu()
{
        puts_scr("Loading Menu");

    // Just trying the basic functions used by the menu
    SceUID id = -1;
	SceIoDirent entry;

    puts_scr("-Test sceIoDopen");

    id = _test_sceIoDopen("ms0:");
    if (id < 0)
    {
		puts_scr_color("--failure", 0x000000FF);
	    sceKernelDelayThread(1000000);
    }

    else
    {
        puts_scr_color("--success", 0x0000FF00);

        puts_scr("-Test sceIoDread");
        memset(&entry, 0, sizeof(SceIoDirent));
        if (_test_sceIoDread(id, &entry) < 0)
        {
	        puts_scr_color("--failure", 0x000000FF);
	        sceKernelDelayThread(1000000);
		}
		else
        	puts_scr_color("--success", 0x0000FF00);

        puts_scr("-Test sceIoDclose");
        id = _test_sceIoDclose(id);
        if (id < 0)
        {
	        puts_scr_color("--failure", 0x000000FF);
	        sceKernelDelayThread(1000000);
        }
        else
            puts_scr_color("--success", 0x0000FF00);
    }

    run_eboot(globals->menupath, 1);
}

// HBL exit callback
int hbl_exit_callback(int arg1, int arg2, void *arg)
{
	// Avoid compiler warnings
	arg1 = arg1;
	arg2 = arg2;
	arg = arg;

	
	LOGSTR0("HBL Exit Callback Called\n");

	// Signal that the callback is being run now
	globals->exit_cb_called = 1;

	if (globals->exitcb)
    {
        LOGSTR1("Call exit CB: %08lX\n", (u32) globals->exitcb);
        globals->calledexitcb = 1;
        globals->exitcb(0, 0, NULL);
    }

	// Signal that the callback has finished
	globals->exit_cb_called = 2;

	return 0;
}

// HBL callback thread
int callback_thread(SceSize args, void *argp)
{
	// Avoid compiler warnings
	args = args;
	argp = argp;

	int cbid, UNUSED(ret);

    //Setup HBL exit callback
	cbid = sceKernelCreateCallback("HBLexitcb", hbl_exit_callback, NULL);
	ret = sceKernelRegisterExitCallback(cbid);

	LOGSTR2("Setup HBL Callback:\n  cbid=%08lX\n  ret=%08lX\n", cbid, ret);

#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
	_hook_sceKernelSleepThreadCB();
#else
    sceKernelSleepThreadCB();
#endif

	return 0;
}

// HBL main thread
int start_thread() //SceSize args, void *argp)
{
    int exit = 0;
	int thid;
    
    // Free memory
    puts_scr("Freeing memory");
    free_game_memory();

    puts_scr("-- Done");

    // Start Callback Thread
	thid = sceKernelCreateThread("HBLexitcbthread", callback_thread, 0x11, 0xFA0, THREAD_ATTR_USER, NULL);
	if(thid > -1)
	{
		LOGSTR1("Callback Thread Created\n  thid=%08lX\n", thid);
		sceKernelStartThread(thid, 0, 0);
	}
	else
		LOGSTR1("Failed Callback Thread Creation\n  thid=%08lX\n", thid);

    LOGSTR0("START HBL\n");

    u32 num_lib = globals->lib_table.num;

    //Run the hardcoded eboot if it exists...
    if (file_exists(EBOOT_PATH))
    {
        exit = 1;
        globals->return_to_xmb_on_exit = 1;
        run_eboot(EBOOT_PATH, 1);
        //we wait infinitely here, or until exit callback is called
        while(!globals->exit_cb_called)
            sceKernelDelayThread(100000);
    }

    //...otherwise launch the menu
    while (!exit)
    {
        int initial_free_ram = sceKernelTotalFreeMemSize();
        //Load default config
        loadGlobalConfig();
        //run menu
        run_menu();
        wait_for_eboot_end();
        cleanup(num_lib);
        ramcheck(initial_free_ram);
        if (strcmp("quit", globals->hb_fname) == 0 || globals->exit_cb_called){
            exit = 1;
            continue;
        }

        initial_free_ram = sceKernelTotalFreeMemSize();
        char filename[512];
        strcpy(filename, globals->hb_fname);
        LOGSTR1("Eboot is: %s\n", (u32)filename);
        //re-Load default config
        loadGlobalConfig();
        LOGSTR0("Config Loaded OK\n");
        LOGSTR1("Eboot is: %s\n", (u32)filename);
        //run homebrew
        run_eboot(filename, 1);
        LOGSTR0("Eboot Started OK\n");
        wait_for_eboot_end();
        cleanup(num_lib);
        ramcheck(initial_free_ram);
        if (globals->exit_cb_called)
			exit = 1;
    }

	sceKernelExitGame();

	return 0;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID thid;

    // Second step of globals initalization
	init_globals_2();


	// Create and start eloader thread
	thid = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0xF0000000, NULL);

	if(thid >= 0)
	{
		thid = sceKernelStartThread(thid, 0, NULL);
	}
    else
    {
        PRTSTR1("Error starting HBL thread 0x%08lX", thid);
    }

	sceKernelExitDeleteThread(0);

	// Never executed (hopefully)
	while(1)
		sceKernelDelayThread(0xFFFFFFFF);
}

// Big thanks to people who share information !!!

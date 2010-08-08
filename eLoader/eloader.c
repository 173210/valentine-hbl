#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "hook.h"
#include "utils.h"
#include "test.h"
#include "settings.h"
#include "graphics.h"
#include "svnversion.h"
#include "malloc.h"
#include "resolve.h"
#include "memory.h"
#include "modmgr.h"
#include "globals.h"



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
    memset(sceGeEdramGetAddr(), 0, sceGeEdramGetSize());    
    
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

  tGlobals * g = get_globals();
  /***************************************************************************/
  /* Sleep until all threads have exited.                                    */
  /***************************************************************************/
    int lwait = 1;
    SceCtrlData pad;
    while(lwait)
    {
        //sceKernelWaitSema(gThrSema, 1, 0);
        
        //Check for force exit to the menu
        if (g->force_exit_buttons)
        {
            sceCtrlReadBufferPositive(&pad, 1);
            if (pad.Buttons == g->force_exit_buttons)
            {
                exit_everything_but_me();
            }
        }
        lwait = g->numRunThreads + g->numPendThreads;
        //sceKernelSignalSema(gThrSema, 1);

        sceKernelDelayThread(1000000);
	}
    cls();
    LOGSTR0("Threads are dead\n");
}

void cleanup(u32 num_lib)
{

    tGlobals * g = get_globals();
    threads_cleanup();
    ram_cleanup();
    free_all_mallocs();
    
    //unload utility modules
    int i;
    int modid;
    u32 num_utility = g->mod_table.num_utility;
	for (i=num_utility-1; i>=0; i--)
	{
		if ((modid = g->mod_table.utility[i]) && modid != PSP_MODULE_AV_MP3) //some utilities cannot be re-loaded after being unloaded ?
        {
            LOGSTR1("UNLoad utility module id  %d \n", modid);
			int ret = sceUtilityUnloadModule(modid);
            if (ret < 0) 
            {
                LOGSTR2("WARNING! error unloading module %d: 0x%08lX\n",g->mod_table.utility[i], ret);
                print_to_screen("WARNING! ERROR UNLOADING UTILITY");
                sceKernelDelayThread(1000000); 
            }
            else
            {
                g->mod_table.utility[i] = 0;
                g->mod_table.utility[i] = g->mod_table.utility[g->mod_table.num_utility-1];
                g->mod_table.utility[g->mod_table.num_utility-1] = 0;
                g->mod_table.num_utility--;
            }
        }
	}
    //cleanup globals
    g->mod_table.num_loaded_mod = 0;
    memset(&(g->mod_table.table), 0, sizeof(HBLModInfo) * MAX_MODULES);
    g->library_table.num = num_lib; //reinit with only the initial libraries, removing the ones loaded outside
    //memset(&(g->library_table), 0, sizeof(HBLLibTable));
    g->calledexitcb = 0;
    g->exitcallback = 0;

    return;
}

void ramcheck(int expected_free_ram) {
    int free_ram = sceKernelTotalFreeMemSize();
    if (expected_free_ram > free_ram && !is_utility_loaded(PSP_MODULE_AV_MP3)) //for now, we admit that mp3 utility needs to be loaded all the time...
    {
        LOGSTR2("WARNING! Memory leak: %d -> %d\n", expected_free_ram, free_ram);
        print_to_screen("WARNING! MEMORY LEAK");
        sceKernelDelayThread(1000000);
    }
}

//Tests some basic functions
// then runs the menu eboot
void run_menu()
{
    tGlobals * g = get_globals();
    print_to_screen("Loading Menu");
      
    // Just trying the basic functions used by the menu
    SceUID id = -1;
	SceIoDirent entry;

    print_to_screen("-Test sceIoDopen");

    id = _test_sceIoDopen("ms0:");
    if (id < 0)
    {
		print_to_screen_color("--failure", 0x000000FF);
	    sceKernelDelayThread(1000000);
    }
	
    else
    {
        print_to_screen_color("--success", 0x0000FF00);
		
        print_to_screen("-Test sceIoDread");
        memset(&entry, 0, sizeof(SceIoDirent));
        if (_test_sceIoDread(id, &entry) < 0)
        {
	        print_to_screen_color("--failure", 0x000000FF);
	        sceKernelDelayThread(1000000);
		}
		else
        	print_to_screen_color("--success", 0x0000FF00);
		
        print_to_screen("-Test sceIoDclose");
        id = _test_sceIoDclose(id);
        if (id < 0)
        {
	        print_to_screen_color("--failure", 0x000000FF);
	        sceKernelDelayThread(1000000);
        }
        else
            print_to_screen_color("--success", 0x0000FF00);
    }
	
    run_eboot(g->menupath, 1);       
}


// HBL main thread
int start_thread() //SceSize args, void *argp)
{
	int num_nids;
    int exit = 0;
    tGlobals * g = get_globals();
	LOGSTR0("Initializing memory allocation\n");
	init_malloc();
	
	// Build NID table
    print_to_screen("Building NIDs table");
	num_nids = build_nid_table();
    LOGSTR1("NUM NIDS: %d\n", num_nids);
	
	if(num_nids <= 0)
	{	
        exit_with_log("No Nids ???", NULL, 0);
    }
    
    // FIRST THING TO DO!!!
    print_to_screen("Resolving own missing stubs");
    resolve_missing_stubs();

    // Free memory
    print_to_screen("Freeing memory");
    free_game_memory();
    
    print_to_screen("-- Done");
    LOGSTR0("START HBL\n");

    u32 num_lib = g->library_table.num;
    
    //Run the hardcoded eboot if it exists...
    if (file_exists(EBOOT_PATH))
    {
        g->return_to_xmb_on_exit = 1;
        run_eboot(EBOOT_PATH, 1);
        //we wait infinitely here
        while(1)
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
        if (strcmp("quit", g->hb_filename) == 0){
            exit = 1;
            continue;
        }
        
        initial_free_ram = sceKernelTotalFreeMemSize();
        char filename[512];
        strcpy(filename, g->hb_filename);
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
    }
	
	sceKernelExitGame();
	
	return 0;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID thid;
    int firmware_version = getFirmwareVersion();
	cls();
    print_to_screen("Starting HBL R"SVNVERSION" http://code.google.com/p/valentine-hbl");
    
#ifdef DEBUG
#ifdef NID_DEBUG
    print_to_screen("DEBUG version (+NIDS)");
#else
    print_to_screen("DEBUG version");        
#endif     
#else
    print_to_screen_color("DO NOT POST LOG FILES OR BUG REPORTS FOR THIS VERSION!!!", 0x000000FF);
#endif

    
	switch (firmware_version) 
	{
		case 0:
		case 1:
		    print_to_screen("Unknown Firmware :(");
		    break; 
		default:
		    PRTSTR2("Firmware %d.%dx detected", firmware_version / 100,  (firmware_version % 100) / 10);
		    break;
    }
    
    if (getPSPModel() == PSP_GO)
	{
        print_to_screen("PSP Go Detected");
    }

    
	// Create and start eloader thread
	thid = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0, NULL);
	
	if(thid >= 0)
	{
		thid = sceKernelStartThread(thid, 0, NULL);
	}

	sceKernelExitDeleteThread(0);

	// Never executed (hopefully)
	return;
}

// Big thanks to people who share information !!!

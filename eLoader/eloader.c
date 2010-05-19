#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "menu.h"
#include "tables.h"
#include "utils.h"
#include "test.h"
#include "settings.h"

#include "svnversion.h"

// Menu variables
int g_menu_enabled = 0; // this is set to 1 at runtime if a menu.bin file exists
u32 * isSet = EBOOT_SET_ADDRESS;
u32 * ebootPath = EBOOT_PATH_ADDRESS;
u32 * menu_pointer = MENU_LOAD_ADDRESS;

/* Loads a Basic menu as a thread
* In the future, we might want the menu to be an actual homebrew
*/
void loadMenu()
{
    print_to_screen("Loading Menu");
      
    // Just trying the basic functions used by the menu
    SceUID id = -1;
    int attempts = 0;
	SceUID menu_file;
    SceOff file_size;
	int bytes_read;
	SceIoDirent entry;
	SceUID menuThread;

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
	
	if ((menu_file = sceIoOpen(MENU_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD MENU ", &menu_file, sizeof(menu_file));

	// Get MENU size
	file_size = sceIoLseek(menu_file, 0, PSP_SEEK_END);
	sceIoLseek(menu_file, 0, PSP_SEEK_SET);
    
	// Load MENU to buffer
	if ((bytes_read = sceIoRead(menu_file, (void*)menu_pointer, file_size)) < 0)
		exit_with_log(" ERROR READING MENU ", &bytes_read, sizeof(bytes_read));
        
    void (*start_entry)(SceSize, void*) = menu_pointer;
	menuThread = sceKernelCreateThread("menu", start_entry, 0x18, 0x10000, 0, NULL);

	if(menuThread >= 0)
	{
		menuThread = sceKernelStartThread(menuThread, 0, NULL);
    } 

	else 
	{
        exit_with_log(" Menu Launch failed ", NULL, 0);
    }        
}

void main_loop() 
{
    isSet[0] = 0;
	
    loadMenu();
	
    while(!isSet[0])
        sceKernelDelayThread(5000);
	
    //this is to avoid the stupid menu variable ebootPath to get overwritten
    char eboot_path_copy[256];
    strcpy(eboot_path_copy, ebootPath);
    
    start_eloader(eboot_path_copy, 1);
}

// HBL entry point
// Needs path to ELF or EBOOT
void start_eloader(const char *path, int is_eboot)
{
	SceUID elf_file;
	SceOff offset = 0;
	SceUID mod_id;

	cls();
	LOGSTR1("EBOOT path: %s\n", path);
    
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

	//weirdest thing here: if this is not the lowercase version of logstr, the non debug HBL crashes (wololo 2010/05/13)
	logstr0("Loading module\n");
	mod_id = load_module(elf_file, path, (void*)PRX_LOAD_ADDRESS, offset);

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

// HBL main thread
int start_thread(SceSize args, void *argp)
{
	int num_nids;

	LOGSTR0("Initializing memory allocation\n");
	init_malloc();
	
	// Build NID table
    print_to_screen("Building NIDs table");
	num_nids = build_nid_table();
    LOGSTR1("NUM NIDS: %d\n", num_nids);
	
	if(num_nids > 0)
	{	
		// FIRST THING TO DO!!!
        print_to_screen("Resolving own missing stubs");
		resolve_missing_stubs();
	
		// Free memory
        print_to_screen("Freeing memory");
		free_game_memory();
		
        print_to_screen("-- Done");
        LOGSTR0("START HBL\n");
        
        //Load config
        loadGlobalConfig();

		// Initialize module loading
		print_to_screen("Initializing LoadModule");

		/*
		if(init_load_module() == NULL)
			exit_with_log(" ERROR: UNABLE TO ALLOCATE MEMORY FOR MODULE TABLES ", NULL, 0);
		*/

		init_load_module();

        // Start the menu or run directly the hardcoded eboot      
        if (file_exists(EBOOT_PATH))
            start_eloader(EBOOT_PATH, 1);
        else if (file_exists(ELF_PATH))
            start_eloader(ELF_PATH, 0);
        else 
		{
            g_menu_enabled = 1;
            main_loop();
        }
	}
	
	// Loop forever
    print_to_screen("Looping HBL Thread");

	while(1)
		sceKernelDelayThread(100000);
	
	return 0;
}

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
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
	return 0;
}

// Big thanks to people who share information !!!

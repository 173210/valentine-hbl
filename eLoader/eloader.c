#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "elf.h"
#include "menu.h"
#include "graphics.h"
#include "utils.h"
#include "reloc.h"
#include "resolve.h"
#include "tables.h"
#include "scratchpad.h"
#include "memory.h"

/* eLoader */
/* Entry point: _start() */

// Any way to have those non globals ?
//u32 gp = 0;
//u32* entry_point = 0;
//u32 hbsize = 4000000; //default value for the hb size roughly 4MB. This value is never used in theory

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
    id = sceIoDopen("ms0:");
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
        if (sceIoDread(id, &entry) < 0 ) 
        {
            print_to_screen_color("--failure", 0x000000FF);
            sceKernelDelayThread(1000000);
        }
        else
            print_to_screen_color("--success", 0x0000FF00);
        
        print_to_screen("-Test sceIoDclose");
        id = sceIoDclose(id);
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

	LOGSTR1("EBOOT path: %s\n", path);
    
	// Extracts ELF from PBP
	if (is_eboot)		
		elf_file = elf_eboot_extract_open(path, &offset);
	// Plain ELF
	else
		elf_file = sceIoOpen(path, PSP_O_RDONLY, 0777);

	LOGSTR0("Loading module\n");
	mod_id = load_module(elf_file, path, (void*)PRX_LOAD_ADDRESS, offset);

	if (mod_id < 0)
	{
		LOGSTR1("ERROR 0x%08lX loading main module\n", mod_id);
		EXIT;
	}

	/*
	// Read ELF header
	sceIoRead(elf_file, &elf_header, sizeof(Elf32_Ehdr));
	
	LOGSTR1("ELF TYPE: 0x%08lX -->", elf_header.e_type);
    
    gp = getGP(elf_file, offset, &elf_header);

	// Static ELF
	if(elf_header.e_type == (Elf32_Half) ELF_STATIC)
	{	
		// TODO: insert into mod_table
		LOGSTR0("STATIC\n");		

		// Load ELF program section into memory
		hbsize = elf_load_program(elf_file, offset, &elf_header);		
	
		// Locate ELF's .lib.stubs section
		stubs_size = elf_find_imports(elf_file, offset, &elf_header, &pstub_entry);
	}
	
	// Relocatable ELF (PRX)
	else if(elf_header.e_type == (Elf32_Half) ELF_RELOC)
	{
		// TODO: insert into mod_table
		LOGSTR0("RELOC\n");
   
		// Load program section into memory and also get stub headers
		void* addr = (void*) PRX_LOAD_ADDRESS;
		stubs_size = prx_load_program(elf_file, offset, &elf_header, &pstub_entry, &hbsize, &addr);

		//Relocate all sections that need to
		sections_relocated = relocate_sections(elf_file, offset, &elf_header);

		// Relocate ELF entry point and GP register
		elf_header.e_entry = (u32)elf_header.e_entry + (u32)PRX_LOAD_ADDRESS;
        gp += (u32)PRX_LOAD_ADDRESS;
	}
	
	// Unknown ELF type
	else
	{
		exit_with_log(" UNKNOWN ELF TYPE ", NULL, 0);
	}

    LOGSTR1("GP: 0x%08lX\n", gp);
	
	// Resolve ELF's stubs with game's stubs and syscall estimation
	stubs_resolved = resolve_imports(pstub_entry, stubs_size);
   
	// No need for ELF file anymore
	sceIoClose(elf_file);	
	
	// DEBUG_PRINT(" PROGRAM SECTION START ", &(elf_header.e_entry), sizeof(Elf32_Addr));
    
    // Commit changes to RAM
	sceKernelDcacheWritebackInvalidateAll();
	*/

	mod_id = start_module(mod_id);

	if (mod_id < 0)
	{
		LOGSTR1("ERROR 0x%08lX starting main module\n", mod_id);
		EXIT;
	}

	/*
	// Create and start hb thread
    if (gp)
        SET_GP(gp);
	entry_point = (u32 *)elf_header.e_entry;

	LOGSTR1("Creating homebrew thread... Entry point: 0x%08lX ", entry_point);

	thid = sceKernelCreateThread("homebrew", entry_point, 0x18, 0x10000, 0, NULL);

	LOGSTR1("THID: 0x%08lX ", thid);

	if(thid >= 0)
	{
		thid = sceKernelStartThread(thid, strlen(eboot_path) + 1, (void *)eboot_path);
		if (thid < 0)
		{
			LOGSTR1(" HB Thread couldn't start. Error 0x%08lX\n", thid);
			sceKernelExitGame();
		}
    } 
	else 
	{
        LOGSTR1(" HB Thread couldn't be created. Error 0x%08lX\n", thid);
		sceKernelExitGame();
	}

	// Uncomment only if no homebrew thread created
	// execute_elf(strlen(eboot_path) + 1, (void *)eboot_path);
	*/

	return;
}

// HBL main thread
int start_thread(SceSize args, void *argp)
{
	int num_nids;
	
	// Build NID table
    print_to_screen("Build Nids table");
	num_nids = build_nid_table(nid_table);
    LOGSTR1("NUM NIDS: %d\n", num_nids);
	
	if(num_nids > 0)
	{	
		// FIRST THING TO DO!!!
        print_to_screen("Resolving missing stubs");
		resolve_missing_stubs();
	
		// Free memory
        print_to_screen("Free memory");
		free_game_memory();
		
        print_to_screen("-- Done (Free memory)");
        LOGSTR0("START HBL\n");

		// Initialize module loading
		print_to_screen("Initialize LoadModule");
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
    void *fb = (void *)0x444000000;
    int firmware_version = getFirmwareVersion();
	
    sceDisplaySetFrameBuf(fb, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
    SetColor(0);
    print_to_screen("Starting HBL -- http://code.google.com/p/valentine-hbl");
    
	switch (firmware_version) {
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

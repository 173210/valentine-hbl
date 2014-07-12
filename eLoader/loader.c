/* Half Byte Loader loader :P */
/* This loads HBL on memory */

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
#include <common/runtime_stubs.h>
#include <exploit_config.h>
#include <svnversion.h>

#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef AUTO_SEARCH_STUBS
#define AUTO_SEARCH_STUBS
#endif
#endif

// These are the addresses were we copy p5 stuff
// This might overwrite some important things in ram,
// so overwrite these values in exploit_config.h if needed
#ifndef RELOC_MODULE_ADDR_1
#define RELOC_MODULE_ADDR_1 0x09d10000
#endif
#ifndef RELOC_MODULE_ADDR_2
#define RELOC_MODULE_ADDR_2 0x09d30000
#endif
#ifndef RELOC_MODULE_ADDR_3
#define RELOC_MODULE_ADDR_3 0x09d50000
#endif
#ifndef RELOC_MODULE_ADDR_4
#define RELOC_MODULE_ADDR_4 0x09d70000
#endif


#ifdef RESET_HOME_SCREEN_LANGUAGE
// Reset language and button assignment for the HOME screen to system defaults
void resetHomeScreenSettings()
{
	int language;
	int buttonSwap;

	// Get system language, default to English on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language) < 0)
		language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;

	// Get button assignment, default to X = Enter on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &buttonSwap) < 0)
		buttonSwap = 1; // X = Enter

	sceImposeSetLanguageMode(language, buttonSwap);
}
#endif


#ifdef GAME_PRELOAD_FREEMEM
void PreloadFreeMem()
{
   u32 i;
   SceUID memids[] = GAME_PRELOAD_FREEMEM;

   for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
   {
      int ret = sceKernelFreePartitionMemory(*(SceUID*)memids[i]);
      if (ret < 0)
      {
         LOGSTR2("--> ERROR 0x%08lX FREEING PARTITON MEMORY ID 0x%08lX\n", ret, *(SceUID*)memids[i]);
      }
   }
}
#endif

#ifdef DELETE_UNKNOWN_FPL
void FreeFpl()
{
    LOGSTR0("loader.c:FreeFpl\n");
    SceUID i =  0x03000000;

    while(i < (SceUID)0xE0000000)
    {
        int ret = sceKernelDeleteFpl(i);
        if (ret >= 0)
        {
            LOGSTR1("Succesfully Deleted FPL ID 0x%08lX\n", i);
            return;
        }
        i++;
    }
}
#endif


#ifdef FORCE_CLOSE_FILES_IN_LOADER
void CloseFiles()
{
    LOGSTR0("memory.c:CloseFiles\n");
	SceUID result;
	int i;

	for (i = 0; i < 8; i++) // How many files can be open at once?
	{
		result = sceIoClose(i);
		if (result != (int)0x80020323) // bad file descriptor
		{
			LOGSTR2("tried closing file %d, result 0x%08lX\n", i, result);
		}
	}
}
#endif

void (*run_eloader)(unsigned long arglen, unsigned long* argp) = 0;

// Loads HBL to memory
void load_hbl(SceUID hbl_file)
{
    tGlobals * g = get_globals();
	unsigned long file_size;
	int bytes_read;
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
	LOGSTR0("loader.c:PreloadFreeFPL\n");
	int i;
	SceUID memids[] = FPL_EARLY_LOAD_ADDR_LIST;

	for(i = 0; i < sizeof(memids) / sizeof(u32); i++)
		if (sceKernelDeleteFpl(*(SceUID*)memids[i]) < 0)
			LOGSTR2("--> ERROR 0x%08lX Deleting FPL ID 0x%08lX\n", ret, *(SceUID*)memids[i]);
#endif


	HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, file_size, (void *)HBL_LOAD_ADDRESS);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);

	// Write HBL memory block info to scratchpad
	g->hbl_block_uid = HBL_block;
	g->hbl_block_addr = (u32)run_eloader;

	// Load HBL to allocated memory
	if ((bytes_read = sceIoRead(hbl_file, (void*)run_eloader, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));

	sceIoClose(hbl_file);

	LOGSTR1("HBL loaded to allocated memory @ 0x%08lX\n", (u32)run_eloader);

	// Commit changes to RAM
	CLEAR_CACHE;
}

// Autoresolves HBL stubs
void resolve_stubs()
{
	tGlobals * g = get_globals();
	u32 i;
	int ret;
	u32 num_nids;
	u32 *cur_stub = (u32 *)HBL_STUBS_START;
	u32 nid = 0, syscall = 0;
	char lib_name[MAX_LIBRARY_NAME_LENGTH];

	ret = config_initialize();

	if (ret < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &ret, sizeof(ret));

	config_num_nids_total(&num_nids);

	for (i = 0; i < num_nids; i++) {
		LOGSTR1("-Resolving import 0x%08lX: ", (int)cur_stub - HBL_STUBS_START);

		// NID & library for i-th import
		get_lib_nid(i, lib_name, &nid);

		LOGSTR0(lib_name);
		LOGSTR1(" 0x%08lX\n", nid);

		// Is it known by HBL?
		ret = get_nid_index(nid);

		// If it's known, get the call
		if (ret > 0) {
			LOGSTR0("-Found in NID table, using real call\n");
			syscall = g->nid_table.table[ret].call;
		} else {
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
			LOGSTR1("HBL Function missing at 0x%08lX, this could lead to trouble\n",  (int)cur_stub);
			syscall = NOP_OPCODE;
#else
			// If not, estimate
			syscall = estimate_syscall(lib_name, nid, g->syscalls_known ? FROM_LOWEST : FROM_CLOSEST);
#endif
		}

		*cur_stub++ = JR_RA_OPCODE;
		*cur_stub++ = syscall;
	}

	CLEAR_CACHE;

	config_close();

	LOGSTR0(" ****STUBS SEARCHED\n");
}


#ifndef DISABLE_KERNEL_DUMP

// Erase kernel dump
void init_kdump()
{
	SceUID fd;

	if ((fd = sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777)) >= 0)
	{
		sceIoClose(fd);
	}
}

// Dumps kmem
void get_kmem_dump()
{
	SceUID dump_fd;

	dump_fd = sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_WRONLY, 0777);

	if (dump_fd >= 0)
	{
		sceIoWrite(dump_fd, (void*) 0x08000000, (unsigned int)0x400000);
		sceIoClose(dump_fd);
	}
}

#endif



// Clear the video memory
void clear_vram()
{
#ifdef FORCE_HARDCODED_VRAM_SIZE
	memset(sceGeEdramGetAddr(), 0, 0x00200000);
#else
	memset(sceGeEdramGetAddr(), 0, sceGeEdramGetSize());
#endif
}



// Initializes the savedata dialog loop, opens p5
void p5_open_savedata(int mode)
{
	SceUtilitySavedataParam dialog;

	memset(&dialog, 0, sizeof(SceUtilitySavedataParam));
	dialog.base.size = sizeof(SceUtilitySavedataParam);

    dialog.base.language = 1;
    dialog.base.buttonSwap = 1;
	dialog.base.graphicsThread = 0x11;
	dialog.base.accessThread = 0x13;
	dialog.base.fontThread = 0x12;
	dialog.base.soundThread = 0x10;

	dialog.mode = mode;
	dialog.overwrite = 1;
	dialog.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST; // Set initial focus to the newest file (for loading)

	strcpy(dialog.gameName, "DEMO11111");	// First part of the save name, game identifier name
	strcpy(dialog.saveName, "0000");	// Second part of the save name, save identifier name

	char nameMultiple[][20] =	// End list with ""
	{
	 "0000",
	 "0001",
	 "0002",
	 "0003",
	 "0004",
	 ""
	};

	// List of multiple names
	dialog.saveNameList = nameMultiple;

	strcpy(dialog.fileName, "DATA.BIN");	// name of the data file

	// Allocate buffers used to store various parts of the save data
	dialog.dataBuf = NULL;//malloc_p2(0x20);
	dialog.dataBufSize = 0;//0x20;
	dialog.dataSize = 0;//0x20;

	sceUtilitySavedataInitStart(&dialog);

	// Wait for the dialog to initialize
	while (sceUtilitySavedataGetStatus() < 2)
    {
#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
        sceKernelDelayThread(100);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
	sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
    }
}



// Runs the savedata dialog loop
void p5_close_savedata()
{
	LOGSTR0("entering savedata dialog loop\n");

	int running = 1;
	int last_status = -1;

	while(running)
	{
		int status = sceUtilitySavedataGetStatus();

		if (status != last_status)
		{
			LOGSTR2("status changed from %d to %d\n", last_status, status);
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
				running = 0;
				break;

			case PSP_UTILITY_DIALOG_FINISHED:
				break;
		}

#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
        sceKernelDelayThread(100);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
	sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
	}

	LOGSTR0("dialog has shut down\n");
}



// Write the p5 memory partition to a file
void p5_dump_memory(char* filename)
{
	SceUID file;
	file = sceIoOpen(filename, PSP_O_CREAT | PSP_O_WRONLY, 0777);
	sceIoWrite(file, (void*)0x08400000, 0x00400000);
	sceIoClose(file);
}


// Check the stub for validity, special version for p5 memory addresses
// Returns !=0 if stub entry is valid, 0 if it's not
int p5_elf_check_stub_entry(tStubEntry* pentry)
{
	return (
    ((u32)(pentry->library_name) > 0x08400000) &&
	((u32)(pentry->library_name) < 0x08800000) &&
	(pentry->nid_pointer) &&
	(pentry->jump_pointer));
}


// Change the stub pointers so that they point into their new memory location
void p5_relocate_stubs(void* destination, void* source)
{
	tStubEntry* pentry = (tStubEntry*)destination;
	int offset = (int)destination - (int)source;

	LOGSTR2("Relocating stub addresses from 0x%08lX to 0x%08lX\n", (u32)source, (u32)destination);

	while (p5_elf_check_stub_entry(pentry))
	{
		if (pentry->import_flags != 0x11)
		{
			// Variable import
			// relocate it to pass the the user memory pointer check
			pentry->library_name = (Elf32_Addr)((int)pentry->library_name + offset);
			pentry = (tStubEntry*)((int)pentry + 4);
		}
		else
		{
			LOGSTR4("current stub: 0x%08lX 0x%08lX 0x%08lX 0x%08lX ", (u32)pentry->library_name, (u32)pentry->import_flags, (u32)pentry->library_version, (u32)pentry->import_stubs);
			LOGSTR3("0x%08lX 0x%08lX 0x%08lX\n", (u32)pentry->stub_size, (u32)pentry->nid_pointer, (u32)pentry->jump_pointer);

			pentry->library_name = (Elf32_Addr)((int)pentry->library_name + offset);
			pentry->nid_pointer = (Elf32_Addr)((int)pentry->nid_pointer + offset);
			pentry->jump_pointer = (Elf32_Addr)((int)pentry->jump_pointer + offset);

			LOGSTR3("relocated to: 0x%08lX 0x%08lX 0x%08lX\n", (u32)pentry->library_name, (u32)pentry->nid_pointer, (u32)pentry->jump_pointer);

			pentry++;
		}
	}
}


// This just copies 128 kiB around the stub address to p2 memory
void p5_copy_stubs(void* destination, void* source)
{
	memcpy((void*)((int)destination - 0x10000), (void*)((int)source - 0x10000), 0x20000);
}



void* p5_find_stub_in_memory(char* library, void* start_address, u32 size)
{
	char* name_address = NULL;
	void* stub_address = NULL;

	name_address = memfindsz(library, (char*)start_address, size);

	if (name_address)
	{
		// Stub pointer is 40 bytes after the library names char[0]
		stub_address = (void*)*(u32*)(name_address + 40);
	}

	return stub_address;
}


// Copy stubs for the savedata dialog with save mode
void p5_copy_stubs_savedata_dialog()
{
	p5_open_savedata(PSP_UTILITY_SAVEDATA_SAVE);

	void* scePaf_Module = p5_find_stub_in_memory("scePaf_Module", (void*)0x084C0000, 0x00010000);
	void* sceVshCommonUtil_Module = p5_find_stub_in_memory("sceVshCommonUtil_Module", (void*)0x08760000, 0x00010000);
	void* sceDialogmain_Module = p5_find_stub_in_memory("sceDialogmain_Module", (void*)0x08770000, 0x00010000);

	//p5_dump_memory("ms0:/p5_savedata_save.dump");

    p5_copy_stubs((void*)RELOC_MODULE_ADDR_1, scePaf_Module);
    p5_copy_stubs((void*)RELOC_MODULE_ADDR_2, sceVshCommonUtil_Module);
    p5_copy_stubs((void*)RELOC_MODULE_ADDR_3, sceDialogmain_Module);

    p5_close_savedata();

    p5_relocate_stubs((void*)RELOC_MODULE_ADDR_1, scePaf_Module);
    p5_relocate_stubs((void*)RELOC_MODULE_ADDR_2, sceVshCommonUtil_Module);
    p5_relocate_stubs((void*)RELOC_MODULE_ADDR_3, sceDialogmain_Module);
}

// Copy stubs for the savedata dialog with autoload mode
void p5_copy_stubs_savedata()
{
	p5_open_savedata(PSP_UTILITY_SAVEDATA_AUTOLOAD);

	void* sceVshSDAuto_Module = p5_find_stub_in_memory("sceVshSDAuto_Module", (void*)0x08410000, 0x00010000);

    p5_copy_stubs((void*)RELOC_MODULE_ADDR_4, sceVshSDAuto_Module);
    p5_close_savedata();
    p5_relocate_stubs((void*)RELOC_MODULE_ADDR_4, sceVshSDAuto_Module);

}

// Copy the dialog stubs from p5 into p2 memory
void p5_get_stubs()
{
	LOGSTR0("p5_get_stubs\n");
#ifndef HOOK_sceKernelVolatileMemUnlock_WITH_dummy
	sceKernelVolatileMemUnlock(0);
#endif
	p5_copy_stubs_savedata();
	p5_copy_stubs_savedata_dialog();
	LOGSTR0("p5_get_stubs DONE\n");
}


// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID hbl_file;
	int num_nids;

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

#ifdef PRE_LOADER_EXEC
	preLoader_Exec();
#endif

#ifdef RESET_HOME_SCREEN_LANGUAGE
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeScreenSettings();
#endif

#if defined(FORCE_FIRST_LOG) || defined(DEBUG)
	// Some exploits need to "trigger" file I/O in order for HBL to work, not sure why
	logstr0("Loader running\n");
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
	CloseFiles();
#endif

	//reset the contents of the debug file;
	init_debug();

	//init global variables
	init_globals();

#ifndef VITA
	int firmware_version = getFirmwareVersion();
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

	if (getPSPModel() == PSP_GO) {
		print_to_screen("PSP Go Detected");
#ifndef DISABLE_KERNEL_DUMP
		// If PSPGo on 6.20+, do a kmem dump
		if (getFirmwareVersion() >= 620)
			get_kmem_dump();
#endif
	}
#endif

	// Get additional syscalls from utility dialogs
	p5_get_stubs();

#ifdef LOAD_MODULES_FOR_SYSCALLS
	load_utility_module(PSP_MODULE_AV_AVCODEC);
#endif

	// Build NID table
	print_to_screen("Building NIDs table");
	num_nids = build_nid_table();
	LOGSTR1("NUM NIDS: %d\n", num_nids);

	if(num_nids <= 0)
		exit_with_log("No Nids ???", NULL, 0);

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));
	else {
		LOGSTR0("Loading HBL\n");
		load_hbl(hbl_file);

		LOGSTR0("Resolving HBL stubs\n");
		resolve_stubs();
		LOGSTR0("HBL stubs copied, running eLoader\n");
		run_eloader(0, NULL);
	}
}

/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include "sdk.h"
#include "loader.h"
#include "debug.h"
#include "elf.h"
#include "config.h"
#include "tables.h"
//#include "scratchpad.h"
#include "utils.h"
#include "eloader.h"

void (*run_eloader)(unsigned long arglen, unsigned long* argp) = 0;

// HBL intermediate buffer
// int hbl_buffer[MAX_ELOADER_SIZE];

// Loads HBL to memory
void load_hbl(SceUID hbl_file)
{	
	unsigned long file_size;
	int bytes_read;
	SceUID HBL_block;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);
	/*
	if (file_size >= MAX_ELOADER_SIZE)
		exit_with_log(" HBL TOO BIG ", &file_size, sizeof(file_size));
	*/
	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));
	
	// Allocate memory for HBL
	// HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, MAX_ELOADER_SIZE, 0x09EC8000);
	// HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_High, file_size, NULL); // Best one, but don't work, why?
	HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, file_size, 0x09EC8000);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);

	/*
	// Write HBL memory block info to scratchpad
	*((SceUID*)ADDR_HBL_BLOCK_UID) = HBL_block;
	*((SceUID*)ADDR_HBL_BLOCK_ADDR) = run_eloader;
	*/

	// Load HBL to buffer
	/*
	if ((bytes_read = sceIoRead(hbl_file, (void*)hbl_buffer, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	*/

	// Load directly to allocated memory
	if ((bytes_read = sceIoRead(hbl_file, (void*)run_eloader, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	
	//DEBUG_PRINT(" HBL LOADED ", &bytes_read, sizeof(bytes_read));

	sceIoClose(hbl_file);

	// Copy HBL to safe memory
	// memcpy((void*)run_eloader, (void*)hbl_buffer, bytes_read);

	LOGSTR1("HBL loaded to allocated memory @ 0x%08lX\n", run_eloader);

	// Commit changes to RAM
	sceKernelDcacheWritebackInvalidateAll();
}

// Fills Stub array
// Returns number of stubs found
// "pentry" points to first stub header in game
// Both lists must have same size
int search_game_stubs(tStubEntry *pentry, u32** stub_list, u32* hbl_imports_list, unsigned int list_size)
{
	int i = 0, j, count = 0;
	u32 *cur_nid, *cur_call;

	LOGSTR1("ENTERING search_game_stubs() 0x%08lX\n", pentry);

	// Zeroing data
	memset(stub_list, 0, list_size * sizeof(u32));

	// While it's a valid stub header
	while(elf_check_stub_entry(pentry))
	{
		// Get current NID and stub pointer
		cur_nid = pentry->nid_pointer;
		cur_call = pentry->jump_pointer;

		// Browse all stubs defined by this header
		for(i=0; i<pentry->stub_size; i++)
		{
			// Compare each NID in game imports with HBL imports
			for(j=0; j<list_size; j++)
			{
				if(hbl_imports_list[j] == *cur_nid)
				{
					stub_list[j] = cur_call;
					LOGSTR3("nid:0x%08lX, address:0x%08lX call:0x%08lX", *cur_nid, cur_call, *cur_call);
					LOGSTR1(" 0x%08lX\n", *(cur_call+1));
					count++;
				}
			}
			cur_nid++;
			cur_call += 2;
		}		
		pentry++;
	}

	return count;
}

// Loads info from config file
int load_imports(u32* hbl_imports)
{
	unsigned int num_imports;
	u32 nid;
	int i = 0, ret;

	// DEBUG_PRINT(" LOADING HBL IMPORTS FROM CONFIG ", NULL, 0);

	ret = config_num_nids_total(&num_imports);

	if(ret < 0)
		exit_with_log(" ERROR READING NUMBER OF IMPORTS ", &ret, sizeof(ret));

	// DEBUG_PRINT(" NUMBER OF IMPORTS ", &num_imports, sizeof(num_imports));

	if(num_imports > NUM_HBL_IMPORTS)
		exit_with_log(" ERROR FILE CONTAINS MORE IMPORTS THAN BUFFER SIZE ", &num_imports, sizeof(num_imports));

	LOGSTR0("--> HBL imports from imports.config:\n");

	// Get NIDs from config
	ret = config_first_nid(&nid);

	// DEBUG_PRINT(" FIRST NID RETURN VALUE ", &ret, sizeof(ret));
	
	do
	{
		if (ret <= 0)
			exit_with_log(" ERROR READING NEXT NID ", &i, sizeof(i));
		
		hbl_imports[i++] = nid;

		LOGSTR2("%d. 0x%08lX\n", i, nid);

		if (i >= num_imports)
			break;
		
		ret = config_next_nid(&nid);		
	} while (1);

	return i;
}

// Copies stubs used by HBL to scratchpad
// This function highly depends on sdk_hbl.S and imports.config contents
// This behaviour must be fixed
void copy_hbl_stubs(void)
{
	// Temp storage
	int i, ret;
	SceUID HBL_stubs_block;
	
	// Where are HBL stubs
	u32* stub_addr;
	
	// HBL imports (NIDs)
	u32 hbl_imports[NUM_HBL_IMPORTS];
	
	// Stub addresses
	// The one sets to 0 are not imported by the game, and will be automatically estimated by HBL when loaded
	// ALL FUNCTIONS USED BEFORE SYSCALL ESTIMATION IS READY MUST BE IMPORTED BY THE GAME
	u32* stub_list[NUM_HBL_IMPORTS];
	
	// Game .lib.stub entry
	u32 pgame_lib_stub;

	// Initialize config	
	ret = config_initialize();
	// DEBUG_PRINT(" CONFIG INITIALIZED ", &ret, sizeof(ret));

	if (ret < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &ret, sizeof(ret));

	// Get game's .lib.stub pointer
	ret = config_first_lib_stub(&pgame_lib_stub);

	if (ret < 0)
		exit_with_log(" ERROR READING LIBSTUB ADDRESS ", &ret, sizeof(ret));
	
	//DEBUG_PRINT(" GOT GAME LIB STUB ", &pgame_lib_stub, sizeof(u32));

	//DEBUG_PRINT(" ZEROING STUBS ", NULL, 0);
	memset(&hbl_imports, 0, sizeof(hbl_imports));

	// Loading HBL imports (NIDs)
	ret = load_imports(hbl_imports);
	//DEBUG_PRINT(" HBL IMPORTS LOADED ", &ret, sizeof(ret));	

	if (ret < 0)
		exit_with_log(" ERROR LOADING IMPORTS FROM CONFIG ", &ret, sizeof(ret));

	ret = search_game_stubs((tStubEntry*) pgame_lib_stub, stub_list, hbl_imports, (unsigned int) NUM_HBL_IMPORTS);	

	if (ret == 0)
		exit_with_log("**ERROR SEARCHING GAME STUBS**", NULL, 0);

	//DEBUG_PRINT(" STUBS SEARCHED ", NULL, 0);

	/*
	// Allocate memory for HBL stubs
	HBL_stubs_block = sceKernelAllocPartitionMemory(2, "ValentineStubs", PSP_SMEM_High, sizeof(u32) * 2 * NUM_HBL_IMPORTS, NULL);
	if(HBL_stubs_block < 0)
		exit_with_log(" ERROR ALLOCATING STUB MEMORY ", &HBL_stubs_block, sizeof(HBL_stubs_block));
	stub_addr = sceKernelGetBlockHeadAddr(HBL_stubs_block);
	
	// Write HBL stubs memory block info to scratchpad
	*((SceUID*)ADDR_HBL_STUBS_BLOCK_UID) = HBL_stubs_block;
	*((SceUID*)ADDR_HBL_STUBS_BLOCK_ADDR) = stub_addr;
	*/

	stub_addr = (u32*)HBL_STUBS_START;

	// DEBUG_PRINT(" COPYING STUBS ", NULL, 0);
	for (i=0; i<NUM_HBL_IMPORTS; i++)
	{
		if (stub_list[i] != NULL)
			memcpy(stub_addr, stub_list[i], sizeof(u32)*2);
		else
			memset(stub_addr, 0, sizeof(u32)*2);
		stub_addr += 2;
		sceKernelDelayThread(100);
	}

	// Config finished
	config_close();

	sceKernelDcacheWritebackInvalidateAll();
}

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

void initSavedata(SceUtilitySavedataParam * savedata)
{
	memset(savedata, 0, sizeof(SceUtilitySavedataParam));
	savedata->base.size = sizeof(SceUtilitySavedataParam);

	savedata->base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	savedata->base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
	savedata->base.graphicsThread = 0x11;
	savedata->base.accessThread = 0x13;
	savedata->base.fontThread = 0x12;
	savedata->base.soundThread = 0x10;

	savedata->mode = PSP_UTILITY_SAVEDATA_LISTLOAD;
	savedata->overwrite = 1;
	savedata->focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST; // Set initial focus to the newest file (for loading)

#if _PSP_FW_VERSION >= 200
	strncpy(savedata->key, key, 16);
#endif

	strcpy(savedata->gameName, "DEMO11111");	// First part of the save name, game identifier name
	strcpy(savedata->saveName, "0000");	// Second part of the save name, save identifier name

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
	savedata->saveNameList = nameMultiple;

	strcpy(savedata->fileName, "DATA.BIN");	// name of the data file

	// Allocate buffers used to store various parts of the save data
	savedata->dataBuf = malloc(DATABUFFLEN);
	savedata->dataBufSize = DATABUFFLEN;
	savedata->dataSize = DATABUFFLEN;
}

// Injects HBL to partition 5
void inject_hbl(SceUID hbl_file, void* addr)
{	
	unsigned long file_size;
	int bytes_read;
	SceUID HBL_block;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);
	
	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	run_eloader = addr;

	// Load directly to allocated memory
	if ((bytes_read = sceIoRead(hbl_file, (void*)run_eloader, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	
	//DEBUG_PRINT(" HBL LOADED ", &bytes_read, sizeof(bytes_read));

	sceIoClose(hbl_file);

	LOGSTR1("HBL loaded @ 0x%08lX\n", run_eloader);

	// Commit changes to RAM
	sceKernelDcacheWritebackInvalidateAll();
}

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{
	SceUID hbl_file;

	LOGSTR0("Loader running\n");

	// Erase previous kernel dump
	init_kdump();

	// If PSPGo, do a kmem dump
	if (getPSPModel() == PSP_GO)
		get_kmem_dump();
	
    //reset the contents of the debug file;
    init_debug();

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));
	
	else
	{
		LOGSTR0("Preparing savedata trick\n");
		
		SceUtilitySavedataParam dialog;
	
		initSavedata(&dialog);
	
		sceUtilitySavedataInitStart(&dialog);
	
		switch(sceUtilitySavedataGetStatus()) 
		{
			case PSP_UTILITY_DIALOG_VISIBLE :

				sceUtilitySavedataUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT :

				sceUtilitySavedataShutdownStart();
				break;
	
			case PSP_UTILITY_DIALOG_FINISHED :
				sceKernelExitGame();
	
			case PSP_UTILITY_DIALOG_NONE :
				sceKernelExitGame;
		}

		LOGSTR0("Savedata trick done!\n");
		LOGSTR0("Injecting HBL\n");
		inject_hbl(hbl_file, (void*)HBL_BUFFER);

		sceUtilitySavedataShutdownStart();

		/*
		LOGSTR0("Loading HBL\n");
		load_hbl(hbl_file);
		*/
	
		LOGSTR0("Copying & resolving HBL stubs\n");
		copy_hbl_stubs();
		
		run_eloader(0, NULL);
	}

	while(1)
		sceKernelDelayThread(100000);
}

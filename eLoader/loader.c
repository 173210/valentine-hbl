/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include "sdk.h"
#include "loader.h"
#include "debug.h"
#include "elf.h"
#include "config.h"
#include "tables.h"
#include "utils.h"
#include "eloader.h"
#include "malloc.h"
#include "globals.h"

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
	
	//if (file_size >= MAX_ELOADER_SIZE)
	//	exit_with_log(" HBL TOO BIG ", &file_size, sizeof(file_size));
	
	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));
	
	// Allocate memory for HBL
	// HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, MAX_ELOADER_SIZE, 0x09EC8000);
	// HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_High, file_size, NULL); // Best one, but don't work, why?
	HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, file_size, (void *)0x09EC8000);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
	
	// Write HBL memory block info to scratchpad
	g->hbl_block_uid = HBL_block;
	g->hbl_block_addr = (u32)run_eloader;

	// Load HBL to buffer
	//if ((bytes_read = sceIoRead(hbl_file, (void*)hbl_buffer, file_size)) < 0)
	//	exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));

	// Load directly to allocated memory
	if ((bytes_read = sceIoRead(hbl_file, (void*)run_eloader, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	
	//DEBUG_PRINT(" HBL LOADED ", &bytes_read, sizeof(bytes_read));

	sceIoClose(hbl_file);

	// Copy HBL to safe memory
	// memcpy((void*)run_eloader, (void*)hbl_buffer, bytes_read);

	LOGSTR1("HBL loaded to allocated memory @ 0x%08lX\n", (u32)run_eloader);

	// Commit changes to RAM
	sceKernelDcacheWritebackInvalidateAll();
}

// Fills Stub array
// Returns number of stubs found
// "pentry" points to first stub header in game
// Both lists must have same size
int search_game_stubs(tStubEntry *pentry, u32** stub_list, u32* hbl_imports_list, unsigned int list_size)
{
	u32 i = 0, j, count = 0;
	u32 *cur_nid, *cur_call;

	LOGSTR1("ENTERING search_game_stubs() 0x%08lX\n", (u32)pentry);

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
					LOGSTR3("nid:0x%08lX, address:0x%08lX call:0x%08lX", *cur_nid, (u32)cur_call, *cur_call);
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
	u32 nid, i = 0;
	int ret;

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


// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
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
    
    //init global variables
    init_globals();

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));
	
	else
	{

		LOGSTR0("Loading HBL\n");
		load_hbl(hbl_file);

	
		LOGSTR0("Copying & resolving HBL stubs\n");
		copy_hbl_stubs();

		run_eloader(0, NULL);
	}

	while(1)
		sceKernelDelayThread(100000);
}

/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include "sdk.h"
#include "loader.h"
#include "debug.h"
#include "elf.h"
#include "config.h"

void (*run_eloader)(unsigned long arglen, unsigned long* argp) = 0;

// HBL intermediate buffer
int hbl_buffer[MAX_ELOADER_SIZE];

// Loads HBL to memory
void load_hbl(SceUID hbl_file)
{	
	SceOff file_size;
	int bytes_read;
	SceUID HBL_block;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);
	if (file_size >= MAX_ELOADER_SIZE)
		exit_with_log(" HBL TOO BIG ", &file_size, sizeof(file_size));
	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));
	
	// Allocate memory for HBL
	HBL_block = sceKernelAllocPartitionMemory(2, "Valentine", PSP_SMEM_Addr, MAX_ELOADER_SIZE, 0x09EC8000);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
	
	// Write HBL memory block info to scratchpad
	*((SceUID*)ADDR_HBL_BLOCK_UID) = HBL_block;
	*((SceUID*)ADDR_HBL_BLOCK_ADDR) = run_eloader;

	// Load HBL to buffer
	if ((bytes_read = sceIoRead(hbl_file, (void*)hbl_buffer, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	
	//DEBUG_PRINT(" HBL LOADED ", &bytes_read, sizeof(bytes_read));

	sceIoClose(hbl_file);

	// Copy HBL to safe memory
	memcpy((void*)run_eloader, (void*)hbl_buffer, bytes_read);

	//write_debug(" HBL COPIED TO SCRATCHPAD ", (void*)hbl_buffer, bytes_read);

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

	DEBUG_PRINT(" ENTERING build_stub_list ", &pentry, sizeof(u32));

	// Zeroing data
	memset(stub_list, 0, list_size * sizeof(u32));

	// While it's a valid stub header
	while(elf_check_stub_entry(pentry))
	{
		// Get current NID and stub pointer
		cur_nid = pentry->nid_pointer;
		cur_call = pentry->jump_pointer;

		/*
		DEBUG_PRINT(" LIBRARY ", pentry->library_name, strlen(pentry->library_name));
		DEBUG_PRINT(" NUM IMPORTS ", &(pentry->stub_size), sizeof(Elf32_Half));
		DEBUG_PRINT(" NID POINTER ", &cur_nid, sizeof(u32));
		DEBUG_PRINT(" NID-CALL ", NULL, 0);
		*/

		// Browse all stubs defined by this header
		for(i=0; i<pentry->stub_size; i++)
		{
			// Compare each NID in game imports with HBL imports
			for(j=0; j<list_size; j++)
			{
				if(hbl_imports_list[j] == *cur_nid)
				{
					DEBUG_PRINT(NULL, cur_nid, sizeof(u32));
					DEBUG_PRINT(NULL, &cur_call, sizeof(u32));
					stub_list[j] = cur_call;
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

	// Get NIDs from config
	ret = config_first_nid(&nid);

	// DEBUG_PRINT(" FIRST NID RETURN VALUE ", &ret, sizeof(ret));
	
	do
	{
		if (ret <= 0)
			exit_with_log(" ERROR READING NEXT NID ", &i, sizeof(i));
		
		hbl_imports[i++] = nid;

		// DEBUG_PRINT(" CURRENT NID ", &(hbl_imports[i]), sizeof(u32));

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
	
	// HBL imports
	u32 hbl_imports[NUM_HBL_IMPORTS];
	
	// Stub addresses
	// The one sets to 0 are not imported by the game, and will be automatically estimated by HBL when loaded
	// ALL FUNCTIONS USED BY HBL BEFORE SYSCALL ESTIMATION IS READY MUST BE IMPORTED BY THE GAME
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
	
	// Allocate memory for HBL stubs
	HBL_stubs_block = sceKernelAllocPartitionMemory(2, "ValentineStubs", PSP_SMEM_High, sizeof(u32) * 2 * NUM_HBL_IMPORTS, NULL);
	if(HBL_stubs_block < 0)
		exit_with_log(" ERROR ALLOCATING STUB MEMORY ", &HBL_stubs_block, sizeof(HBL_stubs_block));
	stub_addr = sceKernelGetBlockHeadAddr(HBL_stubs_block);
	
	// Write HBL stubs memory block info to scratchpad
	*((SceUID*)ADDR_HBL_STUBS_BLOCK_UID) = HBL_stubs_block;
	*((SceUID*)ADDR_HBL_STUBS_BLOCK_ADDR) = stub_addr;

	// DEBUG_PRINT(" COPYING STUBS ", NULL, 0);
	for (i=0; i<NUM_HBL_IMPORTS; i++)
	{
		if (stub_list[i] != NULL)
			memcpy(stub_addr, stub_list[i], sizeof(u32)*2);
		else
			memset(stub_addr, 0, sizeof(u32)*2);
		stub_addr += 2;
		sceKernelDelayThread(100000);
	}

	// Config finished
	config_close();
}

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{
    //reset the contents of the debug file;
    init_debug();
	SceUID hbl_file;

	//DEBUG_PRINT(" LOADER RUNNING ", NULL, 0);	

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));
	
	else
	{
		DEBUG_PRINT(" LOADING HBL ", NULL, 0);
		load_hbl(hbl_file);
	
		DEBUG_PRINT(" COPYING STUBS ", NULL, 0);
		copy_hbl_stubs();
		
		sceKernelDcacheWritebackInvalidateAll();

		DEBUG_PRINT(" PASSING TO HBL ", NULL, 0);
		run_eloader(0, NULL);
	}	

	while(1)
		sceKernelDelayThread(10000000);
}

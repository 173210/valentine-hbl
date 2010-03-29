/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include "sdk.h"
#include "loader.h"
#include "debug.h"
#include "elf.h"
#include "config.h"

// Scratchpad!
void (*run_eloader)(void) = (void*)LOADER_BUFFER;

// HBL intermediate buffer
int hbl_buffer[MAX_ELOADER_SIZE];

// Loads HBL to unused memory
void load_hbl(SceUID hbl_file)
{	
	SceOff file_size;
	int bytes_read;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);
	if (file_size >= MAX_ELOADER_SIZE)
		exit_with_log(" HBL TOO BIG ", &file_size, sizeof(file_size));
	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));		

	// Load HBL to buffer
	if ((bytes_read = sceIoRead(hbl_file, (void*)hbl_buffer, file_size)) < 0)
		exit_with_log(" ERROR READING HBL ", &bytes_read, sizeof(bytes_read));
	
	//write_debug(" HBL LOADED ", &bytes_read, sizeof(bytes_read));

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

	/*
#ifdef DEBUG
	write_debug(" ENTERING build_stub_list ", &pentry, sizeof(u32));
#endif
	*/

	// Zeroing data
	memset(stub_list, 0, list_size);

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

	ret = config_num_nids_total(&num_imports);

	if(ret < 0)
		exit_with_log(" ERROR READING NUMBER OF IMPORTS ", &ret, sizeof(ret));

	if(num_imports > NUM_HBL_IMPORTS)
		exit_with_log(" ERROR FILE CONTAINS MORE IMPORTS THAN BUFFER SIZE ", &num_imports, sizeof(num_imports));

	// Get NIDs from config
	ret = config_first_nid(&nid);

	/*
#ifdef DEBUG
	write_debug("**READING NIDS: **", NULL, 0);
#endif
	*/
	
	do
	{
		hbl_imports[i++] = nid;
		ret = config_next_nid(&nid);
		if (ret < 0)
		{
			write_debug(" ERROR READING NEXT NID ", &i, sizeof(i));
			break;
		}
	} while (i<num_imports);

	return 0;
}

// Copies stubs used by HBL to scratchpad
// This function highly depends on sdk_eloader.S contents
// This behaviour must be modified
void copy_hbl_stubs(void)
{
	// Temp storage
	int i, ret;
	
	// Where are HBL stubs
	u32* scratchpad_stubs = (u32*)SCRATCHPAD_STUBS_START;
	
	// HBL imports
	u32 hbl_imports[NUM_HBL_IMPORTS];
	
	// Stub addresses
	// The one sets to 0 are not imported by the game, and will be automatically estimated by HBL when loaded
	// sceIo* are COMPULSORY because they are used before syscall estimation
	u32* stub_list[NUM_HBL_IMPORTS];
	
	// Game .lib.stub entry
	u32 pgame_lib_stub;

	// Initialize config	
	ret = config_initialize();
	// DEBUG_PRINT(" CONFIG INITIALIZED ", &ret, sizeof(ret));

	if (ret < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &ret, sizeof(ret));

	// Get game's .lib.stub pointer
	ret = config_lib_stub(&pgame_lib_stub);	
	// DEBUG_PRINT(" GOT LIB STUB ", &ret, sizeof(ret));

	if (ret < 0)
		exit_with_log(" ERROR READING LIBSTUB ADDRESS ", &ret, sizeof(ret));

	//DEBUG_PRINT(" ZEROING STUBS ", NULL, 0);
	memset(&hbl_imports, 0, sizeof(hbl_imports));

	// Loading HBL imports (NIDs)
	ret = load_imports(hbl_imports);
	// DEBUG_PRINT(" HBL IMPORTS LOADED ", &ret, sizeof(ret));	

	if (ret < 0)
		exit_with_log(" ERROR LOADING IMPORTS FROM CONFIG ", &ret, sizeof(ret));

	ret = search_game_stubs((tStubEntry*) pgame_lib_stub, stub_list, hbl_imports, (unsigned int) NUM_HBL_IMPORTS);
	// DEBUG_PRINT(" STUBS SEARCHED ", NULL, 0);

	if (ret == 0)
		exit_with_log("**ERROR SEARCHING GAME STUBS**", NULL, 0);

	// DEBUG_PRINT(" COPYING STUBS ", NULL, 0);
	for (i=0; i<NUM_HBL_IMPORTS; i++)
	{
		if (stub_list[i] != NULL)
			memcpy(scratchpad_stubs, stub_list[i], sizeof(u32)*2);
		else
			memset(scratchpad_stubs, 0, sizeof(u32)*2);
		scratchpad_stubs += 2;
		sceKernelDelayThread(100000);
	}

	// Config finished
	config_close();
}

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{
	SceUID hbl_file;

	//DEBUG_PRINT(" LOADER RUNNING ", NULL, 0);	

	if ((hbl_file = sceIoOpen(ELOADER_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));
	
	else
	{
		//write_debug(" LOADING HBL ", NULL, 0);
		load_hbl(hbl_file);
	
		// DEBUG_PRINT(" COPYING STUBS ", NULL, 0);
		copy_hbl_stubs();

		sceKernelDcacheWritebackInvalidateAll();

		// DEBUG_PRINT(" PASSING TO HBL ", NULL, 0);
		run_eloader();		
	}	

	while(1)
		sceKernelDelayThread(10000000);
}
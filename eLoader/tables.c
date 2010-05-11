#include "tables.h"
#include "config.h"
#include "elf.h"
#include "debug.h"

// NID table for resolving imports
HBLNIDTable* nid_table = NULL;
//HBLNIDTable nid_table;

// Auxiliary structure to help with syscall estimation
HBLLibTable* library_table = NULL;
//HBLLibTable library_table;

// Initialize nid_table
void* init_nid_table()
{
	nid_table = malloc(sizeof(HBLNIDTable));

	if (nid_table != NULL)
	{
		memset(nid_table, 0, sizeof(HBLNIDTable));
		LOGSTR1("NID table created @ 0x%08lX\n", nid_table);
	}

	return nid_table;
}

// Initialize library_table
void* init_library_table()
{
	library_table = malloc(sizeof(HBLLibTable));

	if (library_table != NULL)
	{
		memset(library_table, 0, sizeof(HBLLibTable));
		LOGSTR1("Library table created @ 0x%08lX\n", library_table);
	}

	return library_table;
}

// Adds NID entry to nid_table
void add_nid_to_table(u32 nid, u32 call, unsigned int lib_index)
{
	LOGSTR1("Adding NID 0x%08lX to table... ", nid);
	
	// Check if NID already exists in table (by another estimation for example)
	int index = get_nid_index(nid);

	// Doesn't exist, insert new
	if (index < 0)
	{
		if (nid_table->num >= NID_TABLE_SIZE)
		{
			LOGSTR0("WARNING: nid_table full\n");
			return;
		}

		nid_table->table[nid_table->num].nid = nid;
		nid_table->table[nid_table->num].call = call;
		nid_table->table[nid_table->num].lib_index = lib_index;		
		LOGSTR1("Newly added @ %d\n", nid_table->num);
		nid_table->num++;
	}

	// If it exists, just change the old call with the new one
	else
	{
		nid_table->table[index].call = call;
		LOGSTR1("Modified @ %d\n", nid_table->num);
	}

	return;
}

// Returns the index on nid_table for the given call
// NOTE: the whole calling instruction is to be passed
int get_call_index(u32 call)
{
	int i = NID_TABLE_SIZE - 1;
	while(i >= 0 && nid_table->table[i].call != call)
	{
		i--;
	}

	return i;
}

// Gets i-th nid and its associated library
// Returns library name length
int get_lib_nid(int index, char* lib_name, u32* pnid)
{
	int ret;
	unsigned int num_libs, i = 0, count = 0, found = 0;
	tImportedLibrary cur_lib;

	// DEBUG_PRINT("**GETTING NID INDEX:**", &index, sizeof(index));

	index += 1;
	ret = config_num_libraries(&num_libs);
	ret = config_first_library(&cur_lib);

	while (i<num_libs)
	{
		/*
		DEBUG_PRINT("**CURRENT LIB**", cur_lib.library_name, strlen(cur_lib.library_name));
		DEBUG_PRINT(NULL, &(cur_lib.num_imports), sizeof(unsigned int));
		DEBUG_PRINT(NULL, &(cur_lib.nids_offset), sizeof(SceOff));
		*/

		count += cur_lib.num_imports;
		if (index > count)
			ret = config_next_library(&cur_lib);
		else
		{
			ret = config_seek_nid(--index, pnid);
			// DEBUG_PRINT("**SEEK NID**", pnid, sizeof(u32));
			break;
		}
	}

	strcpy(lib_name, cur_lib.library_name);

	return strlen(lib_name);
}

// Return index in NID table for the call that corresponds to the NID pointed by "nid"
// Puts call in call_buffer
u32 get_call_nidtable(u32 nid, u32* call_buffer)
{
	int i;
	
	*call_buffer = 0;
	for(i=0; i<NID_TABLE_SIZE; i++) 
	{
        if(nid == nid_table->table[i].nid)
        {
            if(call_buffer != NULL)
                *call_buffer = nid_table->table[i].call;
            break;
        }
    }
	
	return i;
}

// Return real instruction that makes the system call (jump or syscall)
u32 get_good_call(u32* call_pointer)
{
	// Dirty hack here :P but works
	if(*call_pointer & SYSCALL_MASK_IMPORT)
		call_pointer++;
	return *call_pointer;
}

// Gets lowest syscall from kernel dump
u32 get_klowest_syscall(char* lib_name)
{
	SceUID kdump_fd;
	SceSize kdump_size;
	SceOff lowest_offset = 0;
	u32 lowest_syscall = 0xFFFFFFFF;

	kdump_fd = sceIoOpen(HBL_ROOT"kmem.dump", PSP_O_CREAT | PSP_O_RDONLY, 0777);
	if (kdump_fd >= 0)
	{
		kdump_size = sceIoLseek(kdump_fd, 0, PSP_SEEK_END);

		if (kdump_size > 0)
		{
			if (strcmp(lib_name, "LoadExecForUser") == 0)
				lowest_offset = (SceOff)0x000203AC;
	
			else if (strcmp(lib_name, "UtilsForUser") == 0)
				lowest_offset = (SceOff)0x0002044C;
	
			else if (strcmp(lib_name, "sceSuspendForUser") == 0)
				lowest_offset = (SceOff)0x0002049C;
	
			else if (strcmp(lib_name, "SysMemUserForUser") == 0)
				lowest_offset = (SceOff)0x000204EC;
	
			else if (strcmp(lib_name, "ModuleMgrForUser") == 0)
				lowest_offset = (SceOff)0x000205DC;
	
			else if (strcmp(lib_name, "IoFileMgrForUser") == 0)
				lowest_offset = (SceOff)0x0002071C;
	
			else if (strcmp(lib_name, "StdioForUser") == 0)
				lowest_offset = (SceOff)0x000207BC;
	
			else if (strcmp(lib_name, "ThreadManForUser") == 0)
				lowest_offset = (SceOff)0x000208FC;
	
			else if (strcmp(lib_name, "InterruptManager") == 0)
				lowest_offset = (SceOff)0x0002094C;
	
			else if (strcmp(lib_name, "sceDisplay") == 0)
				lowest_offset = (SceOff)0x0002A8F8;
	
			else if (strcmp(lib_name, "sceRtc") == 0)
				lowest_offset = (SceOff)0x0002B0D8;
	
			else if (strcmp(lib_name, "sceCtrl") == 0)
				lowest_offset = (SceOff)0x000BEDA0;
	
			else if (strcmp(lib_name, "sceWlanDrv_lib") == 0)
				lowest_offset = (SceOff)0x00159DA8;
	
			else if (strcmp(lib_name, "sceWlanDrv") == 0)
				lowest_offset = (SceOff)0x00159E00;
	
			else if (strcmp(lib_name, "sceReg") == 0)
				lowest_offset = (SceOff)0x001889A0;
	
			else if (strcmp(lib_name, "sceUtility") == 0)
				lowest_offset = (SceOff)0x001E5CD0;
	
			else if (strcmp(lib_name, "sceUmdUser") == 0)
				lowest_offset = (SceOff)0x001E6408;
	
			else if (strcmp(lib_name, "sceNetIfhandle_lib") == 0)
				lowest_offset = (SceOff)0x00269D80;
	
			else
				NID_LOGSTR0("Library not hacked\n");			

			if (lowest_offset > 0)
			{
				NID_LOGSTR1("Lowest offset: 0x%08lX\n", lowest_offset);
				sceIoLseek(kdump_fd, lowest_offset, PSP_SEEK_SET);
				sceIoRead(kdump_fd, &lowest_syscall, sizeof(lowest_syscall));
				NID_LOGSTR1("Kernel dump says lowest syscall is: 0x%08lX\n", lowest_syscall);
			}			
		}
		else
		{
			NID_LOGSTR0("Kmemdump empty\n");
		}
		
		sceIoClose(kdump_fd);
	}
	else
		NID_LOGSTR0("Could not open kernel dump\n");

	return lowest_syscall;
}

// Return index in nid_table for closest higher known NID
int get_higher_known_nid(unsigned int lib_index, u32 nid)
{
	int i;
	int higher_index = -1;

	for(i=0; i<NID_TABLE_SIZE; i++)
	{
		if (nid_table->table[i].lib_index == lib_index && nid_table->table[i].nid > nid)
		{
			// First higher found
			if (higher_index < 0)
				higher_index = i;

			// New higher, check if closer
			else if (nid_table->table[i].nid < nid_table->table[higher_index].nid)
				higher_index = i;
		}
	}

	return higher_index;
}

// Return index in nid_table for closest lower known NID
int get_lower_known_nid(unsigned int lib_index, u32 nid)
{
	int i;
	int lower_index = -1;

	for(i=0; i<NID_TABLE_SIZE; i++)
	{
		if (nid_table->table[i].lib_index == lib_index && nid_table->table[i].nid < nid)
		{
			// First higher found
			if (lower_index < 0)
				lower_index = i;

			// New higher, check if closer
			else if (nid_table->table[i].nid > nid_table->table[lower_index].nid)
				lower_index = i;
		}
	}

	return lower_index;
}

// Fills remaining information on a library
tSceLibrary* complete_library(tSceLibrary* plibrary)
{
	char file_path[MAX_LIBRARY_NAME_LENGTH + 100];
	SceUID nid_file;
	u32 nid;
	unsigned int i;
	u32 lowest_syscall;
	int syscall_gap;
	int index;
	
	// Constructing the file path
	strcpy(file_path, LIB_PATH);
	if (getFirmwareVersion() >= 600)
	{
		strcat(file_path, "_6xx/");
	}
	else
	{
		strcat(file_path, "_5xx/");
	}	 
	
	strcat(file_path, plibrary->name);
	strcat(file_path, LIB_EXTENSION);

	NID_LOGSTR0(">> Opening file ");
	NID_LOGSTR0(file_path);
	NID_LOGSTR0("\n");

	// Opening the NID file
	if ((nid_file = sceIoOpen(file_path, PSP_O_RDONLY, 0777)) >= 0)
	{
		// Calculate number of NIDs (size of file/4)
		plibrary->num_library_exports = sceIoLseek(nid_file, 0, PSP_SEEK_END) / sizeof(u32);
		sceIoLseek(nid_file, 0, PSP_SEEK_SET);

		// Search for lowest nid
		i = 0;
		while(sceIoRead(nid_file, &nid, sizeof(u32)) > 0)
		{
			if (plibrary->lowest_nid == nid)
			{
				plibrary->lowest_index = i;
				break;
			}
			i++;
		}

		NID_LOGSTR0("Getting lowest syscall from kernel dump\n");
		lowest_syscall = get_klowest_syscall(plibrary->name);
		if ((lowest_syscall & SYSCALL_MASK_RESOLVE) == 0)
		{
			syscall_gap = plibrary->lowest_syscall - lowest_syscall;
			if (syscall_gap < 0)
			{
				NID_LOGSTR2("WARNING: lowest syscall in kernel (0x%08lX) is bigger than found lowest syscall in tables (0x%08lX)\n", lowest_syscall, 
				        plibrary->lowest_syscall);
			}
			
			else if (syscall_gap == 0)
			{
				NID_LOGSTR0("Nothing to do, lowest syscall from kernel is the same from tables\n");
			}
			
			// Find NID for lowest syscall
			else
			{
				index = plibrary->lowest_index - (unsigned int)syscall_gap;

				// Rotation ;)
				if (index < 0)
					index = plibrary->num_library_exports + index;				
				
				sceIoLseek(nid_file, index * 4, PSP_SEEK_SET);
				sceIoRead(nid_file, &nid, sizeof(u32));

				plibrary->lowest_index = index;
				plibrary->lowest_nid = nid;
				plibrary->lowest_syscall = lowest_syscall;
			}
		}
		
		sceIoClose(nid_file);

		return plibrary;
	}
	else
		return NULL;
}

// Returns index of NID in table
int get_nid_index(u32 nid)
{
	int i;

	for (i=0; i<NID_TABLE_SIZE; i++)
	{
		if (nid_table->table[i].nid == 0)
			break;
		else if (nid_table->table[i].nid == nid)
		    return i;
	}

	return -1;
}

// Checks if a library is in the global library description table
// Returns index if it's there
int get_library_index(char* library_name)
{
	LOGSTR1("Searching for library %s\n", library_name);
	if (library_name == NULL)
        return -1;

	int i;
    for (i=0; i<MAX_LIBRARIES; i++)
    {
		//LOGSTR1("Current library: %s\n", library_table->table[i].library_name);
        if (library_table->table[i].name == NULL)
            break;
        else if (strcmp(library_name, library_table->table[i].name) == 0)
            return i;
    }
    
	return -1;
}

/*
 * Retrieves highest known syscall of the previous library,
 * and lowest known syscall of the next library, to get some
 * rough boundaries of where current library's syscalls should be
 * returns 1 on success, 0 on failure
*/
int get_syscall_boundaries(int lib_index, u32* low, u32* high)
{
    tSceLibrary my_lib;
    int i;
    
    my_lib = library_table->table[lib_index];
    *low = 0;
    *high = 0;
    
    for (i=0; i<MAX_LIBRARIES; i++)
    {
        if (i == lib_index)
            continue;
        
        if (library_table->table[i].name == NULL)
            break;
            
        tSceLibrary lib = library_table->table[i];    
        u32 l = lib.lowest_syscall;
        u32 h = lib.highest_syscall;
        
        if (l > my_lib.highest_syscall && (l < *high || *high == 0))
            *high = l;
            
        if (h < my_lib.lowest_syscall && h > *low)
            *low = h;
    }
  
    if (*low && *high) return 1;
    return 0;
}

/* Fills NID Table */
/* Returns NIDs resolved */
/* "pentry" points to first stub header in game */
int build_nid_table()
{
	int i = 0, j, k = 0, library_index = -1;
	unsigned int nlib_stubs;
	u32 *cur_nid, *cur_call, syscall_num, good_call, aux1, aux2, nid;
	tSceLibrary *ret;
	tStubEntry *pentry;	

	// Initializing global tables
	if (init_nid_table() == NULL)
		exit_with_log(" ERROR: COULDN'T ALLOCATE NID TABLE MEMORY ", NULL, 0);
	
	if (init_library_table() == NULL)
		exit_with_log(" ERROR: COULDN'T ALLOCATE LIBRARY TABLE MEMORY ", NULL, 0);

	/*
	init_nid_table();
	init_library_table();
	*/
	
	// Getting game's .lib.stub address
	if (aux1 = config_initialize() < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &aux1, sizeof(aux1));

	if (aux1 = config_num_lib_stub(&nlib_stubs) < 0)
	    exit_with_log(" ERROR READING NUMBER OF LIBSTUBS FROM CONFIG ", &aux1, sizeof(aux1));

	if (nlib_stubs == 0)
		exit_with_log(" ERROR: NO LIBSTUBS DEFINED IN CONFIG ", NULL, 0);

	if (aux2 = config_first_lib_stub(&aux1) < 0)
		exit_with_log(" ERROR GETTING FIRST LIBSTUB FROM CONFIG ", &aux2, sizeof(aux2));

	pentry = (tStubEntry*) aux1;

	//DEBUG_PRINT(" build_nid_table() ENTERING MAIN LOOP ", NULL, 0);

	do
	{
		NID_LOGSTR1("-->CURRENT MODULE LIBSTUB: 0x%08lX\n", pentry);
		
		// While it's a valid stub header
		while (elf_check_stub_entry(pentry))
		{
			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if ((i >= NID_TABLE_SIZE) || (k >= MAX_LIBRARIES))
			{
				config_close();
				LOGSTR1(" NID TABLE COUNTER: 0x%08lX\n", i);
				LOGSTR1(" LIBRARY TABLE COUNTER: 0x%08lX\n", k);
				LOGSTR0(" NID/LIBRARY TABLES TOO SMALL ");
				sceKernelExitGame();
			}			

			NID_LOGSTR1("-->Processing library: %s ", pentry->library_name);

			// Get actual call
			cur_call = pentry->jump_pointer;
			good_call = get_good_call(cur_call);

			// Only process if syscall
			if (!(good_call & SYSCALL_MASK_RESOLVE))
			{		
				// Get current NID
				cur_nid = pentry->nid_pointer;

				// Is this library on the table?
				library_index = get_library_index(pentry->library_name);

				// New library
				if (library_index < 0)
				{
					NID_LOGSTR1(" --> New: %d\n", k);
			
					strcpy(library_table->table[k].name, pentry->library_name);
					library_table->table[k].calling_mode = SYSCALL_MODE;
			
					// Get number of syscalls imported
					library_table->table[k].num_known_exports = pentry->stub_size;

					NID_LOGSTR1("Total known exports: %d\n", library_table->table[k].num_known_exports);
			
					// Initialize lowest syscall on library table				
					library_table->table[k].lowest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));
					library_table->table[k].lowest_nid = *cur_nid;
					NID_LOGSTR2("Initial lowest nid/syscall: 0x%08lX/0x%08lX \n", library_table->table[k].lowest_syscall, library_table->table[k].lowest_nid);
	
		            // Initialize highest syscall on library table	
		            library_table->table[k].highest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));
		            
					// Browse all stubs defined by this header
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						NID_LOGSTR1("--Current NID: 0x%08lX", nid);
				
						// If NID is already in, don't put it again 
						if (get_nid_index(nid) < 0)
						{
							// Fill NID table
							add_nid_to_table(nid, get_good_call(cur_call), k);
							NID_LOGSTR1(" --> new inserted @ %d", i);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(nid_table->table[i].call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);
					
							if (syscall_num < library_table->table[k].lowest_syscall)
							{
								library_table->table[k].lowest_syscall = syscall_num;
								library_table->table[k].lowest_nid = nid_table->table[i].nid;
								NID_LOGSTR2("\nNew lowest syscall/nid: 0x%08lX/0x%08lX", library_table->table[k].lowest_syscall, library_table->table[k].lowest_nid);
							}
		                    
		                    if (syscall_num > library_table->table[k].highest_syscall)
							{
								library_table->table[k].highest_syscall = syscall_num;
								NID_LOGSTR2("\nNew highest syscall/nid: 0x%08lX/0x%08lX", library_table->table[k].highest_syscall, nid);
							}
							NID_LOGSTR0("\n");
							i++;
						}
						cur_nid++;
						cur_call += 2;
					}

					// Fill remaining data
					NID_LOGSTR0("Completing library...\n");
					ret = complete_library(&(library_table->table[k]));

					// New library
					library_table->num++;

					// Next library entry
					k++;
				}

				// Old library
				else
				{
					NID_LOGSTR1(" --> Old: %d\n", library_index);

					LOGLIB(library_table->table[library_index]);
					
					NID_LOGSTR1("Number of imports of this stub: %d\n", pentry->stub_size);

					NID_LOGSTR2("Current lowest nid/syscall: 0x%08lX/0x%08lX \n", library_table->table[library_index].lowest_syscall, library_table->table[library_index].lowest_nid);
			
					if (library_table->table[library_index].calling_mode != SYSCALL_MODE)
					{
						config_close();
						exit_with_log(" ERROR OLD CALL MODE IS SYSCALL, NEW IS JUMP ", &library_index, sizeof(library_index));
					}
			
					// Browse all stubs defined by this header
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						NID_LOGSTR1("--Current NID: 0x%08lX", nid);
				
						// If NID is already in, don't put it again 
						if (get_nid_index(nid) < 0)
						{
							// Fill NID table
							add_nid_to_table(nid, get_good_call(cur_call), library_index);
							NID_LOGSTR1(" --> new inserted @ %d", i);
				
							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(nid_table->table[i].call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);
							if (syscall_num < library_table->table[library_index].lowest_syscall)
							{
								library_table->table[library_index].lowest_syscall = syscall_num;
								library_table->table[library_index].lowest_nid = nid_table->table[i].nid;
								NID_LOGSTR2("\nNew lowest nid/syscall: 0x%08lX/0x%08lX", library_table->table[library_index].lowest_syscall, library_table->table[library_index].lowest_nid);
							}
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;
							library_table->table[library_index].num_known_exports++;
						}
						NID_LOGSTR0("\n");
						cur_nid++;
						cur_call += 2;
					}
				}
			}

			// Next entry
			pentry++;

			sceKernelDcacheWritebackInvalidateAll();
		}
		
		nlib_stubs--;

		// Next .lib.stub
		if (nlib_stubs > 0)
		{
			aux2 = config_next_lib_stub(&aux1);
			if (aux2 < 0)
				exit_with_log(" ERROR GETTING NEXT LIBSTUB FROM CONFIG ", &aux2, sizeof(aux2));
			pentry = (tStubEntry*) aux1;
		}
		else
			break;

		sceKernelDelayThread(100000);
		
	} while(1);

#ifdef DEBUG
	int c1, c2;
	u32 syscall;
	unsigned int line_count = 0;
	
	NID_LOGSTR0("\n==LIBRARY TABLE DUMP==\n");
	c1 = 0;
	while (c1 < k)
	{
		NID_LOGSTR1("->Index: %d\n", c1);
		LOGLIB(library_table->table[c1]);

		NID_LOGSTR0("\nNID list:\n");
		c2 = 0;
		while (c2 <= i)
		{
			if (nid_table->table[c2].lib_index == c1)
			{
				if (nid_table->table[c2].call & SYSCALL_MASK_RESOLVE)
					syscall = nid_table->table[c2].call;
				else
					syscall = GET_SYSCALL_NUMBER(nid_table->table[c2].call);
				NID_LOGSTR3("[%d] 0x%08lX 0x%08lX ", c2, nid_table->table[c2].nid, syscall);
				
				line_count++;
				if (line_count == 3)
				{
					line_count = 0;
					NID_LOGSTR0("\n");
				}
			}
			c2++;
		}
		c1++;
		NID_LOGSTR0("\n\n");
	}
#endif

	config_close();

	nid_table->num = i;
	
	return i;
}
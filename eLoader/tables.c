#include "tables.h"
#include "config.h"
#include "elf.h"
#include "debug.h"

// NID table for resolving imports
tNIDResolver nid_table[NID_TABLE_SIZE];

// Auxiliary structure to help with syscall estimation
tSceLibrary library_table[MAX_LIBRARIES];

// Returns the index on nid_table for the given call
// NOTE: the whole calling instruction is to be passed
int get_call_index(u32 call)
{
	int i = 0;

	while(i < NID_TABLE_SIZE && nid_table[i].call != call)
		i++;

	if (i == NID_TABLE_SIZE)
		return -1;
	else
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
        if(nid == nid_table[i].nid)
        {
            if(call_buffer != NULL)
                *call_buffer = nid_table[i].call;
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

// Fills remaining information on a library
tSceLibrary* complete_library(tSceLibrary* plibrary)
{
	char file_path[MAX_LIBRARY_NAME_LENGTH + 20];
	SceUID nid_file;
	u32 nid;
	unsigned int i;

	// Constructing the file path
	strcpy(file_path, LIB_PATH);
	strcat(file_path, plibrary->library_name);
	strcat(file_path, LIB_EXTENSION);

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
		if (nid_table[i].nid == 0)
			break;
		else if (nid_table[i].nid == nid)
		    return i;
	}

	return -1;
}

// Returns a pointer to the library descriptor
tSceLibrary* get_library_entry(const char* library_name)
{
	unsigned int i;

	if (library_name != NULL)
	{
		for (i=0; i<MAX_LIBRARIES; i++)
		{
			if (library_table[i].library_name == NULL)
				break;
			if (strcmp(library_name, library_table[i].library_name) == 0)
				return &(library_table[i]);
		}
	}

	return NULL;
}

// Checks if a library is in the global library description table
// Returns index if it's there
int get_library_index(char* library_name)
{
	int i;

	if (library_name != NULL)
	{
		for (i=0; i<MAX_LIBRARIES; i++)
		{
			if (library_table[i].library_name == NULL)
				break;
			else if (strcmp(library_name, library_table[i].library_name) == 0)
				return i;
		}
	}

	return -1;
}

/* Fills NID Table */
/* Returns NIDs resolved */
/* "pentry" points to first stub header in game */
int build_nid_table(tNIDResolver *nid_table)
{
	int i = 0, j, k = 0, library_index = -1;
	unsigned int nlib_stubs;
	u32 *cur_nid, *cur_call, syscall_num, good_call, aux1, aux2, nid;
	tSceLibrary *ret;
	tStubEntry *pentry;	
	
	// Zeroing global tables
	memset(nid_table, 0, sizeof(nid_table));
	memset(library_table, 0, sizeof(library_table));
	
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
		//DEBUG_PRINT(" CURRENT LIBSTUB ", &pentry, sizeof(u32));
		
		// While it's a valid stub header
		while (elf_check_stub_entry(pentry))
		{
			//DEBUG_PRINT(" VALID STUB ", NULL, 0);
			
			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if ((i >= NID_TABLE_SIZE) || (k >= MAX_LIBRARIES))
			{
				config_close();
				write_debug(" NID TABLE COUNTER ", &i, sizeof(i));
				write_debug(" LIBRARY TABLE COUNTER ", &k, sizeof(k));
				exit_with_log(" NID/LIBRARY TABLES TOO SMALL ", NULL, 0);
			}			
		
			// Get current NID and resolved syscall/jump pointer
			cur_nid = pentry->nid_pointer;
			cur_call = pentry->jump_pointer;

			// Is this library on the table?
			library_index = get_library_index(pentry->library_name);

			//DEBUG_PRINT(" CURRENT LIBRARY ", NULL, 0);
			//DEBUG_PRINT(pentry->library_name, NULL, 0);

			// New library
			if (library_index < 0)
			{
				//DEBUG_PRINT(" LIBRARY NOT ON TABLE ", NULL, 0);	
				//DEBUG_PRINT(" CURRENT LIBRARY POSITION ", &k, sizeof(k));
				
				strcpy(library_table[k].library_name, pentry->library_name);
				
				good_call = get_good_call(cur_call);
				
				// Initialize lowest syscall on library table				
				library_table[k].lowest_syscall = GET_SYSCALL_NUMBER(good_call);
				library_table[k].lowest_nid = *cur_nid;

				// Get number of syscalls imported
				library_table[k].num_known_exports = pentry->stub_size;			

				// DEBUG_PRINT(" NEW LIBRARY ", (void*)library_table[k].library_name, strlen(library_table[k].library_name));

				// JUMP call
				if (good_call & SYSCALL_MASK_RESOLVE)
				{
					library_table[k].calling_mode = JUMP_MODE;
			
					// Browse all stubs defined by this header and fill NID table
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						
						// Only insert NID if it's a new one
						if (get_nid_index(nid) < 0)
						{
							nid_table[i].nid = nid;			
							nid_table[i].call = get_good_call(cur_call);						
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;							
						}
						cur_nid++;
						cur_call += 2;
					}
				}

				// SYSCALL call
				else
				{					
					library_table[k].calling_mode = SYSCALL_MODE;
			
					// Browse all stubs defined by this header
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						
						// If NID is already in, don't put it again 
						if (get_nid_index(nid) < 0)
						{
							// Fill NID table
							nid_table[i].nid = nid;			
							nid_table[i].call = get_good_call(cur_call);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(nid_table[i].call);
							if (syscall_num < library_table[k].lowest_syscall)
							{
								library_table[k].lowest_syscall = syscall_num;
								library_table[k].lowest_nid = nid_table[i].nid;
							}
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;
						}
						cur_nid++;
						cur_call += 2;
					}

					// Fill remaining data
					ret = complete_library(&(library_table[k]));
				}			

				// Next library entry
				k++;
			}

			// Old library
			else
			{
				//DEBUG_PRINT(" LIBRARY ON TABLE ", NULL, 0);	
				//DEBUG_PRINT(" LIBRARY POSITION ", &library_index, sizeof(library_index));

				good_call = get_good_call(cur_call);

				// JUMP
				if (good_call & SYSCALL_MASK_RESOLVE)
				{
					if (library_table[library_index].calling_mode != JUMP_MODE)
					{
						config_close();
						exit_with_log(" ERROR OLD CALL MODE IS JUMP, NEW IS SYSCALL ", &library_index, sizeof(library_index));
					}
					
					// Browse all stubs defined by this header and fill NID table
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						
						// Only insert NID if it's a new one
						if (get_nid_index(nid) < 0)
						{
							nid_table[i].nid = nid;			
							nid_table[i].call = get_good_call(cur_call);
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;
							library_table[library_index].num_known_exports++;
						}
						cur_nid++;
						cur_call += 2;
					}
				}

				// SYSCALL
				else
				{
					if (library_table[library_index].calling_mode != SYSCALL_MODE)
					{
						config_close();
						exit_with_log(" ERROR OLD CALL MODE IS SYSCALL, NEW IS JUMP ", &library_index, sizeof(library_index));
					}
					
					// Browse all stubs defined by this header
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						
						// If NID is already in, don't put it again 
						if (get_nid_index(nid) < 0)
						{
							// Fill NID table
							nid_table[i].nid = nid;			
							nid_table[i].call = get_good_call(cur_call);
						
							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(nid_table[i].call);
							if (syscall_num < library_table[library_index].lowest_syscall)
							{
								library_table[library_index].lowest_syscall = syscall_num;
								library_table[library_index].lowest_nid = nid_table[i].nid;
							}
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;
							library_table[library_index].num_known_exports++;
						}
						cur_nid++;
						cur_call += 2;
					}
				}
			}

			// Next entry
			pentry++;
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
	int c = 0;
	u32 syscall;
	unsigned int line_count = 0;
	LOGSTR0("==NID TABLE (NID/SYSCALL NUMBER)==\n");
	while (c <= i)
	{
		line_count++;
		if (nid_table[c].call & SYSCALL_MASK_RESOLVE)
			syscall = nid_table[c].call;
		else
			syscall = GET_SYSCALL_NUMBER(nid_table[c].call);
        LOGSTR2("0x%08lX 0x%08lX ", nid_table[c++].nid, syscall);
		if (line_count == 3)
		{
			line_count = 0;
			LOGSTR0("\n");
		}
	}
	LOGSTR0("==LIBRARY TABLE DUMP==\n");
	c = 0;
	while (c <= k)
		write_debug(" ", &library_table[c++], sizeof(tSceLibrary));
#endif

	config_close();

	LOGSTR0("build_nid_table() EXITING\n");

	return i;
}
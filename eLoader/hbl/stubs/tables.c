#include <hbl/stubs/tables.h>
#include <common/config.h>
#include <hbl/mod/elf.h>
#include <common/debug.h>
#include <common/malloc.h>
#include <common/lib.h>
#include <common/utils.h>
#include <common/globals.h>
#include <hbl/stubs/syscall.h>
#include <hbl/utils/graphics.h>
#include <common/runtime_stubs.h>
#include <exploit_config.h>

#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef AUTO_SEARCH_STUBS
#define AUTO_SEARCH_STUBS
#endif
#endif

// Adds NID entry to nid_table
int add_nid_to_table(u32 nid, u32 call, unsigned int lib_index)
{
    tGlobals * g = get_globals();
	NID_LOGSTR1("Adding NID 0x%08lX to table... ", nid);

	// Check if NID already exists in table (by another estimation for example)
	int index = get_nid_index(nid);

	// Doesn't exist, insert new
	if (index < 0)
	{
		if (g->nid_table.num >= NID_TABLE_SIZE)
		{
            exit_with_log("FATAL:NID TABLE IS FULL", NULL, 0);
			return -1;
		}

		g->nid_table.table[g->nid_table.num].nid = nid;
		g->nid_table.table[g->nid_table.num].call = call;
		g->nid_table.table[g->nid_table.num].lib_index = lib_index;
		NID_LOGSTR1("Newly added @ %d\n", g->nid_table.num);
		g->nid_table.num++;
	}

	// If it exists, just change the old call with the new one
	else
	{
		g->nid_table.table[index].call = call;
		NID_LOGSTR1("Modified @ %d\n", g->nid_table.num);
	}

	return index;
}

// Adds a new library
int add_library_to_table(const tSceLibrary lib)
{
	tGlobals* g = get_globals();

	// Check if library exists
	int index = get_library_index(lib.name);

	// Doesn't exist, insert new
	if (index < 0)
	{
		// Check if there's space on the table
		if (g->library_table.num >= MAX_LIBRARIES)
		{
			LOGSTR1("->WARNING: Library table full, skipping new library %s\n", (u32)lib.name);
			return -1;
		}

		index = g->library_table.num;
        g->library_table.num++;
	}

	// Copying new library
	memcpy(&(g->library_table.table[index]), &lib, sizeof(lib));


	LOGSTR2("Added library %s @ %d\n", (u32)g->library_table.table[index].name, index);
	LOGLIB(g->library_table.table[index]);

	return index;
}

// Returns the index on nid_table for the given call
// NOTE: the whole calling instruction is to be passed
int get_call_index(u32 call)
{
    tGlobals * g = get_globals();
	int i = NID_TABLE_SIZE - 1;
	while(i >= 0 && g->nid_table.table[i].call != call)
	{
		i--;
	}

	return i;
}

// Gets i-th nid and its associated library
// Returns library name length
int get_lib_nid(u32 index, char* lib_name, u32* pnid)
{
	unsigned int num_libs, i = 0, count = 0;
	tImportedLibrary cur_lib;

	// DEBUG_PRINT("**GETTING NID INDEX:**", &index, sizeof(index));

	index += 1;
	config_num_libraries(&num_libs);
	config_first_library(&cur_lib);

	while (i<num_libs)
	{
		/*
		DEBUG_PRINT("**CURRENT LIB**", cur_lib.library_name, strlen(cur_lib.library_name));
		DEBUG_PRINT(NULL, &(cur_lib.num_imports), sizeof(unsigned int));
		DEBUG_PRINT(NULL, &(cur_lib.nids_offset), sizeof(SceOff));
		*/

		count += cur_lib.num_imports;
		if (index > count)
			config_next_library(&cur_lib);
		else
		{
			config_seek_nid(--index, pnid);
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
    tGlobals * g = get_globals();
	int i;

	*call_buffer = 0;
	for(i=0; i<NID_TABLE_SIZE; i++)
	{
        if(nid == g->nid_table.table[i].nid)
        {
            if(call_buffer != NULL)
                *call_buffer = g->nid_table.table[i].call;
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


int get_library_index_from_name(char* name)
{
	if (strcmp(name, "InterruptManager") == 0)
		return LIBRARY_INTERRUPTMANAGER;
	else if (strcmp(name, "ThreadManForUser") == 0)
		return LIBRARY_THREADMANFORUSER;
	else if (strcmp(name, "StdioForUser") == 0)
		return LIBRARY_STDIOFORUSER;
	else if (strcmp(name, "IoFileMgrForUser") == 0)
		return LIBRARY_IOFILEMGRFORUSER;
	else if (strcmp(name, "ModuleMgrForUser") == 0)
		return LIBRARY_MODULEMGRFORUSER;
	else if (strcmp(name, "SysMemUserForUser") == 0)
		return LIBRARY_SYSMEMUSERFORUSER;
	else if (strcmp(name, "sceSuspendForUser") == 0)
		return LIBRARY_SCESUSPENDFORUSER;
	else if (strcmp(name, "UtilsForUser") == 0)
		return LIBRARY_UTILSFORUSER;
	else if (strcmp(name, "LoadExecForUser") == 0)
		return LIBRARY_LOADEXECFORUSER;
	else if (strcmp(name, "sceGe_user") == 0)
		return LIBRARY_SCEGE_USER;
	else if (strcmp(name, "sceRtc") == 0)
		return LIBRARY_SCERTC;
	else if (strcmp(name, "sceAudio") == 0)
		return LIBRARY_SCEAUDIO;
	else if (strcmp(name, "sceDisplay") == 0)
		return LIBRARY_SCEDISPLAY;
	else if (strcmp(name, "sceCtrl") == 0)
		return LIBRARY_SCECTRL;
	else if (strcmp(name, "sceHprm") == 0)
		return LIBRARY_SCEHPRM;
	else if (strcmp(name, "scePower") == 0)
		return LIBRARY_SCEPOWER;
	else if (strcmp(name, "sceOpenPSID") == 0)
		return LIBRARY_SCEOPENPSID;
	else if (strcmp(name, "sceWlanDrv") == 0)
		return LIBRARY_SCEWLANDRV;
	else if (strcmp(name, "sceUmdUser") == 0)
		return LIBRARY_SCEUMDUSER;
	else if (strcmp(name, "sceUtility") == 0)
		return LIBRARY_SCEUTILITY;
	else if (strcmp(name, "sceSasCore") == 0)
		return LIBRARY_SCESASCORE;
	else if (strcmp(name, "sceImpose") == 0)
		return LIBRARY_SCEIMPOSE;
	else if (strcmp(name, "sceReg") == 0)
		return LIBRARY_SCEREG;
	else if (strcmp(name, "sceChnnlsv") == 0)
		return LIBRARY_SCECHNNLSV;
	else
		return -1;
}


int get_library_kernel_memory_offset(char* library_name)
{
#ifdef SYSCALL_KERNEL_OFFSETS_620
	u32 offset_620[] = SYSCALL_KERNEL_OFFSETS_620;
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_630
	u32 offset_630[] = SYSCALL_KERNEL_OFFSETS_630;
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_635
	u32 offset_635[] = SYSCALL_KERNEL_OFFSETS_635;
#endif

	int library_index = get_library_index_from_name(library_name);

	if (library_index > -1)
	{
		switch (getFirmwareVersion())
		{
#ifdef SYSCALL_KERNEL_OFFSETS_620
			case 620:
				return offset_620[library_index];
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_630

			case 630:
			case 631:
				return offset_630[library_index];
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_635
			case 635:
				return offset_635[library_index];
#endif
		}
	}

	return -1;
}


int get_library_offset(char* library_name, int is_cfw)
{
#ifdef SYSCALL_OFFSETS_570
	short offset_570[] = SYSCALL_OFFSETS_570;
#endif
#ifdef SYSCALL_OFFSETS_570_GO
	short offset_570_go[] = SYSCALL_OFFSETS_570_GO;
#endif
#ifdef SYSCALL_OFFSETS_500
	short offset_500[] = SYSCALL_OFFSETS_500;
#endif
#ifdef SYSCALL_OFFSETS_500_CFW
	short offset_500_cfw[] = SYSCALL_OFFSETS_500_CFW;
#endif
#ifdef SYSCALL_OFFSETS_550
	short offset_550[] = SYSCALL_OFFSETS_550;
#endif
#ifdef SYSCALL_OFFSETS_550_CFW
	short offset_550_cfw[] = SYSCALL_OFFSETS_550_CFW;
#endif
#ifdef SYSCALL_OFFSETS_600
	short offset_600[] = SYSCALL_OFFSETS_600;
#endif
#ifdef SYSCALL_OFFSETS_600_GO
	short offset_600_go[] = SYSCALL_OFFSETS_600_GO;
#endif

	int library_index = get_library_index_from_name(library_name);

	if (library_index > -1)
	{
		switch (getFirmwareVersion())
		{
			case 500:
			case 503:
				if (is_cfw)
#ifdef SYSCALL_OFFSETS_500_CFW
					return offset_500_cfw[library_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_500
					return offset_500[library_index];
#else
					break;
#endif

			case 550:
			case 551:
			case 555:
				if (is_cfw)
#ifdef SYSCALL_OFFSETS_550_CFW
					return offset_550_cfw[library_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_550
					return offset_550[library_index];
#else
					break;
#endif

			case 570:
				if (getPSPModel() == PSP_GO)
#ifdef SYSCALL_OFFSETS_570_GO
					return offset_570_go[library_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_570
					return offset_570[library_index];
#else
					break;
#endif

			case 600:
			case 610:
				if (getPSPModel() == PSP_GO)
#ifdef SYSCALL_OFFSETS_600_GO
					return offset_600_go[library_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_600
					return offset_600[library_index];
#else
					break;
#endif
		}
	}

	return -1;
}


// Gets lowest syscall from kernel dump
u32 get_klowest_syscall(tSceLibrary* library, int reference_library_index, int is_cfw)
{
	library->gap = 0; // For < 6.20 and in case of error

	u32 lowest_syscall = 0xFFFFFFFF;
	tGlobals * g = get_globals();

	LOGSTR1("Get lowest syscall for %s\n", (u32)library->name);

	if (g->syscalls_known)
	{
		if (getFirmwareVersion() <= 610)
		{
			// Syscalls can be calculated from the library offsets on FW <= 6.10
			tSceLibrary* lib_base = &(g->library_table.table[reference_library_index]);

			int base_syscall = lib_base->lowest_syscall - get_library_offset(lib_base->name, is_cfw);

			lowest_syscall = base_syscall + get_library_offset(library->name, is_cfw);

			LOGSTR1("Lowest syscall is %d\n", lowest_syscall);
		}
#ifdef XMB_LAUNCHER
		else if (getFirmwareVersion() >= 660)
		{
			// Lowest syscall and gap is extracted from the launcher imports
			if (library->lowest_index == 0)
			{
				// Library does not wrap around
				library->gap = 0;
			}
			else
			{
				library->gap = (library->highest_syscall - library->lowest_syscall) - (library->num_library_exports - 1);
			}

			lowest_syscall = library->lowest_syscall;
		}
#endif
		else
		{
			// Read the lowest syscalls from the GO kernel memory dump, only for 6.20+
			SceUID kdump_fd;
			SceSize kdump_size;
			SceOff lowest_offset = 0;

			kdump_fd = sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_RDONLY, 0777);
			if (kdump_fd >= 0)
			{
				kdump_size = sceIoLseek(kdump_fd, 0, PSP_SEEK_END);

				if (kdump_size > 0)
				{
					lowest_offset = get_library_kernel_memory_offset(library->name);

					if (lowest_offset > 0)
					{
						HBLKernelLibraryStruct library_info;

						LOGSTR1("Lowest offset: 0x%08lX\n", lowest_offset);
						sceIoLseek(kdump_fd, lowest_offset, PSP_SEEK_SET);
						sceIoRead(kdump_fd, &library_info, sizeof(HBLKernelLibraryStruct));
						LOGSTR3("Kernel dump says: lowest syscall = 0x%08lX, gap = %d, offset to index 0 = %d\n", library_info.lowest_syscall, library_info.gap, library_info.offset_to_zero_index);

						// If the library doesn't wrap around, report the gap as 0 to avoid
						// having to deal with lowest syscalls that lie inside the gap.
						if (library_info.offset_to_zero_index > library_info.gap)
						{
							lowest_syscall = library_info.lowest_syscall;
							library->gap = library_info.gap;
						}
						else
						{
							lowest_syscall = library_info.lowest_syscall + library_info.offset_to_zero_index;
							library->gap = 0;
							LOGSTR0("->library doesn't wrap, gap set to 0\n");
						}
					}
				}
				else
				{
					LOGSTR0("Kmemdump empty\n");
				}

				sceIoClose(kdump_fd);
			}
			else
			{
				LOGSTR0("Could not open kernel dump\n");
			}
		}
	}

	return lowest_syscall;
}

// Return index in nid_table for closest higher known NID
int get_higher_known_nid(unsigned int lib_index, u32 nid)
{
    tGlobals * g = get_globals();
	int i;
	int higher_index = -1;

	for(i=0; i<NID_TABLE_SIZE; i++)
	{
		if (g->nid_table.table[i].lib_index == lib_index && g->nid_table.table[i].nid > nid)
		{
			// First higher found
			if (higher_index < 0)
				higher_index = i;

			// New higher, check if closer
			else if (g->nid_table.table[i].nid < g->nid_table.table[higher_index].nid)
				higher_index = i;
		}
	}

	return higher_index;
}

// Return index in nid_table for closest lower known NID
int get_lower_known_nid(unsigned int lib_index, u32 nid)
{
    tGlobals * g = get_globals();
	int i;
	int lower_index = -1;

	for(i=0; i<NID_TABLE_SIZE; i++)
	{
		if (g->nid_table.table[i].lib_index == lib_index && g->nid_table.table[i].nid < nid)
		{
			// First higher found
			if (lower_index < 0)
				lower_index = i;

			// New higher, check if closer
			else if (g->nid_table.table[i].nid > g->nid_table.table[lower_index].nid)
				lower_index = i;
		}
	}

	return lower_index;
}

// Fills remaining information on a library
tSceLibrary* complete_library(tSceLibrary* plibrary, int UNUSED(reference_library_index), int UNUSED(is_cfw))
{
	SceUID nid_file;
	u32 nid;
	unsigned int i;
	u32 lowest_syscall;
	int syscall_gap;
	int index;

	nid_file = open_nids_file(plibrary->name);

	// Opening the NID file
	if (nid_file > -1)
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

		NID_LOGSTR0("Getting lowest syscall\n");
		lowest_syscall = get_klowest_syscall(plibrary, reference_library_index, is_cfw);
		if ((lowest_syscall & SYSCALL_MASK_RESOLVE) == 0)
		{
			syscall_gap = plibrary->lowest_syscall - lowest_syscall;

			// We must consider the gap here too
			if ((int)plibrary->lowest_index < syscall_gap)
				syscall_gap -= plibrary->gap;

			if (syscall_gap < 0)
			{
				// This is actually a serious error indicating that the syscall offset or
				// memory address is wrong
				LOGSTR2("ERROR: lowest syscall in kernel (0x%08lX) is bigger than found lowest syscall in tables (0x%08lX)\n", lowest_syscall,
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

				// Add to nid table
				add_nid_to_table(nid, MAKE_SYSCALL(lowest_syscall), get_library_index(plibrary->name));
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
    tGlobals * g = get_globals();
	int i;

	for (i=0; i<NID_TABLE_SIZE; i++)
	{
		if (g->nid_table.table[i].nid == 0)
			break;
		else if (g->nid_table.table[i].nid == nid)
		    return i;
	}

	return -1;
}

// Checks if a library is in the global library description table
// Returns index if it's there
int get_library_index(const char* library_name)
{
    tGlobals * g = get_globals();
	LOGSTR1("Searching for library %s\n", (u32)library_name);
	if (library_name == NULL)
        return -1;

	int i;
    for (i=0; i<(int) g->library_table.num; i++)
    {
		//LOGSTR1("Current library: %s\n", g->library_table.table[i].library_name);
        if (g->library_table.table[i].name == NULL)
            break;
        else if (strcmp(library_name, g->library_table.table[i].name) == 0)
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
    tGlobals * g = get_globals();
    tSceLibrary my_lib;
    int i;

    my_lib = g->library_table.table[lib_index];
    *low = 0;
    *high = 0;

    for (i=0; i<(int)g->library_table.num; i++)
    {
        if (i == lib_index)
            continue;

        if (g->library_table.table[i].name == NULL)
            break;

        tSceLibrary lib = g->library_table.table[i];
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
    tGlobals * g = get_globals();
	int library_index = -1;
    u32 i = 0, j, k = 0;
	unsigned int nlib_stubs;
	u32 *cur_nid, *cur_call, syscall_num, good_call, nid;
	tStubEntry *pentry;

#ifdef AUTO_SEARCH_STUBS
#ifdef LOAD_MODULES_FOR_SYSCALLS
    load_modules_for_stubs();
#endif

    u32 stubs[MAX_RUNTIME_STUB_HEADERS];
    unsigned int current_stubid = 0;
    nlib_stubs = (unsigned int) (search_stubs(stubs));
    LOGSTR1("Found %d stubs\n", nlib_stubs);
	if (nlib_stubs == 0)
		exit_with_log(" ERROR: NO LIBSTUBS DEFINED IN CONFIG ", NULL, 0);
    CLEAR_CACHE;
    pentry = (tStubEntry*) stubs[current_stubid];
#else
    int aux1, aux2;
    u32 aux3;

	// Getting game's .lib.stub address
	if ((aux1 = config_initialize()) < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &aux1, sizeof(aux1));

	if ((aux1 = config_num_lib_stub(&nlib_stubs)) < 0)
	    exit_with_log(" ERROR READING NUMBER OF LIBSTUBS FROM CONFIG ", &aux1, sizeof(aux1));

	if (nlib_stubs == 0)
		exit_with_log(" ERROR: NO LIBSTUBS DEFINED IN CONFIG ", NULL, 0);

	if ((aux2 = config_first_lib_stub(&aux3)) < 0)
		exit_with_log(" ERROR GETTING FIRST LIBSTUB FROM CONFIG ", &aux2, sizeof(aux2));

	pentry = (tStubEntry*) aux3;

#endif


	//DEBUG_PRINT(" build_nid_table() ENTERING MAIN LOOP ", NULL, 0);

	do
	{
		NID_LOGSTR1("-->CURRENT MODULE LIBSTUB: 0x%08lX\n", (u32)pentry);

		// While it's a valid stub header
		while (elf_check_stub_entry(pentry))
		{
			if ((pentry->import_flags != 0x11) && (pentry->import_flags != 0))
			{
				// Variable import, skip it
				pentry = (tStubEntry*)((int)pentry + sizeof(u32));
				continue;
			}

#ifndef XMB_LAUNCHER
			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if ((i >= NID_TABLE_SIZE) || (k >= MAX_LIBRARIES))
			{
				config_close();
				LOGSTR1(" NID TABLE COUNTER: 0x%08lX\n", i);
				LOGSTR1(" LIBRARY TABLE COUNTER: 0x%08lX\n", k);
				LOGSTR0(" NID/LIBRARY TABLES TOO SMALL ");
				sceKernelExitGame();
			}
#endif

			NID_LOGSTR1("-->Processing library: %s ", (u32)(pentry->library_name));

			// Get actual call
			cur_call = pentry->jump_pointer;
			good_call = get_good_call(cur_call);

			// Only process if syscall and if the import is already resolved
			if (!(good_call & SYSCALL_MASK_RESOLVE) && (GET_SYSCALL_NUMBER(good_call) != SYSCALL_IMPORT_NOT_RESOLVED_YET))
			{
				// Get current NID
				cur_nid = pentry->nid_pointer;

				// Is this library on the table?
				library_index = get_library_index((char *)pentry->library_name);

				// New library
				if (library_index < 0)
				{
					NID_LOGSTR1(" --> New: %d\n", k);

					strcpy(g->library_table.table[k].name, (char *)pentry->library_name);
					g->library_table.table[k].calling_mode = SYSCALL_MODE;

					// Get number of syscalls imported
					g->library_table.table[k].num_known_exports = pentry->stub_size;

					NID_LOGSTR1("Total known exports: %d\n", g->library_table.table[k].num_known_exports);

					// Initialize lowest syscall on library table
					g->library_table.table[k].lowest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));
					g->library_table.table[k].lowest_nid = *cur_nid;
					NID_LOGSTR2("Initial lowest nid/syscall: 0x%08lX/0x%08lX \n", g->library_table.table[k].lowest_syscall, g->library_table.table[k].lowest_nid);

		            // Initialize highest syscall on library table
		            g->library_table.table[k].highest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));

					// Browse all stubs defined by this header
					for(j=0; j<pentry->stub_size; j++)
					{
						nid = *cur_nid;
						NID_LOGSTR1("--Current NID: 0x%08lX", nid);

						// If NID is already in, don't put it again
						if (get_nid_index(nid) < 0)
						{
							// Fill NID table

// There is probably a way to shorten this #ifdef block, but I'm too lazy to check if I broke backwards compatibility here,
// so I'm being concervative(wololo)
#ifdef XMB_LAUNCHER
							good_call = get_good_call(cur_call);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(good_call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);

							if (syscall_num < g->library_table.table[k].lowest_syscall)
							{
								g->library_table.table[k].lowest_syscall = syscall_num;
								g->library_table.table[k].lowest_nid = nid;//g->nid_table.table[i].nid;
								NID_LOGSTR2("\nNew lowest syscall/nid: 0x%08lX/0x%08lX", g->library_table.table[k].lowest_syscall, g->library_table.table[k].lowest_nid);
							}
#else
							add_nid_to_table(nid, get_good_call(cur_call), k);
							NID_LOGSTR1(" --> new inserted @ %d", i);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(g->nid_table.table[i].call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);

							if (syscall_num < g->library_table.table[k].lowest_syscall)
							{
								g->library_table.table[k].lowest_syscall = syscall_num;
								g->library_table.table[k].lowest_nid = g->nid_table.table[i].nid;
								NID_LOGSTR2("\nNew lowest syscall/nid: 0x%08lX/0x%08lX", g->library_table.table[k].lowest_syscall, g->library_table.table[k].lowest_nid);
							}
#endif

		                    if (syscall_num > g->library_table.table[k].highest_syscall)
							{
								g->library_table.table[k].highest_syscall = syscall_num;
								NID_LOGSTR2("\nNew highest syscall/nid: 0x%08lX/0x%08lX", g->library_table.table[k].highest_syscall, nid);
							}
							NID_LOGSTR0("\n");
							i++;
						}
						cur_nid++;
						cur_call += 2;
					}

					// New library
					g->library_table.num++;

					// Next library entry
					k++;
				}

				// Old library
				else
				{
					NID_LOGSTR1(" --> Old: %d\n", library_index);

					LOGLIB(g->library_table.table[library_index]);

					NID_LOGSTR1("Number of imports of this stub: %d\n", pentry->stub_size);

					NID_LOGSTR2("Current lowest nid/syscall: 0x%08lX/0x%08lX \n", g->library_table.table[library_index].lowest_syscall, g->library_table.table[library_index].lowest_nid);

					if (g->library_table.table[library_index].calling_mode != SYSCALL_MODE)
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

// There is probably a way to shorten this #ifdef block, but I'm too lazy to check if I broke backwards compatibility here,
// so I'm being concervative(wololo)
#ifdef XMB_LAUNCHER
							good_call = get_good_call(cur_call);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(good_call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);
							if (syscall_num < g->library_table.table[library_index].lowest_syscall)
							{
								g->library_table.table[library_index].lowest_syscall = syscall_num;
								g->library_table.table[library_index].lowest_nid = nid;
								NID_LOGSTR2("\nNew lowest nid/syscall: 0x%08lX/0x%08lX", g->library_table.table[library_index].lowest_syscall, g->library_table.table[library_index].lowest_nid);
							}
#else
							add_nid_to_table(nid, get_good_call(cur_call), library_index);
							NID_LOGSTR1(" --> new inserted @ %d", i);

							// Check lowest syscall
							syscall_num = GET_SYSCALL_NUMBER(g->nid_table.table[i].call);
							NID_LOGSTR1(" with syscall 0x%08lX", syscall_num);
							if (syscall_num < g->library_table.table[library_index].lowest_syscall)
							{
								g->library_table.table[library_index].lowest_syscall = syscall_num;
								g->library_table.table[library_index].lowest_nid = g->nid_table.table[i].nid;
								NID_LOGSTR2("\nNew lowest nid/syscall: 0x%08lX/0x%08lX", g->library_table.table[library_index].lowest_syscall, g->library_table.table[library_index].lowest_nid);
							}
#endif
		                    if (syscall_num > g->library_table.table[library_index].highest_syscall)
							{
								g->library_table.table[library_index].highest_syscall = syscall_num;
								NID_LOGSTR2("\nNew highest syscall/nid: 0x%08lX/0x%08lX", g->library_table.table[library_index].highest_syscall, nid);
							}
							//DEBUG_PRINT(" NID INSERTED ", &nid_table[i].nid, sizeof(u32));
							//DEBUG_PRINT(" CURRENT POSITION ", &i, sizeof(i));
							i++;
							g->library_table.table[library_index].num_known_exports++;
						}
						NID_LOGSTR0("\n");
						cur_nid++;
						cur_call += 2;
					}
				}
			}

			// Next entry
			pentry++;

			CLEAR_CACHE;
		}

#ifdef AUTO_SEARCH_STUBS
        current_stubid++;
 		// Next .lib.stub
		if (current_stubid < nlib_stubs)
		{
			pentry = (tStubEntry*) stubs[current_stubid];
		}
		else
			break;
#else
		nlib_stubs--;

		// Next .lib.stub
		if (nlib_stubs > 0)
		{
			aux2 = config_next_lib_stub(&aux3);
			if (aux2 < 0)
				exit_with_log(" ERROR GETTING NEXT LIBSTUB FROM CONFIG ", &aux2, sizeof(aux2));
			pentry = (tStubEntry*) aux3;
		}
		else
			break;
#endif
		sceKernelDelayThread(100000);

	} while(1);


	u32 m;
	int reference_library_index = get_library_index(SYSCALL_REFERENCE_LIBRARY);

#ifdef DEBUG
	if (g->syscalls_known && (getFirmwareVersion() <= 610))
	{
		// Write out a library table
		int is_aligned = 0;
		int base_syscall = g->library_table.table[reference_library_index].lowest_syscall;
		int firmware_version = getFirmwareVersion();

		for (m = 0; m < g->library_table.num; m++)
		{
			int nidsfile = open_nids_file(g->library_table.table[m].name);
			int num_library_exports = sceIoLseek(nidsfile, 0, PSP_SEEK_END) / sizeof(u32);
			sceIoClose(nidsfile);

			if (num_library_exports < 0)
			  num_library_exports = -1;

			is_aligned = ((g->library_table.table[m].highest_syscall - g->library_table.table[m].lowest_syscall) == (u32)num_library_exports - 1);
			LOGSTR4("%d %s %d %d ", firmware_version, (u32)g->library_table.table[m].name, is_aligned, (u32)num_library_exports);
			LOGSTR1("%d ", g->library_table.table[m].highest_syscall - g->library_table.table[m].lowest_syscall + 1);
			LOGSTR4("%d %d %d %d\n", g->library_table.table[m].lowest_syscall - base_syscall, g->library_table.table[m].highest_syscall - base_syscall, g->library_table.table[m].lowest_syscall, g->library_table.table[m].highest_syscall);
		}

		LOGSTR0("\n");
	}
#endif

	// On CFW there is a higher syscall difference between SysmemUserForUser
	// and lower libraries than without it.
	int sysmem_lowest_syscall = g->library_table.table[get_library_index("SysMemUserForUser")].lowest_syscall;
	int interrupt_lowest_syscall = g->library_table.table[get_library_index("InterruptManager")].lowest_syscall;

	int is_cfw = ((getFirmwareVersion() <= 550)
		&& (sysmem_lowest_syscall - interrupt_lowest_syscall > 244));

	if (is_cfw)
		print_to_screen("Using offsets for Custom Firmware");


	// Fill remaining data after the reference library is known
	for (m = 0; m < g->library_table.num; m++)
	{
		LOGSTR1("Completing library...%d\n", m);
		complete_library(&(g->library_table.table[m]), reference_library_index, is_cfw);
	}
	CLEAR_CACHE;


#ifdef DEBUG
	u32 c1, c2;
	u32 syscall;
	int estimated_correctly = 0;

	LOGSTR0("\n==LIBRARY TABLE DUMP==\n");
	c1 = 0;
	while (c1 < g->library_table.num)
	{
		LOGSTR2("->Index: %d, name %s\n", c1, (u32)g->library_table.table[c1].name);
		LOGSTR2("predicted syscall range from 0x%08lX to 0x%08lX\n", g->library_table.table[c1].lowest_syscall, g->library_table.table[c1].lowest_syscall + g->library_table.table[c1].num_library_exports + g->library_table.table[c1].gap - 1);

		int nid_file = open_nids_file(g->library_table.table[c1].name);

		c2 = 0;
		while (c2 < g->nid_table.num)
		{
			int position = 0;
			int foundNid = 0;

			if (g->nid_table.table[c2].lib_index == c1)
			{
				if (g->nid_table.table[c2].call & SYSCALL_MASK_RESOLVE)
					syscall = g->nid_table.table[c2].call;
				else
					syscall = GET_SYSCALL_NUMBER(g->nid_table.table[c2].call);

				LOGSTR3("[%d] 0x%08lX 0x%08lX ", c2, g->nid_table.table[c2].nid, syscall);

				// Check nid in table against the estimated nids
				if (g->syscalls_known && (nid_file > -1))
				{
					sceIoLseek(nid_file, 0, PSP_SEEK_SET);
					u32 current_nid = 0;
					i = 0;
					position = 0;
					foundNid = 0;

					for (i = g->library_table.table[c1].lowest_index; i < g->library_table.table[c1].num_library_exports; i++)
					{
						sceIoLseek(nid_file, 4 * i, PSP_SEEK_SET);
						sceIoRead(nid_file, &current_nid, sizeof(u32));
						if (current_nid == g->nid_table.table[c2].nid)
						{
							foundNid = 1;
							break;
						}
						else
							position++;
					}


					if (!foundNid)
					{
						// gap
						position += g->library_table.table[c1].gap;

						sceIoLseek(nid_file, 0, PSP_SEEK_SET);
						for (i = 0; i < g->library_table.table[c1].lowest_index; i++)
						{
							sceIoLseek(nid_file, 4 * i, PSP_SEEK_SET);
							sceIoRead(nid_file, &current_nid, sizeof(u32));

							if (current_nid == g->nid_table.table[c2].nid)
							{
								foundNid = 1;
								break;
							}
							else
								position++;
						}
					}

					estimated_correctly = ((g->library_table.table[c1].lowest_syscall + position) == syscall);

					// format: estimated call, index in nid-file, correctly estimated?
					LOGSTR3("0x%08lX %d %s\n", (g->library_table.table[c1].lowest_syscall + position), i, estimated_correctly ? (u32)"YES" : (u32)"NO");
				}
				else
					LOGSTR0("\n");

			}

			c2++;
		}

		sceIoClose(nid_file);

		c1++;
		LOGSTR0("\n\n");
	}
#endif

#ifdef AUTO_SEARCH_STUBS
#ifdef LOAD_MODULES_FOR_SYSCALLS
    unload_modules_for_stubs();
#endif
#else
	config_close();
#endif

#ifndef XMB_LAUNCHER
	g->nid_table.num = i;
#endif

	return i;
}
#include "tables.h"
#include "config.h"
#include "elf.h"
#include "debug.h"
#include "malloc.h"
#include "lib.h"
#include "utils.h"
#include "globals.h"
#include "syscall.h"

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
	int ret;
	unsigned int num_libs, i = 0, count = 0;
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


int get_library_offset(u32 library, int psplink_running)
{
	// This could be in external files later on

	short offset_570[] =    { -522, -513, -361, -350, -313, -291, -268, -262, -214, -205, -187, -140, -113, -90, -70, -58, 0, 15, 68, 82, 247, 228, 41 };
	short offset_570_go[] = { -522, -513, -361, -350, -313, -291, -268, -262, -214, -205, -187, -140, -113, -90, -70, -58, 0, 15, 68, 82, 363, 344, 41 };

	short offset_500[] =    { -503, -494, -351, -340, -303, -282, -260, -254, -210, -201, -183, -136, -109, -86, -70, -58, 0, 15, 69, 83, 245, 226, 42 };
	short offset_550[] =    { -504, -495, -352, -341, -304, -282, -260, -254, -210, -201, -183, -136, -109, -86, -70, -58, 0, 15, 69, 83, 246, 227, 42 };
	short offset_600[] =    { -523, -514, -363, -352, -315, -293, -268, -262, -214, -205, -187, -39, -140, -117, -12, -97, 0, 15, 69, 83, 271, 249, 41 };
	short offset_600_go[] = { -523, -514, -363, -352, -315, -293, -268, -262, -214, -205, -187, -39, -140, -117, -12, -97, 0, 15, 69, 83, 383, 361, 41 };

	short offset_500_psplink[] = { -542, -533, -390, -379, -342, -282, -260, -254, -210, -201, -183, -136, -109, -86, -70, -48, 0, 15, 69, 83, 265, 246, 42 };
	short offset_550_psplink[] = { -543, -534, -391, -380, -343, -282, -260, -254, -210, -201, -183, -136, -109, -86, -70, -58, 0, 15, 69, 83, 266, 247, 42 };

	switch (getFirmwareVersion())
	{
		case 500:
		case 503:
			if (psplink_running)
				return offset_500_psplink[library];
			else 
				return offset_500[library];

		case 550:
		case 551:
		case 555:
			if (psplink_running)
				return offset_550_psplink[library];
			else
				return offset_550[library];

		case 570:
			if (getPSPModel() == PSP_GO)
				return offset_570_go[library];
			else
				return offset_570[library];

		case 600:
		case 610:
			if (getPSPModel() == PSP_GO)
				return offset_600_go[library];
			else
				return offset_600[library];
	}

	return 0;
}


// Gets lowest syscall from kernel dump
u32 get_klowest_syscall(tSceLibrary* library)
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
			tSceLibrary* lib_base = &(g->library_table.table[20]);
			LOGSTR1("Base syscall is %d\n", lib_base->lowest_syscall);

			// Detect if PSPLink is running by analyzing the position of sceImpose, it is > 245 on CFW
			int psplink_running = ((getFirmwareVersion() <= 550) 
				&& ((g->library_table.table[17].lowest_syscall - lib_base->lowest_syscall) > 245));

			if (psplink_running)
				LOGSTR0("Using offsets for CFW\n");

			if (strcmp(library->name, "InterruptManager") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_INTERRUPTMANAGER, psplink_running);

			else if (strcmp(library->name, "ThreadManForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_THREADMANFORUSER, psplink_running);

			else if (strcmp(library->name, "StdioForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_STDIOFORUSER, psplink_running);

			else if (strcmp(library->name, "IoFileMgrForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_IOFILEMGRFORUSER, psplink_running);

			else if (strcmp(library->name, "ModuleMgrForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_MODULEMGRFORUSER, psplink_running);

			else if (strcmp(library->name, "SysMemUserForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SYSMEMUSERFORUSER, psplink_running);

			else if (strcmp(library->name, "sceSuspendForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCESUSPENDFORUSER, psplink_running);
			
			else if (strcmp(library->name, "UtilsForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_UTILSFORUSER, psplink_running);

			else if (strcmp(library->name, "LoadExecForUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_LOADEXECFORUSER, psplink_running);

			else if (strcmp(library->name, "sceGe_user") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEGE_USER, psplink_running);

			else if (strcmp(library->name, "sceRtc") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCERTC, psplink_running);

			else if (strcmp(library->name, "sceAudio") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEAUDIO, psplink_running);

			else if (strcmp(library->name, "sceDisplay") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEDISPLAY, psplink_running);

			else if (strcmp(library->name, "sceCtrl") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCECTRL, psplink_running);

			else if (strcmp(library->name, "sceHprm") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEHPRM, psplink_running);

			else if (strcmp(library->name, "scePower") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEPOWER, psplink_running);

			else if (strcmp(library->name, "sceOpenPSID") == 0)
				lowest_syscall = lib_base->lowest_syscall;

			else if (strcmp(library->name, "sceWlanDrv") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEWLANDRV, psplink_running);

			else if (strcmp(library->name, "sceUmdUser") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEUMDUSER, psplink_running);

			else if (strcmp(library->name, "sceUtility") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEUTILITY, psplink_running);

			else if (strcmp(library->name, "sceSasCore") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCESASCORE, psplink_running);

			else if (strcmp(library->name, "sceImpose") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEIMPOSE, psplink_running);

			else if (strcmp(library->name, "sceReg") == 0)
				lowest_syscall = lib_base->lowest_syscall + get_library_offset(LIBRARY_SCEREG, psplink_running);

		}
		else
		{
			// Read the lowest syscalls from the GO kernel memory dump, only for 6.20
			SceUID kdump_fd;
			SceSize kdump_size;
			SceOff lowest_offset = 0;
			SceOff next_library_offset = 0;
			u32 next_library_base_syscall = 0;

			kdump_fd = sceIoOpen(HBL_ROOT"kmem.dump", PSP_O_CREAT | PSP_O_RDONLY, 0777);
			if (kdump_fd >= 0)
			{
				kdump_size = sceIoLseek(kdump_fd, 0, PSP_SEEK_END);

				if (kdump_size > 0)
				{
					// Libraries are listed in the same order as in the 6.20 kernel

					if (strcmp(library->name, "InterruptManager") == 0)
					{
						lowest_offset = (SceOff)0x0002094C;
						next_library_offset = (SceOff)0x000208FC;
					}
					else if (strcmp(library->name, "ThreadManForUser") == 0)
					{
						lowest_offset = (SceOff)0x000208FC;
						next_library_offset = (SceOff)0x000207BC;
					}
					else if (strcmp(library->name, "StdioForUser") == 0)
					{
						lowest_offset = (SceOff)0x000207BC;
						next_library_offset = (SceOff)0x0002071C;
					}
					else if (strcmp(library->name, "IoFileMgrForUser") == 0)
					{
						lowest_offset = (SceOff)0x0002071C;
						next_library_offset = (SceOff)0x000205DC;
					}
					else if (strcmp(library->name, "ModuleMgrForUser") == 0)
					{
						lowest_offset = (SceOff)0x000205DC;
						next_library_offset = (SceOff)0x000204EC;
					}
					else if (strcmp(library->name, "SysMemUserForUser") == 0)
					{
						lowest_offset = (SceOff)0x000204EC;
						next_library_offset = (SceOff)0x0002049C;
					}
					else if (strcmp(library->name, "sceSuspendForUser") == 0)
					{
						lowest_offset = (SceOff)0x0002049C;
						next_library_offset = (SceOff)0x0002044C;
					}
					else if (strcmp(library->name, "UtilsForUser") == 0)
					{
						lowest_offset = (SceOff)0x0002044C;
						next_library_offset = (SceOff)0x000203AC;
					}
					else if (strcmp(library->name, "LoadExecForUser") == 0)
					{
						lowest_offset = (SceOff)0x000203AC;
						// Unknown library @ 0x0002B630
						next_library_offset = (SceOff)0x0002B630;
					}
					else if (strcmp(library->name, "sceGe_user") == 0)
					{
						lowest_offset = (SceOff)0x0002B420;
						next_library_offset = (SceOff)0x0002B0D8;
					}
					else if (strcmp(library->name, "sceRtc") == 0)
					{
						lowest_offset = (SceOff)0x0002B0D8;
						next_library_offset = (SceOff)0x0002A8F8;
					}
					else if (strcmp(library->name, "sceDisplay") == 0)
					{
						lowest_offset = (SceOff)0x0002A8F8;
						next_library_offset = (SceOff)0x000BEDA0;
					}
					else if (strcmp(library->name, "sceCtrl") == 0)
					{
						lowest_offset = (SceOff)0x000BEDA0;
						next_library_offset = (SceOff)0x000BE858;
					}
					else if (strcmp(library->name, "scePower") == 0)
					{
						lowest_offset = (SceOff)0x000BE858;
						next_library_offset = (SceOff)0x000F8010;
					}
					else if (strcmp(library->name, "sceAudio") == 0)
					{
						lowest_offset = (SceOff)0x000F8010;
						next_library_offset = (SceOff)0x0015A9A0;
					}
					else if (strcmp(library->name, "sceHprm") == 0)
					{
						lowest_offset = (SceOff)0x0015A9A0;	
						next_library_offset = (SceOff)0x0015A670;
					}
					else if (strcmp(library->name, "sceOpenPSID") == 0)
					{
						lowest_offset = (SceOff)0x0015A670;
						// Unknown library @ 0x0015A2E0 with 15 exports
						next_library_offset = (SceOff)0x0015A2E0;
					}
					else if (strcmp(library->name, "sceWlanDrv") == 0)
					{
						lowest_offset = (SceOff)0x00159E00;
						next_library_offset = (SceOff)0x00159DA8;
					}
					else if (strcmp(library->name, "sceWlanDrv_lib") == 0)
					{
						lowest_offset = (SceOff)0x00159DA8;
						next_library_offset = (SceOff)0x001889A0;
					}
					else if (strcmp(library->name, "sceReg") == 0)
					{
						lowest_offset = (SceOff)0x001889A0;
						// Unknown library @ 0x001882C8 with 8 exports
						next_library_offset = (SceOff)0x001882C8;
					}
					else if (strcmp(library->name, "sceUmdUser") == 0)
					{
						lowest_offset = (SceOff)0x001E6408;
						next_library_offset = (SceOff)0x001E5CD0;
					}
					else if (strcmp(library->name, "sceUtility") == 0)
					{
						lowest_offset = (SceOff)0x001E5CD0;
						// Unknown library @ 0x001E5C78
						next_library_offset = (SceOff)0x001E5C78;
					}
					else if (strcmp(library->name, "sceSasCore") == 0)
					{
						lowest_offset = (SceOff)0x0026A4D0;
						// Unknown library @ 0x0026A420
						next_library_offset = (SceOff)0x0026A420;
					}
					else if (strcmp(library->name, "sceImpose") == 0)
					{
						lowest_offset = (SceOff)0x0025B3A0;
						// Unknown library @ 0x0025B2F0
						next_library_offset = (SceOff)0x0025B2F0;
					}
					else
					{
						LOGSTR0("Library not hacked\n");			
					}

					if (lowest_offset > 0)
					{
						LOGSTR1("Lowest offset: 0x%08lX\n", lowest_offset);
						sceIoLseek(kdump_fd, lowest_offset, PSP_SEEK_SET);
						sceIoRead(kdump_fd, &lowest_syscall, sizeof(lowest_syscall));
						LOGSTR1("Kernel dump says lowest syscall is: 0x%08lX\n", lowest_syscall);

						// Read next library base
						NID_LOGSTR1("Next library offset: 0x%08lX\n", next_library_offset);
						sceIoLseek(kdump_fd, next_library_offset, PSP_SEEK_SET);
						sceIoRead(kdump_fd, &next_library_base_syscall, sizeof(next_library_base_syscall));
						LOGSTR1("Kernel dump says next libraries syscall is: 0x%08lX\n", next_library_base_syscall);

						// Does the library fit without need to wrap around?
						int position_of_first_nid = library->lowest_syscall - library->lowest_index;
						int position_of_last_nid = position_of_first_nid + library->num_library_exports - 1;
						if ((position_of_first_nid >= (int)lowest_syscall) && (position_of_last_nid < (int)next_library_base_syscall))
						{
							// The library fits without wrap around, so the gap can be
							// ignored and the lowest syscall must be adjusted.
							library->gap = 0;
							lowest_syscall = position_of_first_nid;
						}
						else
						{
							// Library wraps around, calculate gap
							library->gap = next_library_base_syscall - lowest_syscall - library->num_library_exports;
						}

						LOGSTR1("The gap is: %d\n", library->gap);
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
tSceLibrary* complete_library(tSceLibrary* plibrary)
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

		NID_LOGSTR0("Getting lowest syscall from kernel dump\n");
		lowest_syscall = get_klowest_syscall(plibrary);
		if ((lowest_syscall & SYSCALL_MASK_RESOLVE) == 0)
		{
			syscall_gap = plibrary->lowest_syscall - lowest_syscall;

			// We must consider the gap here too
			if ((int)plibrary->lowest_index < syscall_gap)
				syscall_gap -= plibrary->gap;

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
    int aux1, aux2;
    u32 aux3;
	tSceLibrary *ret;
	tStubEntry *pentry;	

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

	//DEBUG_PRINT(" build_nid_table() ENTERING MAIN LOOP ", NULL, 0);

	do
	{
		NID_LOGSTR1("-->CURRENT MODULE LIBSTUB: 0x%08lX\n", (u32)pentry);
		
		// While it's a valid stub header
		while (elf_check_stub_entry(pentry))
		{
			if (pentry->import_flags != 0x11)
			{
				// Variable import, skip it
				pentry = (tStubEntry*)((int)pentry + sizeof(u32));
				continue;
			}

			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if ((i >= NID_TABLE_SIZE) || (k >= MAX_LIBRARIES))
			{
				config_close();
				LOGSTR1(" NID TABLE COUNTER: 0x%08lX\n", i);
				LOGSTR1(" LIBRARY TABLE COUNTER: 0x%08lX\n", k);
				LOGSTR0(" NID/LIBRARY TABLES TOO SMALL ");
				sceKernelExitGame();
			}			

			NID_LOGSTR1("-->Processing library: %s ", (u32)(pentry->library_name));

			// Get actual call
			cur_call = pentry->jump_pointer;
			good_call = get_good_call(cur_call);

			// Only process if syscall
			if (!(good_call & SYSCALL_MASK_RESOLVE))
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

			sceKernelDcacheWritebackInvalidateAll();
		}
		
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

		sceKernelDelayThread(100000);
		
	} while(1);

	// Fill remaining data after the syscall for sceOpenPSID is known
	u32 m;
	for (m = 0; m < g->library_table.num; m++)
	{
	  LOGSTR1("Completing library...%d\n", m);
	  ret = complete_library(&(g->library_table.table[m]));
	}
	sceKernelDcacheWritebackInvalidateAll();


#ifdef NID_DEBUG
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

	config_close();

	g->nid_table.num = i;
	
	return i;
}

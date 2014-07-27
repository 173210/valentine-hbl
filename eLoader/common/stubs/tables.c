#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <hbl/mod/elf.h>
#include <common/debug.h>
#include <common/malloc.h>
#include <common/utils.h>
#include <common/globals.h>
#include <exploit_config.h>

// Returns the index on nid_table for the given call
// NOTE: the whole calling instruction is to be passed
int get_call_index(u32 call)
{
    	int i = NID_TABLE_SIZE - 1;
	while(i >= 0 && globals->nid_table.table[i].call != call)
	{
		i--;
	}

	return i;
}

// Return index in NID table for the call that corresponds to the NID pointed by "nid"
// Puts call in call_buffer
u32 get_call_nidtable(u32 nid, u32* call_buffer)
{
    	int i;

	*call_buffer = 0;
	for(i=0; i<NID_TABLE_SIZE; i++)
	{
        if(nid == globals->nid_table.table[i].nid)
        {
            if(call_buffer != NULL)
                *call_buffer = globals->nid_table.table[i].call;
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


int get_lib_index_from_name(char* name)
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


#ifndef DEACTIVATE_SYSCALL_ESTIMATION
int get_library_kernel_memory_offset(char* lib_name)
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

	int lib_index = get_lib_index_from_name(lib_name);

	if (lib_index > -1)
	{
		switch (get_fw_ver())
		{
#ifdef SYSCALL_KERNEL_OFFSETS_620
			case 620:
				return offset_620[lib_index];
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_630

			case 630:
			case 631:
				return offset_630[lib_index];
#endif
#ifdef SYSCALL_KERNEL_OFFSETS_635
			case 635:
				return offset_635[lib_index];
#endif
		}
	}

	return -1;
}


int get_library_offset(char* lib_name, int is_cfw)
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

	int lib_index = get_lib_index_from_name(lib_name);

	if (lib_index > -1)
	{
		switch (get_fw_ver())
		{
			case 500:
			case 503:
				if (is_cfw)
#ifdef SYSCALL_OFFSETS_500_CFW
					return offset_500_cfw[lib_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_500
					return offset_500[lib_index];
#else
					break;
#endif

			case 550:
			case 551:
			case 555:
				if (is_cfw)
#ifdef SYSCALL_OFFSETS_550_CFW
					return offset_550_cfw[lib_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_550
					return offset_550[lib_index];
#else
					break;
#endif

			case 570:
				if (getPSPModel() == PSP_GO)
#ifdef SYSCALL_OFFSETS_570_GO
					return offset_570_go[lib_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_570
					return offset_570[lib_index];
#else
					break;
#endif

			case 600:
			case 610:
				if (getPSPModel() == PSP_GO)
#ifdef SYSCALL_OFFSETS_600_GO
					return offset_600_go[lib_index];
#else
					break;
#endif
				else
#ifdef SYSCALL_OFFSETS_600
					return offset_600[lib_index];
#else
					break;
#endif
		}
	}

	return -1;
}


// Gets lowest syscall from kernel dump
u32 get_klowest_syscall(tSceLibrary* library, int ref_lib_index, int is_cfw)
{
	library->gap = 0; // For < 6.20 and in case of error

	u32 lowest_syscall = 0xFFFFFFFF;
	
	LOG_PRINTF("Get lowest syscall for %s\n", (u32)library->name);

	if (globals->syscalls_known)
	{
		if (get_fw_ver() <= 610)
		{
			// Syscalls can be calculated from the library offsets on FW <= 6.10
			tSceLibrary* lib_base = &(globals->lib_table.table[ref_lib_index]);

			int base_syscall = lib_base->lowest_syscall - get_library_offset(lib_base->name, is_cfw);

			lowest_syscall = base_syscall + get_library_offset(library->name, is_cfw);

			LOG_PRINTF("Lowest syscall is %d\n", lowest_syscall);
		}
#ifdef XMB_LAUNCHER
		else if (get_fw_ver() >= 660)
		{
			// Lowest syscall and gap is extracted from the launcher imports
			if (library->lowest_index == 0)
			{
				// Library does not wrap around
				library->gap = 0;
			}
			else
			{
				library->gap = (library->highest_syscall - library->lowest_syscall) - (library->num_lib_exports - 1);
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

						LOG_PRINTF("Lowest offset: 0x%08X\n", lowest_offset);
						sceIoLseek(kdump_fd, lowest_offset, PSP_SEEK_SET);
						sceIoRead(kdump_fd, &library_info, sizeof(HBLKernelLibraryStruct));
						LOG_PRINTF("Kernel dump says: lowest syscall = 0x%08X, gap = %d, offset to index 0 = %d\n", library_info.lowest_syscall, library_info.gap, library_info.offset_to_zero_index);

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
							LOG_PRINTF("->library doesn't wrap, gap set to 0\n");
						}
					}
				}
				else
				{
					LOG_PRINTF("Kmemdump empty\n");
				}

				sceIoClose(kdump_fd);
			}
			else
			{
				LOG_PRINTF("Could not open kernel dump\n");
			}
		}
	}

	return lowest_syscall;
}
#endif

// Return index in nid_table for closest higher known NID
int get_higher_known_nid(unsigned int lib_index, u32 nid)
{
    	int i;
	int higher_index = -1;

	for(i=0; i<NID_TABLE_SIZE; i++)
	{
		if (globals->nid_table.table[i].lib_index == lib_index && globals->nid_table.table[i].nid > nid)
		{
			// First higher found
			if (higher_index < 0)
				higher_index = i;

			// New higher, check if closer
			else if (globals->nid_table.table[i].nid < globals->nid_table.table[higher_index].nid)
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
		if (globals->nid_table.table[i].lib_index == lib_index && globals->nid_table.table[i].nid < nid)
		{
			// First higher found
			if (lower_index < 0)
				lower_index = i;

			// New higher, check if closer
			else if (globals->nid_table.table[i].nid > globals->nid_table.table[lower_index].nid)
				lower_index = i;
		}
	}

	return lower_index;
}
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
// Fills remaining information on a library
tSceLibrary* complete_library(tSceLibrary* plibrary, int UNUSED(ref_lib_index), int UNUSED(is_cfw))
{
	SceUID nid_file;
	u32 nid;
	unsigned int i;

	nid_file = open_nids_file(plibrary->name);

	// Opening the NID file
	if (nid_file > -1)
	{
		// Calculate number of NIDs (size of file/4)
		plibrary->num_lib_exports = sceIoLseek(nid_file, 0, PSP_SEEK_END) / sizeof(u32);
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
		u32 lowest_syscall;
		int syscall_gap;
		int index;

		NID_LOG_PRINTF("Getting lowest syscall\n");
		lowest_syscall = get_klowest_syscall(plibrary, ref_lib_index, is_cfw);
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
				LOG_PRINTF("ERROR: lowest syscall in kernel (0x%08X) is bigger than found lowest syscall in tables (0x%08X)\n", lowest_syscall,
				        plibrary->lowest_syscall);
			}

			else if (syscall_gap == 0)
			{
				NID_LOG_PRINTF("Nothing to do, lowest syscall from kernel is the same from tables\n");
			}

			// Find NID for lowest syscall
			else
			{
				index = plibrary->lowest_index - (unsigned int)syscall_gap;

				// Rotation ;)
				if (index < 0)
					index = plibrary->num_lib_exports + index;

				sceIoLseek(nid_file, index * 4, PSP_SEEK_SET);
				sceIoRead(nid_file, &nid, sizeof(u32));

				plibrary->lowest_index = index;
				plibrary->lowest_nid = nid;
				plibrary->lowest_syscall = lowest_syscall;

				// Add to nid table
				add_nid_to_table(nid, MAKE_SYSCALL(lowest_syscall), get_lib_index(plibrary->name));
			}
		}
		sceIoClose(nid_file);

		return plibrary;
	}
	else
		return NULL;
}
#endif

// Returns index of NID in table
int get_nid_index(u32 nid)
{
    	int i;

	for (i=0; i<NID_TABLE_SIZE; i++)
	{
		if (globals->nid_table.table[i].nid == 0)
			break;
		else if (globals->nid_table.table[i].nid == nid)
		    return i;
	}

	return -1;
}

// Checks if a library is in the global library description table
// Returns index if it's there
int get_lib_index(const char* lib_name)
{
    	LOG_PRINTF("Searching for library %s\n", lib_name);
	if (lib_name == NULL)
        return -1;

	int i;
    for (i=0; i<(int) globals->lib_table.num; i++)
    {
		//LOG_PRINTF("Current library: %s\n", globals->lib_table.table[i].lib_name);
        if (globals->lib_table.table[i].name == NULL)
            break;
        else if (strcmp(lib_name, globals->lib_table.table[i].name) == 0)
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
int get_syscall_boundaries(int lib_index, int *low, int *high)
{
	tSceLibrary lib, my_lib;
	int i, l, h;

	my_lib = globals->lib_table.table[lib_index];
	*low = 0;
	*high = 0;

	for (i = 0; i < (int)globals->lib_table.num; i++) {
		if (i == lib_index)
			continue;

		if (globals->lib_table.table[i].name == NULL)
			break;

		lib = globals->lib_table.table[i];
		l = lib.lowest_syscall;
		h = lib.highest_syscall;

		if (l > my_lib.highest_syscall && (l < *high || *high == 0))
			*high = l;

		if (h < my_lib.lowest_syscall && h > *low)
			*low = h;
	}

	return *low && *high;
}

// Adds NID entry to nid_table
int add_nid_to_table(u32 nid, u32 call, unsigned int lib_index)
{
    	NID_LOG_PRINTF("Adding NID 0x%08X to table... ", (int)nid);

	// Check if NID already exists in table (by another estimation for example)
	int index = get_nid_index(nid);

	// Doesn't exist, insert new
	if (index < 0)
	{
		if (globals->nid_table.num >= NID_TABLE_SIZE) {
			LOG_PRINTF("FATAL: NID TABLE IS FULL\n");
			return -1;
		}

		globals->nid_table.table[globals->nid_table.num].nid = nid;
		globals->nid_table.table[globals->nid_table.num].call = call;
		globals->nid_table.table[globals->nid_table.num].lib_index = lib_index;
		NID_LOG_PRINTF("Newly added @ %d\n", (int)globals->nid_table.num);
		globals->nid_table.num++;
	}

	// If it exists, just change the old call with the new one
	else
	{
		globals->nid_table.table[index].call = call;
		NID_LOG_PRINTF("Modified @ %d\n", (int)globals->nid_table.num);
	}

	return index;
}

// Adds a new library
int add_library_to_table(const tSceLibrary lib)
{
	// Check if library exists
	int index = get_lib_index(lib.name);

	// Doesn't exist, insert new
	if (index < 0)
	{
		// Check if there's space on the table
		if (globals->lib_table.num >= MAX_LIBRARIES)
		{
			LOG_PRINTF("->WARNING: Library table full, skipping new library %s\n", lib.name);
			return -1;
		}

		index = globals->lib_table.num;
        globals->lib_table.num++;
	}

	// Copying new library
	memcpy(&(globals->lib_table.table[index]), &lib, sizeof(lib));


	LOG_PRINTF("Added library %s @ %d\n", (u32)globals->lib_table.table[index].name, index);
	LOGLIB(globals->lib_table.table[index]);

	return index;
}

int add_stub_to_table(tStubEntry *pentry)
{
	Elf32_Addr cur_nid, cur_call;
	int nid_index, lib_index, syscall_num, good_call, nid;
	int num = 0;

	NID_LOG_PRINTF("-->Processing library: %s ", (u32)(pentry->lib_name));

	// Get actual call
	cur_call = pentry->jump_p;
	good_call = get_good_call(cur_call);

	// Only process if syscall and if the import is already resolved
	if (!(good_call & SYSCALL_MASK_RESOLVE) &&
		(GET_SYSCALL_NUMBER(good_call) != SYSCALL_IMPORT_NOT_RESOLVED_YET)) {

		// Get current NID
		cur_nid = pentry->nid_p;

		// Is this library on the table?
		lib_index = get_lib_index((char *)pentry->lib_name);

		if (lib_index < 0) {
			// New library
#ifndef XMB_LAUNCHER
			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if (globals->lib_table.num >= MAX_LIBRARIES) {
				LOG_PRINTF(" LIBRARY TABLE COUNTER: 0x%08X\n",
					globals->lib_table.num);
				LOG_PRINTF(" LIBRARY TABLES TOO SMALL ");
				sceKernelExitGame();
			}
#endif
			lib_index = globals->lib_table.num;
			NID_LOG_PRINTF(" --> New: %d\n", lib_index);

			strcpy(globals->lib_table.table[lib_index].name, (char *)pentry->lib_name);
			globals->lib_table.table[lib_index].calling_mode = SYSCALL_MODE;

			// Get number of syscalls imported
			globals->lib_table.table[lib_index].num_known_exports = pentry->stub_size;

			NID_LOG_PRINTF("Total known exports: %d\n", globals->lib_table.table[lib_index].num_known_exports);

			// Initialize lowest syscall on library table
			globals->lib_table.table[lib_index].lowest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));
			globals->lib_table.table[lib_index].lowest_nid = *cur_nid;

			// Initialize highest syscall on library table
			globals->lib_table.table[lib_index].highest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));

			// New library
			globals->lib_table.num++;
		} else {
				// Old library
			NID_LOG_PRINTF(" --> Old: %d\n", lib_index);

			NID_LOG_PRINTF("Number of imports of this stub: %d\n", pentry->stub_size);

			if (globals->lib_table.table[lib_index].calling_mode != SYSCALL_MODE) {
				LOG_PRINTF(" ERROR OLD CALL MODE IS SYSCALL, NEW IS JUMP 0x%08X\n", &lib_index);
				return 0;
			}

			globals->lib_table.table[lib_index].num_known_exports++;
		}

		LOGLIB(globals->lib_table.table[lib_index]);

		NID_LOG_PRINTF("Current lowest nid/syscall: 0x%08X/0x%08X \n",
			globals->lib_table.table[lib_index].lowest_syscall, globals->lib_table.table[lib_index].lowest_nid);

		// Browse all stubs defined by this header
		for(nid_index = 0; nid_index < pentry->stub_size; nid_index++) {
			nid = *cur_nid;
			NID_LOG_PRINTF("--Current NID: 0x%08X", nid);

			// If NID is already in, don't put it again
			if (get_nid_index(nid) < 0) {
					// Fill NID table
#ifdef XMB_LAUNCHER
				good_call = get_good_call(cur_call);
				// Check lowest syscall
				syscall_num = GET_SYSCALL_NUMBER(good_call);
#else
				add_nid_to_table(nid, get_good_call(cur_call), lib_index);
				NID_LOG_PRINTF(" --> new inserted @ %d", nid_index);
				// Check lowest syscall
				syscall_num = GET_SYSCALL_NUMBER(globals->nid_table.table[nid_index].call);
				nid = globals->nid_table.table[nid_index].nid;
#endif
				NID_LOG_PRINTF(" with syscall 0x%08X", syscall_num);

				if (syscall_num < globals->lib_table.table[lib_index].lowest_syscall) {
					globals->lib_table.table[lib_index].lowest_syscall = syscall_num;
					globals->lib_table.table[lib_index].lowest_nid = nid; // globals->nid_table.table[i].nid;
					NID_LOG_PRINTF("\nNew lowest syscall/nid: 0x%08X/0x%08X",
						globals->lib_table.table[lib_index].lowest_syscall, globals->lib_table.table[lib_index].lowest_nid);
				}

				if (syscall_num > globals->lib_table.table[lib_index].highest_syscall) {
					globals->lib_table.table[lib_index].highest_syscall = syscall_num;
					NID_LOG_PRINTF("\nNew highest syscall/nid: 0x%08X/0x%08X", globals->lib_table.table[lib_index].highest_syscall, nid);
				}
				NID_LOG_PRINTF("\n");
				num++;
			}
			cur_nid++;
			cur_call += 2;
		}
	}

	return num;
}

#if defined(DEBUG) && !defined(DEACTIVATE_SYSCALL_ESTIMATION)
void dump_lib_table()
{
	SceUID fd;
	int fw_ver = get_fw_ver();
	int is_cfw = fw_ver <= 550 &&
		globals->lib_table.table[get_lib_index("SysMemUserForUser")].lowest_syscall
			- globals->lib_table.table[get_lib_index("InterruptManager")].lowest_syscall > 244;
	int estimated_correctly = 0;
	int i, lib_index, nid_index, pos, syscall;

	if (globals->syscalls_known && (get_fw_ver() <= 610))
	{
		// Write out a library table
		int num_lib_exports;
		int ref_lib_index = get_lib_index(SYSCALL_REF_LIB);
		int base_syscall = globals->lib_table.table[ref_lib_index].lowest_syscall;

		if (num_lib_exports < 0)
			num_lib_exports = -1;

		for (lib_index = 0; lib_index < globals->lib_table.num; lib_index++) {
			fd = open_nids_file(globals->lib_table.table[lib_index].name);
			num_lib_exports = sceIoLseek(fd, 0, PSP_SEEK_END) / sizeof(int);
			sceIoClose(fd);

			LOG_PRINTF("%d %s %s %d ", fw_ver,
				(int)globals->lib_table.table[lib_index].name,
				globals->lib_table.table[lib_index].highest_syscall - globals->lib_table.table[lib_index].lowest_syscall
					== (int)num_lib_exports - 1 ?
					(int)"aligned" : (int)"not aligned",
				(int)num_lib_exports);
			LOG_PRINTF("%d ", globals->lib_table.table[lib_index].highest_syscall - globals->lib_table.table[lib_index].lowest_syscall + 1);
			LOG_PRINTF("%d %d %d %d\n", globals->lib_table.table[lib_index].lowest_syscall - base_syscall,
				globals->lib_table.table[lib_index].highest_syscall - base_syscall,
				globals->lib_table.table[lib_index].lowest_syscall,
				globals->lib_table.table[lib_index].highest_syscall);
		}

		LOG_PRINTF("\n");
	}

	// On CFW there is a higher syscall difference between SysmemUserForUser
	// and lower libraries than without it.
	if (is_cfw)
		scr_puts("Using offsets for Custom Firmware\n");


	// Fill remaining data after the reference library is known
	for (lib_index = 0; lib_index < globals->lib_table.num; lib_index++) {
		LOG_PRINTF("Completing library...%d\n", lib_index);
		complete_library(&(globals->lib_table.table[lib_index]), ref_lib_index, is_cfw);
	}

	LOG_PRINTF("\n==LIBRARY TABLE DUMP==\n");
	for (lib_index = 0; lib_index < globals->lib_table.num; lib_index++) {
		LOG_PRINTF("->Index: %d, name %s\n", lib_index, (u32)globals->lib_table.table[lib_index].name);
		LOG_PRINTF("predicted syscall range from 0x%08X to 0x%08X\n",
			globals->lib_table.table[lib_index].lowest_syscall,
			globals->lib_table.table[lib_index].lowest_syscall
				+ globals->lib_table.table[lib_index].num_lib_exports
				+ globals->lib_table.table[lib_index].gap - 1);

		fd = open_nids_file(globals->lib_table.table[lib_index].name);

		for (nid_index = 0; nid_index < globals->nid_table.num; nid_index++) {
			if (globals->nid_table.table[nid_index].lib_index == lib_index) {
				syscall = globals->nid_table.table[nid_index].call & SYSCALL_MASK_RESOLVE ?
					syscall = globals->nid_table.table[nid_index].call :
					syscall = GET_SYSCALL_NUMBER(globals->nid_table.table[nid_index].call);

				LOG_PRINTF("[%d] 0x%08X 0x%08X ", nid_index, globals->nid_table.table[nid_index].nid, syscall);

				// Check nid in table against the estimated nids
				if (globals->syscalls_known && fd > 0) {
					int cur_nid = 0;
					pos = -1;
					i = globals->lib_table.table[lib_index].lowest_index;

					do {
						pos++;

						if (i >= globals->lib_table.table[lib_index].num_lib_exports) {
							// gap
							pos += globals->lib_table.table[lib_index].gap;

							sceIoLseek(fd, 0, PSP_SEEK_SET);
							for (i = 0; i < globals->lib_table.table[lib_index].lowest_index; i++) {
								sceIoLseek(fd, 4 * i, PSP_SEEK_SET);
								sceIoRead(fd, &cur_nid, sizeof(cur_nid));

								if (cur_nid == globals->nid_table.table[nid_index].nid)
									break;
								else
									pos++;
							}
							break;
						}

						sceIoLseek(fd, 4 * i++, PSP_SEEK_SET);
						sceIoRead(fd, &cur_nid, sizeof(int));
					} while (cur_nid != globals->nid_table.table[nid_index].nid)

					// format: estimated call, index in nid-file, correctly estimated?
					LOG_PRINTF("0x%08X %d %s\n", globals->lib_table.table[lib_index].lowest_syscall + pos, lib_index,
						globals->lib_table.table[lib_index].lowest_syscall + pos == syscall ?
							(int)"YES" : (int)"NO";
				} else
					LOG_PRINTF("\n");
			}
		}

		sceIoClose(fd);

		LOG_PRINTF("\n\n");
	}
}
#endif


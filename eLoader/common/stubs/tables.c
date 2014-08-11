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

#ifdef DEBUG
static void log_lib(tSceLibrary lib)
{
	dbg_printf("\n-->Library name: %s\n"
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
		"--Total library exports: %d\n"
		"--Lowest NID/SYSCALL:  0x%08X/0x%08X\n"
		"--Lowest index in file: %d\n"
#endif
		"--Calling mode: %d\n",
		lib.name,
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
		lib.num_lib_exports,
		lib.lowest_nid, lib.lowest_syscall,
		lib.lowest_index,
#endif
		lib.calling_mode
	);
}
#endif

// Return real instruction that makes the system call (jump or syscall)
static int get_good_call(const void *buf)
{
	// Dirty hack here :P but works
	if(*(const int *)buf & SYSCALL_MASK_IMPORT)
		buf += 4;
	return *(const int *)buf;
}

// Adds a new library
int add_lib(const tSceLibrary lib)
{
	// Check if library exists
	int index = get_lib_index(lib.name);

	// Doesn't exist, insert new
	if (index < 0) {
		index = globals->lib_num;
		// Check if there's space on the table
		if (index >= MAX_LIBRARIES) {
			dbg_printf("->WARNING: Library table full, skipping new library %s\n", lib.name);
			return -1;
		}

		globals->lib_num++;
	}

	// Copying new library
	memcpy(&(globals->lib_table[index]), &lib, sizeof(lib));


	dbg_printf("Added library %s @ %d\n", globals->lib_table[index].name, index);
#ifdef DEBUG
	log_lib(globals->lib_table[index]);
#endif

	return index;
}

// Adds NID entry to nid_table
int add_nid(int nid, int call, int lib)
{
	int index;

    	NID_DBG_PRINTF("Adding NID 0x%08X to table...\n", nid);

	// Check if NID already exists in table (by another estimation for example)
	index = get_nid_index(nid);
	if (index < 0) {
		index = globals->nid_num;

		// Doesn't exist, insert new
		if (index >= NID_TABLE_SIZE) {
			dbg_printf("-> FATAL: NID TABLE IS FULL\n");
			return -1;
		}

		globals->nid_table[index].nid = nid;
		globals->nid_table[index].call = call;
		globals->nid_table[index].lib = lib;
		globals->nid_num++;
		NID_DBG_PRINTF("-> Newly added @ %d\n", index);
	} else {
		// If it exists, just change the old call with the new one
		globals->nid_table[index].call = call;
		NID_DBG_PRINTF("-> Modified @ %d\n", index);
	}

	return index;
}

int add_stub(const tStubEntry *stub)
{
	int *cur_nid;
	int *cur_call;
	int i, nid_index, lib_index, good_call, nid;
	int num = 0;
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	int syscall_num;
#endif

	NID_DBG_PRINTF("-->Processing library: %s\n", stub->lib_name);

	// Get actual call
	cur_call = stub->jump_p;
	good_call = get_good_call(cur_call);

	// Only process if syscall and if the import is already resolved
	if (!(good_call & SYSCALL_MASK_RESOLVE) &&
		(GET_SYSCALL_NUMBER(good_call) != SYSCALL_IMPORT_NOT_RESOLVED_YET)) {

		// Get current NID
		cur_nid = stub->nid_p;

		// Is this library on the table?
		lib_index = get_lib_index(stub->lib_name);

		if (lib_index < 0) {
			// New library
#ifndef XMB_LAUNCHER
			// Even if the stub appears to be valid, we shouldn't overflow the static arrays
			if (globals->lib_num >= MAX_LIBRARIES) {
				dbg_printf(" LIBRARY TABLE COUNTER: 0x%08X\n",
					globals->lib_num);
				dbg_printf(" LIBRARY TABLES TOO SMALL\n");
				sceKernelExitGame();
			}
#endif
			lib_index = globals->lib_num;
			NID_DBG_PRINTF(" --> New: %d\n", lib_index);

			strcpy(globals->lib_table[lib_index].name, stub->lib_name);
			globals->lib_table[lib_index].calling_mode = SYSCALL_MODE;

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
			// Initialize lowest syscall on library table
			globals->lib_table[lib_index].lowest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));
			globals->lib_table[lib_index].lowest_nid = *cur_nid;

			// Initialize highest syscall on library table
			globals->lib_table[lib_index].highest_syscall = GET_SYSCALL_NUMBER(get_good_call(cur_call));

			NID_DBG_PRINTF("Current lowest nid/syscall: 0x%08X/0x%08X\n",
				globals->lib_table[lib_index].lowest_syscall, globals->lib_table[lib_index].lowest_nid);
#endif

			// New library
			globals->lib_num++;
		} else {
				// Old library
			NID_DBG_PRINTF(" --> Old: %d\n", lib_index);

			NID_DBG_PRINTF("Number of imports of this stub: %d\n", stub->stub_size);

			if (globals->lib_table[lib_index].calling_mode != SYSCALL_MODE) {
				dbg_printf(" ERROR OLD CALL MODE IS SYSCALL, NEW IS JUMP 0x%08X\n", &lib_index);
				return 0;
			}
		}
#ifdef DEBUG
		log_lib(globals->lib_table[lib_index]);
#endif

		// Browse all stubs defined by this header
		for (i = 0; i < stub->stub_size; i++) {
			nid = *cur_nid;
			NID_DBG_PRINTF(" --Current NID: 0x%08X\n", nid);

			// If NID is already in, don't put it again
			nid_index = get_nid_index(nid);
			if (nid_index < 0) {
					// Fill NID table
#ifdef XMB_LAUNCHER
				good_call = get_good_call(cur_call);
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
				// Check lowest syscall
				syscall_num = GET_SYSCALL_NUMBER(good_call);
#endif
#else
				add_nid(nid, get_good_call(cur_call), lib_index);
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
				// Check lowest syscall
				syscall_num = GET_SYSCALL_NUMBER(globals->nid_table[nid_index].call);
#endif
#endif
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
				NID_DBG_PRINTF("  --> with syscall 0x%08X\n", syscall_num);

				if (syscall_num < globals->lib_table[lib_index].lowest_syscall) {
					globals->lib_table[lib_index].lowest_syscall = syscall_num;
					globals->lib_table[lib_index].lowest_nid = nid; // globals->nid_table[i].nid;
					NID_DBG_PRINTF("New lowest syscall/nid: 0x%08X/0x%08X\n",
						globals->lib_table[lib_index].lowest_syscall, globals->lib_table[lib_index].lowest_nid);
				}

				if (syscall_num > globals->lib_table[lib_index].highest_syscall) {
					globals->lib_table[lib_index].highest_syscall = syscall_num;
					NID_DBG_PRINTF("New highest syscall/nid: 0x%08X/0x%08X\n", globals->lib_table[lib_index].highest_syscall, nid);
				}
#endif
				num++;
			}
			cur_nid++;
			cur_call += 2;
		}
	}

	return num;
}

// Returns nid_table index where the call is found, -1 if not found
int get_call_index(int call)
{
    	int i;

	for (i = 0; i < globals->nid_num; i++)
		if (globals->nid_table[i].call == call)
			return i;

	return -1;
}

// Returns lib_table index where the library is found, -1 if not found
int get_lib_index(const char *name)
{
	int i;

	if (name != NULL)
		for (i = 0; i < globals->lib_num; i++)
			if (!strcmp(name, globals->lib_table[i].name))
				return i;

	return -1;
}

// Returns nid_table index where the nid is found, -1 if not found
int get_nid_index(int nid)
{
    	int i;

	for (i = 0; i < globals->nid_num; i++)
		if (globals->nid_table[i].nid == nid)
			return i;

	return -1;
}

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
// Return index in nid_table for closest higher known NID
int get_higher_known_nid(int index, int nid)
{
    	int i;
	int higher;

	for(i = 0; globals->nid_table[i].lib_index != index &&
		globals->nid_table[i].nid <= nid; i++)
		if (i >= NID_TABLE_SIZE)
			return -1;
	higher = i;

	while (i < NID_TABLE_SIZE) {
		if (globals->nid_table[i].lib_index == index &&
			globals->nid_table[i].nid > nid &&
			globals->nid_table[i].nid < globals->nid_table[higher].nid)

			higher = i;

		i++;
	}

	return higher;
}

// Return index in nid_table for closest lower known NID
int get_lower_known_nid(int index, int nid)
{
    	int i;
	int lower;

	for(i = 0; globals->nid_table[i].lib_index != index &&
		globals->nid_table[i].nid >= nid; i++)
		if (i >= NID_TABLE_SIZE)
			return -1;
	lower = i;

	while (i < NID_TABLE_SIZE) {
		if (globals->nid_table[i].lib_index == index &&
			globals->nid_table[i].nid > nid &&
			globals->nid_table[i].nid > globals->nid_table[lower].nid)

			lower = i;

		i++;
	}

	return lower;
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

	my_lib = globals->lib_table[lib_index];
	*low = 0;
	*high = 0;

	for (i = 0; i < (int)globals->lib_num; i++) {
		if (i == lib_index)
			continue;

		if (globals->lib_table[i].name == NULL)
			break;

		lib = globals->lib_table[i];
		l = lib.lowest_syscall;
		h = lib.highest_syscall;

		if (l > my_lib.highest_syscall && (l < *high || *high == 0))
			*high = l;

		if (h < my_lib.lowest_syscall && h > *low)
			*low = h;
	}

	return *low && *high;
}
#endif


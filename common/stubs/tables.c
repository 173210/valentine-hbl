#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <hbl/modmgr/elf.h>
#include <common/debug.h>
#include <common/utils.h>
#include <common/globals.h>
#include <config.h>

// Return real instruction that makes the system call (jump or syscall)
static int get_good_call(const void *buf)
{
	int ret;

	ret = ((int *)buf)[0];
	return ret & J_OPCODE ? ret : ((int *)buf)[1];
}

// Adds NID entry to nid_table
int add_nid(int nid, int call)
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
	int i, nid_index, good_call, nid;
	int num = 0;

	NID_DBG_PRINTF("-->Processing library: %s\n", stub->lib_name);

	// Get actual call
	cur_call = stub->jump_p;
	good_call = get_good_call(cur_call);

	// Only process if syscall and if the import is already resolved
	if ((good_call & 0xFC00003F) == SYSCALL_OPCODE
		&& GET_SYSCALL_NUMBER(good_call) != SYSCALL_IMPORT_NOT_RESOLVED_YET) {

		// Get current NID
		cur_nid = stub->nid_p;

		// Browse all stubs defined by this header
		for (i = 0; i < stub->stub_size; i++) {
			nid = *cur_nid;
			NID_DBG_PRINTF(" --Current NID: 0x%08X\n", nid);

			// If NID is already in, don't put it again
			nid_index = get_nid_index(nid);
			if (nid_index < 0) {
					// Fill NID table
				add_nid(nid, get_good_call(cur_call));
				num++;
			}
			cur_nid++;
			cur_call += 2;
		}
	}

	return num;
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

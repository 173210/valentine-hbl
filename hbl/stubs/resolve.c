#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <hbl/modmgr/elf.h>
#include <hbl/modmgr/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/stubs/resolve.h>
#include <hbl/eloader.h>
#include <exploit_config.h>

// Subsitutes the right instruction
void resolve_call(int *call_to_resolve, u32 call_resolved)
{
	// SYSCALL
	if(!(call_resolved & SYSCALL_MASK_RESOLVE))
	{
		*call_to_resolve = JR_RA_OPCODE;
		*(++call_to_resolve) = call_resolved;
	}

	// JUMP
	else
	{
		*call_to_resolve = call_resolved;
		*(++call_to_resolve) = NOP_OPCODE;
	}
}

static u32 get_jump_from_export(u32 nid, SceLibraryEntryTable *pexports)
{
	if( pexports != NULL){
		u32* pnids = (u32*)pexports->entrytable;
		u32* pfunctions = (u32*)((u32)pexports->entrytable + (u16)pexports->stubcount * 4);

		// Insert NIDs on NID table
		int i;
		for (i=0; i<(u16)pexports->stubcount; i++)
		{
			if( pnids[i] == nid){
				dbg_printf("NID FOUND in %s: 0x%08X Function: 0x%08X\n",
					pexports->libname, pnids[i], pfunctions[i]);
				return MAKE_JUMP(pfunctions[i]);
			}
		}
	}
	return 0;
}

// Resolves imports in ELF's program section already loaded in memory
// Returns number of resolves
unsigned int resolve_imports(const tStubEntry *pstub_entry, unsigned int stubs_size)
{
	int i, j, nid_index;
	int *cur_nid;
	int *cur_call;
	int real_call;
	SceLibraryEntryTable* utility_exp = NULL;
	unsigned int resolving_count = 0;

	dbg_printf("RESOLVING IMPORTS. Stubs size: %d\n", stubs_size);
	/* Browse ELF stub headers */
	for(i=0; i<stubs_size; i+=sizeof(tStubEntry))
	{
		dbg_printf("Pointer to stub entry: 0x%08X\n", (u32)pstub_entry);

		cur_nid = pstub_entry->nid_p;
		cur_call = pstub_entry->jump_p;

		dbg_printf("Current library: %s\n", (u32)pstub_entry->lib_name);

		// Load utility if necessary
		utility_exp = load_export_util(pstub_entry->lib_name);

		/* For each stub header, browse all stubs */
		for(j=0; j<pstub_entry->stub_size; j++)
		{

			dbg_printf("Current nid: 0x%08X\n", *cur_nid);
			NID_DBG_PRINTF("Current call: 0x%08X\n", (u32)cur_call);

			// Get syscall/jump instruction for current NID
			real_call = 0;

			if( utility_exp != NULL){
				real_call = get_jump_from_export( *cur_nid, utility_exp );

			}

			if( real_call == 0){
				nid_index = get_nid_index(*cur_nid);
				if (nid_index >= 0) {
					NID_DBG_PRINTF("Index for NID on table: %d\n", nid_index);
					real_call = globals->nid_table[nid_index].call;
				}
			}


			u32 hook_call = setup_hook(*cur_nid, real_call);

			if (hook_call != 0)
				real_call = hook_call;

			NID_DBG_PRINTF("Real call before finalization: 0x%08X\n", real_call);

			/* If NID not found in game imports */
			/* generic error/ok if syscall estimation is not available */
			/* OR Syscall estimation if syscall estimation is ON (default)  and library available */
			if (real_call == 0)
				real_call = globals->isEmu ? setup_default_nid(*cur_nid) :
					estimate_syscall((char *)pstub_entry->lib_name, *cur_nid);

			NID_DBG_PRINTF("Real call after finalization: 0x%08X\n", real_call);

			/* If it's an instruction, resolve it */
			/* 0xC -> syscall 0 */
			/* Jumps are always > 0xC */
			if(real_call > 0xC)
			{
				/* Write it in ELF stubs memory */
				resolve_call(cur_call, real_call);
				resolving_count++;
			}

			dbg_printf("Resolved stub 0x%08X: 0x%08X 0x%08X\n", (u32)cur_call, *cur_call, *(cur_call+1));

			cur_nid++;
			cur_call += 2;
		}

		pstub_entry++;
	}

    dbg_printf("RESOLVING IMPORTS: Done.");
	return resolving_count;
}

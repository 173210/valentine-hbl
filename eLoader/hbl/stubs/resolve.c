#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <hbl/eloader.h>
//#include "scratchpad.h"
#include <hbl/mod/elf.h>
#include <hbl/stubs/hook.h>
#include <hbl/mod/modmgr.h>
#include <common/config.h>
#include <hbl/stubs/resolve.h>
#include <common/globals.h>
#include <exploit_config.h>

// Subsitutes the right instruction
void resolve_call(u32 *call_to_resolve, u32 call_resolved)
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

// Returns utility ID that loads the given library
int is_utility(const char* lib_name)
{
	if (strcmp(lib_name, "sceMpeg") == 0 )
		return PSP_MODULE_AV_MPEGBASE;

	if (strcmp(lib_name, "sceAtrac3plus") == 0)
	    return PSP_MODULE_AV_ATRAC3PLUS;

	if (strcmp(lib_name, "sceMp3") == 0)
	    return PSP_MODULE_AV_MP3;

	else if (strcmp(lib_name, "sceNetInet") == 0 ||
			 strcmp(lib_name, "sceNetInet_lib") == 0 ||
	    	 strcmp(lib_name, "sceNetApctl") == 0 ||
	    	 strcmp(lib_name, "sceNetApctl_lib") == 0 ||
	    	 strcmp(lib_name, "sceNetApctl_internal_user") == 0 ||
	    	 strcmp(lib_name, "sceNetApctl_lib2") == 0 ||
	    	 strcmp(lib_name, "sceNetResolver") == 0)
		return PSP_MODULE_NET_INET;

	else if (strcmp(lib_name, "sceNet") == 0 ||
	    	 strcmp(lib_name, "sceNet_lib") == 0)
		return PSP_MODULE_NET_COMMON;

	else if (strcmp(lib_name, "sceNetAdhoc") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhoc_lib") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhocctl") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhocctl_lib") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhocMatching") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhocDownload") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhocDiscover") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhoc") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhoc") == 0 ||
	    	 strcmp(lib_name, "sceNetAdhoc") == 0)
		return PSP_MODULE_NET_ADHOC;

	else if (strcmp(lib_name, "sceHttp") == 0 )
		return PSP_MODULE_NET_HTTP;
/*
	else if (strcmp(lib_name, "sceSsl") == 0 )
		return PSP_MODULE_NET_SSL;
*/
	else
		return -1;
}

static u32 get_jump_from_export(u32 nid, tExportEntry *pexports)
{
	if( pexports != NULL){
		u32* pnids = (u32*)pexports->exports_pointer;
		u32* pfunctions = (u32*)((u32)pexports->exports_pointer + (u16)pexports->num_functions * 4);

		// Insert NIDs on NID table
		int i;
		for (i=0; i<(u16)pexports->num_functions; i++)
		{
			if( pnids[i] == nid){
				LOGSTR3("NID FOUND in %s: 0x%08lX Function: 0x%08lX\n", (u32)pexports->name , pnids[i], pfunctions[i]);
				return MAKE_JUMP(pfunctions[i]);
			}
		}
	}
	return 0;
}

// Resolves imports in ELF's program section already loaded in memory
// Returns number of resolves
unsigned int resolve_imports(tStubEntry* pstub_entry, unsigned int stubs_size)
{
	unsigned int i,j,UNUSED(nid_index);
	u32* cur_nid;
	u32* cur_call;
	u32 real_call;
	tExportEntry* utility_exp = NULL;
	unsigned int resolving_count = 0;

#ifdef HOOK_CHDIR_AND_FRIENDS
    tGlobals * g = get_globals();
    g->chdir_ok = test_sceIoChdir();
#endif

	LOGSTR1("RESOLVING IMPORTS. Stubs size: %d\n", stubs_size);
	/* Browse ELF stub headers */
	for(i=0; i<stubs_size; i+=sizeof(tStubEntry))
	{
		LOGSTR1("Pointer to stub entry: 0x%08lX\n", (u32)pstub_entry);

		cur_nid = pstub_entry->nid_pointer;
		cur_call = pstub_entry->jump_pointer;

		LOGSTR1("Current library: %s\n", (u32)pstub_entry->library_name);

		// Load utility if necessary
		int mod_id = is_utility((char*)pstub_entry->library_name);
		if (mod_id > 0)
		{
			load_export_utility_module(mod_id, (char*)pstub_entry->library_name, (void **)&utility_exp);
		}

		/* For each stub header, browse all stubs */
		for(j=0; j<pstub_entry->stub_size; j++)
		{

			LOGSTR1("Current nid: 0x%08lX\n", *cur_nid);
			NID_LOGSTR1("Current call: 0x%08lX\n", (u32)cur_call);

			// Get syscall/jump instruction for current NID
			real_call = 0;

			if( utility_exp != NULL){
				real_call = get_jump_from_export( *cur_nid, utility_exp );

			}

			if( real_call == 0){
				nid_index = get_call_nidtable(*cur_nid, &real_call);
				NID_LOGSTR1("Index for NID on table: %d\n", nid_index);
			}


			u32 hook_call = setup_hook(*cur_nid, real_call);

			if (hook_call != 0)
				real_call = hook_call;

			NID_LOGSTR1("Real call before estimation: 0x%08lX\n", real_call);

			/* If NID not found in game imports */
			/* generic error/ok if syscall estimation is not available */
			/* OR Syscall estimation if syscall estimation is ON (default)  and library available */
			if (real_call == 0)
			{
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
                real_call = setup_default_nid(*cur_nid);
#else
				real_call = estimate_syscall((char *)pstub_entry->library_name, *cur_nid, g->syscalls_known ? FROM_LOWEST : FROM_CLOSEST);
#endif
			}

			NID_LOGSTR1("Real call after estimation: 0x%08lX\n", real_call);

			/* If it's an instruction, resolve it */
			/* 0xC -> syscall 0 */
			/* Jumps are always > 0xC */
			if(real_call > 0xC)
			{
				/* Write it in ELF stubs memory */
				resolve_call(cur_call, real_call);
				resolving_count++;
			}

			LOGSTR3("Resolved stub 0x%08lX: 0x%08lX 0x%08lX\n", (u32)cur_call, *cur_call, *(cur_call+1));

			CLEAR_CACHE;

			cur_nid++;
			cur_call += 2;
		}

		pstub_entry++;
	}

    LOGSTR0("RESOLVING IMPORTS: Done.");
	return resolving_count;
}

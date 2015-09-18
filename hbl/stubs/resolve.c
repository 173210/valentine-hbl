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
#include <config.h>

static int get_jump_from_export(int *dst, u32 nid, SceLibraryEntryTable *pexports)
{
	if (dst == NULL || pexports == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDRESS;

	u32* pnids = (u32*)pexports->entrytable;
	u32* pfunctions = (u32*)((u32)pexports->entrytable + (u16)pexports->stubcount * 4);

	// Insert NIDs on NID table
	int i;
	for (i=0; i<(u16)pexports->stubcount; i++)
	{
		if( pnids[i] == nid){
			dbg_printf("NID FOUND in %s: 0x%08X Function: 0x%08X\n",
				pexports->libname, pnids[i], pfunctions[i]);
			dst[0] = J_ASM(pfunctions[i]);
			dst[1] = NOP_ASM;

			return 0;
		}
	}

	return SCE_KERNEL_ERROR_ERROR;
}

// Resolves imports in ELF's program section already loaded in memory
int resolve_imports(tStubEntry *pstub_entry, unsigned int stubs_size)
{
	UtilModInfo *util_mod;
	tStubEntry *netLib;
	uintptr_t btm;
	int i, res, nid_index;
	int *cur_nid;
	int *cur_call;
#ifndef NO_SYSCALL_RESOLVER
	int netCommonIsImported = 0;
#endif
	SceLibraryEntryTable* utility_exp = NULL;

	dbg_printf("RESOLVING IMPORTS. Stubs size: %d\n", stubs_size);

	if (pstub_entry == NULL)
		return SCE_KERNEL_ERROR_ERROR;

#ifndef NO_SYSCALL_RESOLVER
	res = loadNetCommon();
	if (res)
		return res;

	netLib = getNetLibStubInfo();
	if (netLib == NULL)
		return SCE_KERNEL_ERROR_ERROR;
#endif

	for (btm = (uintptr_t)pstub_entry + stubs_size;
		(uintptr_t)pstub_entry < btm; pstub_entry++)
	{
		dbg_printf("Pointer to stub entry: 0x%08X\n", (u32)pstub_entry);

		cur_nid = pstub_entry->nid_p;
		cur_call = pstub_entry->jump_p;

		dbg_printf("Current library: %s\n", (u32)pstub_entry->lib_name);

		util_mod = get_util_mod_info(pstub_entry->lib_name);
		if (util_mod != NULL) {
#ifndef NO_SYSCALL_RESOLVER
			if (util_mod->id == PSP_MODULE_NET_COMMON)
				netCommonIsImported = 1;
#endif
			// Load utility if necessary
			utility_exp = load_export_util(util_mod, pstub_entry->lib_name);
			if (utility_exp == NULL) {
				dbg_puts("warning: failed to load utility");
				continue;
			}

			for (i = 0; i < pstub_entry->stub_size; i++) {
				get_jump_from_export(cur_call, *cur_nid, utility_exp);

				cur_nid++;
				cur_call += 2;
			}
		} else {
#ifdef NO_SYSCALL_RESOLVER
			for (i = 0; i < pstub_entry->stub_size; i++) {
				nid_index = get_nid_index(*cur_nid);
				if (nid_index >= 0) {
					NID_DBG_PRINTF("Index for NID on table: %d\n", nid_index);
					cur_call[0] = JR_ASM(REG_RA);
					cur_call[1] = globals->nid_table[nid_index].call;
				} else
					hook(cur_call, *cur_nid);

				cur_nid++;
				cur_call += 2;
			}
#else
			res = resolveSyscall(pstub_entry, netLib);
			if (res)
				dbg_printf("warning: failed to resolve syscall: 0x%08X\n", res);

			for (i = 0; i < pstub_entry->stub_size; i++)
				hook((int32_t *)pstub_entry->jump_p + i * 2,
					((int32_t *)pstub_entry->nid_p)[i]);
#endif
		}
	}

#ifndef NO_SYSCALL_RESOLVER
	if (!netCommonIsImported)
		return unloadNetCommon();
#endif

	dbg_puts("RESOLVING IMPORTS: Done.");
	return 0;
}

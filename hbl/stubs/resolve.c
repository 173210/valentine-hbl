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

static int resolveImportWithUtility(const tStubEntry *import,
	const UtilModInfo *utility)
{
	SceLibraryEntryTable *utilityEntry;
	int *nid;
	int *call;
	int i;

	if (import == NULL || utility == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDRESS;

	utilityEntry = load_export_util(utility, import->lib_name);
	if (utilityEntry == NULL)
		return SCE_KERNEL_ERROR_ERROR;

	nid = import->nid_p;
	call = import->jump_p;
	for (i = 0; i < import->stub_size; i++) {
		get_jump_from_export(call, *nid, utilityEntry);

		nid++;
		call += 2;
	}

	return 0;
}

#ifdef NO_SYSCALL_RESOLVER
static int resolveImportWithSyscall(const tStubEntry *import)
{
	int *nid;
	int *call;
	int i, index;

	if (import == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDRESS;

	nid = import->nid_p;
	call = import->jump_p;
	for (i = 0; i < import->stub_size; i++) {
		index = get_nid_index(*nid);
		if (index >= 0) {
			NID_DBG_PRINTF("Index for NID on table: %d\n", index);
			call[0] = JR_ASM(REG_RA);
			call[1] = globals->nid_table[index].call;
		} else
			hook(call, *nid);

		nid++;
		call += 2;
	}

	return 0;
}
#else
static int resolveImportWithSyscall(tStubEntry *import,
	struct syscallResolver *ctx)
{
	int *nid;
	int *call;
	int i, res;

	if (import == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDRESS;

	res = resolveSyscall(import, ctx);
	if (res)
		dbg_printf("warning: failed to resolve syscall: 0x%08X\n", res);

	nid = import->nid_p;
	call = import->jump_p;
	for (i = 0; i < import->stub_size; i++)
		hook((int32_t *)import->jump_p + i * 2,
			((int32_t *)import->nid_p)[i]);

	return 0;
}
#endif

// Resolves imports in ELF's program section already loaded in memory
int resolve_imports(tStubEntry *pstub_entry, unsigned int stubs_size)
{
	UtilModInfo *util_mod;
	uintptr_t btm;
	int res;
#ifndef NO_SYSCALL_RESOLVER
	struct syscallResolver ctx;
	UtilModInfo *netCommon = NULL;
	tStubEntry *netCommonImport = NULL;
#endif

	dbg_printf("RESOLVING IMPORTS. Stubs size: %d\n", stubs_size);

	if (pstub_entry == NULL)
		return SCE_KERNEL_ERROR_ERROR;

#ifndef NO_SYSCALL_RESOLVER
	res = initSyscallResolver(&ctx);
	if (res)
		return res;
#endif

	for (btm = (uintptr_t)pstub_entry + stubs_size;
		(uintptr_t)pstub_entry < btm; pstub_entry++)
	{
		dbg_printf("Pointer to stub entry: 0x%08X\n", (u32)pstub_entry);

		dbg_printf("Current library: %s\n", (u32)pstub_entry->lib_name);

		util_mod = get_util_mod_info(pstub_entry->lib_name);
		if (util_mod != NULL) {
#ifndef NO_SYSCALL_RESOLVER
			if (util_mod->id == PSP_MODULE_NET_COMMON) {
				netCommon = util_mod;
				netCommonImport = pstub_entry;
				continue;
			}
#endif
			res = resolveImportWithUtility(pstub_entry, util_mod);
		} else {
#ifdef NO_SYSCALL_RESOLVER
			res = resolveImportWithSyscall(pstub_entry);
#else
			res = resolveImportWithSyscall(pstub_entry, &ctx);
#endif
		}

		if (res) {
			dbg_printf("%s: warning: failed to resolve import: 0x%08X\n",
				__func__, res);
			continue;
		}
	}

#ifndef NO_SYSCALL_RESOLVER
	deinitSyscallResolver(&ctx);
	if (netCommon != NULL && netCommonImport != NULL) {
		res = resolveImportWithUtility(netCommonImport, netCommon);
		if (res)
			dbg_printf("warning: %s: failed to resolve import: 0x%08X\n",
				__func__, res);
	}
#endif

	dbg_puts("RESOLVING IMPORTS: Done.");
	return 0;
}

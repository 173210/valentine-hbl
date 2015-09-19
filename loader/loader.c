/* Half Byte Loader loader :P */
/* This initializes and loads HBL on memory */

#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/cache.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/path.h>
#include <common/prx.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <loader/freemem.h>
#include <loader/runtime.h>
#include <hbl/eloader.h>
#include <config.h>

#define FIRST_LOG "Loader running\n"

HBL_MODULE_INFO("LOADER", PSP_MODULE_USER, MAJOR_VER, MINOR_VER);

#ifdef RESET_HOME_LANG
// Reset language and button assignment for the HOME screen to system defaults
static void resetHomeSettings()
{
	int lang;
	int button;

	// Get system language, default to English on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &lang) < 0)
		lang = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;

	// Get button assignment, default to X = Enter on error
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &button) < 0)
		button = 1; // X = Enter

	sceImposeSetLanguageMode(lang, button);
}
#endif

static void (* run_eloader)();

static void *hblMalloc(const char *name, SceSize size, void *p)
{
	SceUID blockid;

	dbg_printf("%s: size: %d, p: 0x%08X\n", __func__, size, (int)p);

	if (name == NULL)
		return NULL;

	if (p == NULL) {
		blockid = sceKernelAllocPartitionMemory(
			2, name, PSP_SMEM_Low, size, NULL);
		if (blockid < 0) {
			dbg_printf("FAILED: 0x%08X\n", blockid);
			return NULL;
		}

		p = sceKernelGetBlockHeadAddr(blockid);

		if ((int)p + size >= PRX_LOAD_ADDRESS) {
			blockid = sceKernelAllocPartitionMemory(
				2, name, PSP_SMEM_High, size, NULL);
			if (blockid < 0) {
				dbg_printf("FAILED: 0x%08X\n", blockid);
				return NULL;
			}
		}
	} else {
		blockid = sceKernelAllocPartitionMemory(2, name,
			PSP_SMEM_Addr, size, p);
		if (blockid < 0) {
			dbg_printf("FAILED: 0x%08X\n", blockid);
			return NULL;
		}
	}

	return p;
}

// Loads HBL to memory
static int load_hbl()
{
	_sceModuleInfo modinfo;
	Elf32_Ehdr ehdr;
	SceUID fd;
	void *p = NULL;
	int ret;

	fd = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777);
	if (fd < 0) {
		scr_printf(" FAILED TO LOAD HBL 0x%08X\n", fd);
		sceIoClose(fd);
		return fd;
	}

	dbg_printf("Loading HBL...\n");
	dbg_printf(" Reading ELF header...\n");
	sceIoRead(fd, &ehdr, sizeof(ehdr));

	dbg_printf(" Loading PRX...\n");
	ret = prx_load(fd, 0, &ehdr, &modinfo, &p, hblMalloc);
	if (ret <= 0) {
		scr_printf(" ERROR READING HBL 0x%08X\n", ret);
		sceIoClose(fd);
		return ret;
	}

	sceIoClose(fd);

	dbg_printf(" Resolving Stubs...\n");
	ret = resolveHblSyscall((void *)(modinfo.stub_top + (uintptr_t)p),
		modinfo.stub_end - modinfo.stub_top);
	if (ret) {
		scr_printf(" ERROR RESOLVING STUBS 0x%08X\n", ret);
		return ret;
	}

	run_eloader = (void *)((int)ehdr.e_entry + (int)p);

	dbg_printf("HBL loaded to allocated memory @ 0x%08X\n", (int)run_eloader);

	// Commit changes to RAM
	if (isImported(sceKernelDcacheWritebackRange))
		sceKernelDcacheWritebackRange(run_eloader, ret);
	else
		sceKernelDcacheWritebackAll();

	hblIcacheFillRange(run_eloader, (void *)((int)run_eloader + ret));

	return 0;
}

#if defined(DEBUG) || defined(FORCE_FIRST_LOG)
//reset the contents of the debug file;
static void log_init()
{
	SceUID fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);

	sceIoWrite(PSPLINK_OUT, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));
	sceIoWrite(fd, FIRST_LOG, sizeof(FIRST_LOG) - sizeof(char));

	sceIoClose(fd);
}
#endif

#if defined(DEBUG) && !defined(LAUNCHER)
static void dbg_init()
{
	tStubEntry *entry;
	int i, j;

	i = 0;
	for (entry = (tStubEntry *)0x08800000;
		(int)entry < 0x0A000000;
		entry = (tStubEntry *)((int)entry + 4)) {

		while (elf_check_stub_entry(entry)) {
			if (!strcmp(entry->lib_name, "IoFileMgrForUser"))
				for (j = 0; j < entry->stub_size; j++) {
					switch (((int *)entry->nid_p)[j]) {
						case 0x109F50BC:
							memcpy((void *)((int)sceIoOpen | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
						case 0x810C4BC3:
							memcpy((void *)((int)sceIoClose | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
						case 0x42EC03AC:
							memcpy((void *)((int)sceIoWrite | 0x40000000), entry->jump_p + j * 8, 8);
							i++;
							break;
					}
					if (i >= 3)
						return;
				}

			entry++;
		}
	}

	hblIcacheFillRange(sceIoOpen, (void *)sceIoOpen + 8);
	hblIcacheFillRange(sceIoClose, (void *)sceIoClose + 8);
	hblIcacheFillRange(sceIoWrite, (void *)sceIoWrite + 8);
}
#endif

static int test_sceIoChdir()
{
	SceUID fd;

	sceIoChdir(HBL_ROOT);

	fd = sceIoOpen(HBL_PRX, PSP_O_RDONLY, 0777);
	if (fd > 0) {
		sceIoClose(fd);
		dbg_printf("sceIoChdir failed!\n");
		return 1;
	}

	return 0;
}

// "cache" for the firmware version
// 0 means unknown

// cache for the psp model
// 0 means unknown
// 1 is PSPGO


// New method by neur0n to get the firmware version from the
// module_sdk_version export of sceKernelLibrary
// http://wololo.net/talk/viewtopic.php?f=4&t=128
static void get_module_sdk_version()
{
	int i, cnt;

	SceLibraryEntryTable *tbl = *(SceLibraryEntryTable **)
		(findstr("sceKernelLibrary", (void *)0x08800300, 0x00001000) + 32);

	cnt = tbl->vstubcount + tbl->stubcount;

	// dbg_printf("cnt is 0x%08X \n", cnt);

	for (i = 0; ((int *)tbl->entrytable)[i] != 0x11B97506; i++)
		if (i >= cnt) {
			dbg_printf("Warning: Cannot find module_sdk_version\n");
			return;
		}

	globals->module_sdk_version = *((int **)tbl->entrytable)[i + cnt];
}

static int detectEmu()
{
	SceUID fd;
	SceIoDirent dir;
	int ret;

	fd = sceIoDopen(HBL_ROOT);
	if (fd < 0) {
		dbg_printf("%s: Opening " HBL_ROOT " failed: 0x%08X\n",
			__func__, fd);
		globals->isEmu = 0;
		return fd;
	}

	memset(&dir, 0, sizeof(dir));
	ret = sceIoDread(fd, &dir);
	if (ret < 0)
		dbg_printf("%s: Reading " HBL_ROOT " failed: 0x%08X\n",
			__func__, ret);

	globals->isEmu = (ret < 0 || dir.d_name[0] != '.' || dir.d_name[2]);

	return sceIoDclose(fd);
}

static void hook_init()
{
	int i;

	if (globals->isEmu)
		for (i = 0; i < MAX_OPEN_DIR_VITA; i++) {
				globals->dirFix[i][0] = -1;
				globals->dirFix[i][1] = -1;
		}

	globals->chdir_ok = test_sceIoChdir();
	globals->memSema = sceKernelCreateSema("hblmemsema", 0, 1, 1, 0);
	globals->thSema = sceKernelCreateSema("hblthSema", 0, 1, 1, 0);
	globals->cbSema = sceKernelCreateSema("hblcbsema", 0, 1, 1, 0);
	globals->audioSema = sceKernelCreateSema("hblaudiosema", 0, 1, 1, 0);
	globals->ioSema = sceKernelCreateSema("hbliosema", 0, 1, 1, 0);
}

int start_thread()
{
	// Free memory
	scr_puts("Freeing memory");
	free_game_memory();
#ifdef NO_SYSCALL_RESOLVER
	scr_puts("Building NIDs table with utilities");
	load_utils();
	p2_add_stubs();
#endif
#ifdef DISABLE_UNLOAD_UTILITY_MODULES
	UnloadModules();
#else
	unload_utils();
#endif
#ifdef NO_SYSCALL_RESOLVER
	scr_puts("Building NIDs table with savedata utility");
	p5_add_stubs();
#endif
	scr_puts("Initializing hook");
	hook_init();

#ifdef RESET_HOME_LANG
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeSettings();
#endif

	scr_puts("Loading HBL");
	if (load_hbl()) {
		if (isImported(sceKernelExitGame)) {
			sceKernelExitGame();
			return 0;
		} else if (isImported(sceKernelExitGameWithStatus))
			return sceKernelExitGameWithStatus(-1);
	}

	scr_puts("Running HBL");
	run_eloader();

	return 0;
}

// Entry point
#ifdef LAUNCHER
int module_start()
#else
void _start() __attribute__ ((section (".text.start"), noreturn));
void _start()
#endif
{
	int ret;

#if !defined(LAUNCHER) && (defined(DEBUG) || !defined(NO_SYSCALL_RESOLVER))
	initLoaderStubs();
#endif
#ifdef DEBUG
	log_init();
#endif

#ifndef LAUNCHER
	globals->isEmu = 1;
#ifdef NO_SYSCALL_RESOLVER
	globals->nid_num = 0;

	p2_add_stubs();
#else
	ret = unloadNetCommon();
	if (ret != 0 && ret != 0x80110803)
		dbg_printf("warning: failed to unload a module 0x%08X\n", ret);
#endif
	ret = resolveHblSyscall(libStub, (size_t)libStubSize);
	if (ret)
		dbg_printf("warning: failed to resolve HBL stub 0x%08X\n", ret);

	synciLoaderStub();
#endif
#if !defined(DEBUG) && defined(FORCE_FIRST_LOG)
	log_init();
#endif

	scr_init();
	scr_puts("Starting HBL " VER_STR " http://wololo.net/hbl/");
#ifdef DEBUG
#ifdef NID_DEBUG
	scr_puts("DEBUG version (+NIDS)");
#else
	scr_puts("DEBUG version");
#endif
#endif

#ifdef PRE_LOADER_EXEC
	PRE_LOADER_EXEC
#endif

	// Intialize firmware and model
	get_module_sdk_version();
	detectEmu();
	scr_printf("Firmware version: 0x%08X", globals->module_sdk_version);
	if (globals->isEmu)
		scr_printf(" (emulator)");

	scr_puts("\nFreeing game memory");
	preload_free_game_memory();

	// Create and start eloader thread
	scr_printf("Starting HBL thread\n");
	globals->hblThread = sceKernelCreateThread("HBL", start_thread, 0x18, 0x10000, 0xF0000000, NULL);

	if (globals->hblThread < 0) {
		scr_printf("Error creating HBL thread: 0x%08X\n", globals->hblThread);
		hblExitGameWithStatus(globals->hblThread);
	}

	ret = sceKernelStartThread(globals->hblThread, 0, NULL);
	if (ret) {
		scr_printf("Error starting HBL thread: 0x%08X\n", ret);
		hblExitGameWithStatus(ret);
	}

#ifdef LAUNCHER
	return ret;
#else
	if (isImported(sceKernelExitDeleteThread))
		ret = sceKernelExitDeleteThread(0);
	else if (isImported(sceKernelExitThread)) {
		dbg_printf("%s: warning: only sceKernelExitThread available for suicide\n",
			__func__);
		ret = sceKernelExitThread(0);
	} else {
		dbg_printf("%s: warning: no function availabale for suicide\n",
			__func__);
		goto loop;
	}

	if (ret)
		dbg_printf("%s: exiting current thread failed: 0x%08X\n",
			__func__, ret);

loop:
	// Never executed (hopefully)
	while(1)
		sceKernelDelayThread(0xFFFFFFFF);
#endif
}

/* Half Byte Loader loader :P */
/* This loads HBL on memory */

#include <common/stubs/syscall.h>
#include <common/stubs/tables.h>
#include <common/utils/graphics.h>
#include <common/sdk.h>
#include <loader.h>
#include <common/debug.h>
#include <common/config.h>
#include <common/utils.h>
#include <hbl/eloader.h>
#include <common/malloc.h>
#include <common/globals.h>
#include <common/runtime_stubs.h>
#include <exploit_config.h>
#include <svnversion.h>

#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef AUTO_SEARCH_STUBS
#define AUTO_SEARCH_STUBS
#endif
#endif

// These are the addresses were we copy p5 stuff
// This might overwrite some important things in ram,
// so overwrite these values in exploit_config.h if needed
#ifndef RELOC_MODULE_ADDR_1
#define RELOC_MODULE_ADDR_1 0x09d10000
#endif
#ifndef RELOC_MODULE_ADDR_2
#define RELOC_MODULE_ADDR_2 0x09d30000
#endif
#ifndef RELOC_MODULE_ADDR_3
#define RELOC_MODULE_ADDR_3 0x09d50000
#endif
#ifndef RELOC_MODULE_ADDR_4
#define RELOC_MODULE_ADDR_4 0x09d70000
#endif


#ifdef RESET_HOME_LANG
// Reset language and button assignment for the HOME screen to system defaults
void resetHomeSettings()
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


#ifdef GAME_PRELOAD_FREEMEM
void PreloadFreeMem()
{
	unsigned i;
	int blockids[] = GAME_PRELOAD_FREEMEM;

	for(i = 0; i < sizeof(blockids) / sizeof(int); i++) {
		int ret = sceKernelFreePartitionMemory(*(SceUID *)blockids[i]);
		if (ret < 0)
			LOGSTR("--> ERROR 0x%08X FREEING PARTITON MEMORY ID 0x%08X\n", ret, *(SceUID *)blockids[i]);
	}
}
#endif

#ifdef DELETE_UNKNOWN_FPL
void FreeFpl()
{
	SceUID uid;

	LOGSTR("loader.c: FreeFpl\n");

	for (uid =  0x03000000; uid < 0xE0000000; uid++)
        	if (sceKernelDeleteFpl(uid) >= 0) {
			LOGSTR("Succesfully Deleted FPL ID 0x%08X\n", uid);
			return;
        	}
}
#endif


#ifdef FORCE_CLOSE_FILES_IN_LOADER
void CloseFiles()
{
	SceUID fd;
	int ret;

	LOGSTR("memory.c:CloseFiles\n");

	for (fd = 0; fd < 8; fd++) {
		ret = sceIoClose(fd);
		if (ret != SCE_KERNEL_ERROR_BADF)
			LOGSTR("tried closing file %d, result 0x%08X\n", fd, ret);
	}
}
#endif

void (* run_eloader)();

// Loads HBL to memory
void load_hbl(SceUID hbl_file)
{
	long file_size;
	int ret;
	SceUID HBL_block;

	// Get HBL size
	file_size = sceIoLseek(hbl_file, 0, PSP_SEEK_END);

	sceIoLseek(hbl_file, 0, PSP_SEEK_SET);

	//write_debug(" HBL SIZE ", &file_size, sizeof(file_size));

	// Allocate memory for HBL
#ifdef GAME_PRELOAD_FREEMEM
	PreloadFreeMem();
#endif
#ifdef FPL_EARLY_LOAD_ADDR_LIST
	//early memory cleanup to be able to load HBL at a convenient place
	LOGSTR("loader.c: PreloadFreeFPL\n");

	int uids[] = FPL_EARLY_LOAD_ADDR_LIST;

	for(i = 0; i < sizeof(uids) / sizeof(int); i++)
		if (sceKernelDeleteFpl(*(SceUID *)uids[i]) < 0)
			LOGSTR("--> ERROR 0x%08X Deleting FPL ID 0x%08X\n", ret, *(SceUID *)uids[i]);
#endif


	HBL_block = sceKernelAllocPartitionMemory(
		2, "Valentine", PSP_SMEM_Addr, file_size + sizeof(tGlobals), (void *)HBL_LOAD_ADDR);
	if(HBL_block < 0)
		exit_with_log(" ERROR ALLOCATING HBL MEMORY ", &HBL_block, sizeof(HBL_block));
	run_eloader = sceKernelGetBlockHeadAddr(HBL_block);
	globals = (tGlobals *)((int)run_eloader + file_size);

	// Load HBL to allocated memory
	ret = sceIoRead(hbl_file, (void *)run_eloader, file_size);
	if (ret < 0)
		exit_with_log(" ERROR READING HBL ", &ret, sizeof(ret));

	sceIoClose(hbl_file);

	LOGSTR("HBL loaded to allocated memory @ 0x%08X\n", (int)run_eloader);

	// Commit changes to RAM
	CLEAR_CACHE;
}

// Autoresolves HBL stubs
void resolve_stubs()
{
	int index, ret;
	int num_nids;
	int *cur_stub = (int *)HBL_STUBS_START;
	int nid = 0, syscall = 0;
	char lib_name[MAX_LIB_NAME_LEN];

	ret = cfg_init();

	if (ret < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &ret, sizeof(ret));

	cfg_num_nids_total(&num_nids);

	for (index = 0; index < num_nids; index++) {
		LOGSTR("-Resolving import 0x%08X: ", index * 2 * sizeof(int));

		// NID & library for i-th import
		get_lib_nid(index, lib_name, &nid);

		LOGSTR(lib_name);
		LOGSTR(" 0x%08X\n", nid);

		// Is it known by HBL?
		ret = get_nid_index(nid);

		// If it's known, get the call
		if (ret > 0) {
			LOGSTR("-Found in NID table, using real call\n");
			syscall = globals->nid_table.table[ret].call;
		} else {
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
			LOGSTR("HBL Function missing at 0x%08X, this can lead to trouble\n",  (int)cur_stub);
			syscall = NOP_OPCODE;
#else
			// If not, estimate
			syscall = estimate_syscall(lib_name, nid, globals->syscalls_known ? FROM_LOWEST : FROM_CLOSEST);
#endif
		}

		*cur_stub++ = JR_RA_OPCODE;
		*cur_stub++ = syscall;
	}

	CLEAR_CACHE;

	cfg_close();

	LOGSTR(" ****STUBS SEARCHED\n");
}


#ifndef DISABLE_KERNEL_DUMP

// Erase kernel dump
void init_kdump()
{
	sceIoClose(sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777));
}

// Dumps kmem
void get_kmem_dump()
{
	SceUID fd = sceIoOpen(KDUMP_PATH, PSP_O_CREAT | PSP_O_WRONLY, 0777);
	sceIoWrite(fd, (void*)0x08000000, 0x400000);
	sceIoClose(fd);
}

#endif


// Initializes the savedata dialog loop, opens p5
void p5_open_savedata(int mode)
{
	SceUtilitySavedataParam dialog = {
		.base = {
			.size = sizeof(SceUtilitySavedataParam),
			.language = 1,
			.graphicsThread = 16,
			.accessThread = 16,
			.fontThread = 16,
			.soundThread = 16
		},
		.mode = mode
	};

	sceUtilitySavedataInitStart(&dialog);

	// Wait for the dialog to initialize
	while (sceUtilitySavedataGetStatus() < 2)
#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
		sceKernelDelayThread(256);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
		sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
}

// Runs the savedata dialog loop
void p5_close_savedata()
{
	int status;
	int last_status = -1;

	LOGSTR("entering savedata dialog loop\n");

	while(1) {
		status = sceUtilitySavedataGetStatus();

		if (status != last_status) {
			LOGSTR("status changed from %d to %d\n", last_status, status);
			last_status = status;
		}

		switch(status)
		{
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilitySavedataUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilitySavedataShutdownStart();
				break;

			case PSP_UTILITY_DIALOG_NONE:
				LOGSTR("dialog has shut down\n");
				return;
		}

#ifdef HOOK_sceDisplayWaitVblankStart_WITH_sceKernelDelayThread
		sceKernelDelayThread(256);
#elif defined(HOOK_sceDisplayWaitVblankStart_WITH_sceDisplayWaitVblankStartCB)
		sceDisplayWaitVblankStartCB();
#else
		sceDisplayWaitVblankStart();
#endif
	}
}

int p5_find_add_stubs_to_table(const char *libname, void *p, size_t size, int *index)
{
	int num = 0;
	tStubEntry *pentry = *(tStubEntry **)(memfindsz(libname, p, size) + 40);

	// While it's a valid stub header
	while ((int)pentry->lib_name > 0x08400000 &&
        	(int)pentry->lib_name < 0x08800000 &&
        	pentry->nid_p &&
        	pentry->jump_p) {
		if ((pentry->import_flags != 0x11) && (pentry->import_flags != 0)) {
			// Variable import, skip it
			pentry = (tStubEntry *)((int)pentry + sizeof(int));
		} else {
			num += add_stub_to_table(pentry, index);
			// Next entry
			pentry++;
		}
	}
	
	return num;
}


int add_stubs_to_table(tStubEntry *pentry, int *index)
{
	int num = 0;

	// While it's a valid stub header
	while (elf_check_stub_entry(pentry)) {
		if ((pentry->import_flags != 0x11) && (pentry->import_flags != 0)) {
			// Variable import, skip it
			pentry = (tStubEntry *)((int)pentry + sizeof(int));
		} else {
			num += add_stub_to_table(pentry, index);
			// Next entry
			pentry++;
		}
	}
	
	return num;
}

/* Fills NID Table */
/* Returns NIDs resolved */
/* "pentry" points to first stub header in game */
int build_nid_table()
{
	int num = 0;
	int nlib_stubs;
	int lib_index = 0;

#ifdef AUTO_SEARCH_STUBS
	int cur_stub;
	tStubEntry *stubs[MAX_RUNTIME_STUB_HEADERS];
    
#ifdef LOAD_MODULES_FOR_SYSCALLS
	load_utils();
#endif
	nlib_stubs = search_stubs(stubs);
	LOGSTR("Found %d stubs\n", nlib_stubs);
	if (nlib_stubs == 0)
		exit_with_log(" ERROR: NO LIBSTUBS DEFINED IN CONFIG ", NULL, 0);
	CLEAR_CACHE;

	//DEBUG_PRINT(" build_nid_table() ENTERING MAIN LOOP ", NULL, 0);
	for (cur_stub = 0; cur_stub < nlib_stubs; cur_stub++) {
		NID_LOGSTR("-->CURRENT MODULE LIBSTUB: 0x%08X\n", (int)stubs[cur_stub]);

		num += add_stubs_to_table((tStubEntry *)stubs[cur_stub], &lib_index);
	}

#ifndef HOOK_sceKernelVolatileMemUnlock_WITH_dummy
	sceKernelVolatileMemUnlock(0);
#endif

	p5_open_savedata(PSP_UTILITY_SAVEDATA_SAVE);

	num += p5_find_add_stubs_to_table("scePaf_Module", (void *)0x084C0000, 0x00010000, &lib_index);
	num += p5_find_add_stubs_to_table("sceVshCommonUtil_Module", (void *)0x08760000, 0x00010000, &lib_index);
	num += p5_find_add_stubs_to_table("sceDialogmain_Module", (void *)0x08770000, 0x00010000, &lib_index);

	p5_close_savedata();
	cls();

	p5_open_savedata(PSP_UTILITY_SAVEDATA_AUTOLOAD);

	num += p5_find_add_stubs_to_table("sceVshSDAuto_Module", (void *)0x08410000, 0x00010000, &lib_index);

	p5_close_savedata();

#ifdef LOAD_MODULES_FOR_SYSCALLS
	unload_utils();
#endif
#else
	int ret;
	tStubEntry *pentry;

	// Getting game's .lib.stub address
	ret = cfg_init();
	if (ret < 0)
		exit_with_log(" ERROR INITIALIZING CONFIG ", &ret, sizeof(ret));

	ret = cfg_num_lib_stub(&nlib_stubs);
	if (ret < 0)
	    exit_with_log(" ERROR READING NUMBER OF LIBSTUBS FROM CONFIG ", &ret, sizeof(ret));

	if (nlib_stubs == 0)
		exit_with_log(" ERROR: NO LIBSTUBS DEFINED IN CONFIG ", NULL, 0);

	//DEBUG_PRINT(" build_nid_table() ENTERING MAIN LOOP ", NULL, 0);
	while (nlib_stubs > 0) {
		ret = cfg_next_lib_stub(&pentry);
		if (ret < 0)
			exit_with_log(" ERROR GETTING NEXT LIBSTUB FROM CONFIG ", &ret, sizeof(ret));

		NID_LOGSTR("-->CURRENT MODULE LIBSTUB: 0x%08X\n", (int)pentry);

		num += add_stubs_nid_to_table(pentry, &lib_index);

		nlib_stubs--;
	}

	cfg_close();
#endif
	CLEAR_CACHE;

#if defined(DEBUG) && !defined(DEACTIVATE_SYSCALL_ESTIMATION)
	SceUID fd;
	int fw_ver = get_fw_ver();
	int is_cfw = fw_ver <= 550 &&
		globals->lib_table.table[get_lib_index("SysMemUserForUser")].lowest_syscall
			- globals->lib_table.table[get_lib_index("InterruptManager")].lowest_syscall > 244;
	int estimated_correctly = 0;
	int i, nid_index, pos, syscall;

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

			LOGSTR("%d %s %s %d ", fw_ver,
				(int)globals->lib_table.table[lib_index].name,
				globals->lib_table.table[lib_index].highest_syscall - globals->lib_table.table[lib_index].lowest_syscall
					== (int)num_lib_exports - 1 ?
					(int)"aligned" : (int)"not aligned",
				(int)num_lib_exports);
			LOGSTR("%d ", globals->lib_table.table[lib_index].highest_syscall - globals->lib_table.table[lib_index].lowest_syscall + 1);
			LOGSTR("%d %d %d %d\n", globals->lib_table.table[lib_index].lowest_syscall - base_syscall,
				globals->lib_table.table[lib_index].highest_syscall - base_syscall,
				globals->lib_table.table[lib_index].lowest_syscall,
				globals->lib_table.table[lib_index].highest_syscall);
		}

		LOGSTR("\n");
	}

	// On CFW there is a higher syscall difference between SysmemUserForUser
	// and lower libraries than without it.
	if (is_cfw)
		puts_scr("Using offsets for Custom Firmware");


	// Fill remaining data after the reference library is known
	for (lib_index = 0; lib_index < globals->lib_table.num; lib_index++) {
		LOGSTR("Completing library...%d\n", lib_index);
		complete_library(&(globals->lib_table.table[lib_index]), ref_lib_index, is_cfw);
	}

	CLEAR_CACHE;

	LOGSTR("\n==LIBRARY TABLE DUMP==\n");
	for (lib_index = 0; lib_index < globals->lib_table.num; lib_index++) {
		LOGSTR("->Index: %d, name %s\n", lib_index, (u32)globals->lib_table.table[lib_index].name);
		LOGSTR("predicted syscall range from 0x%08X to 0x%08X\n",
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

				LOGSTR("[%d] 0x%08X 0x%08X ", nid_index, globals->nid_table.table[nid_index].nid, syscall);

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
					LOGSTR("0x%08X %d %s\n", globals->lib_table.table[lib_index].lowest_syscall + pos, lib_index,
						globals->lib_table.table[lib_index].lowest_syscall + pos == syscall ?
							(int)"YES" : (int)"NO";
				} else
					LOGSTR("\n");
			}
		}

		sceIoClose(fd);

		LOGSTR("\n\n");
	}
#endif

#ifndef XMB_LAUNCHER
	globals->nid_table.num = num;
#endif

	return num;
}

// Entry point
void _start() __attribute__ ((section (".text.start")));
void _start()
{
	SceUID hbl_file;
	int num_nids;

	cls();
	puts_scr("Starting HBL R"SVNVERSION" http://code.google.com/p/valentine-hbl");
#ifdef DEBUG
#ifdef NID_DEBUG
	puts_scr("DEBUG version (+NIDS)");
#else
	puts_scr("DEBUG version");
#endif
#else
	puts_scr_color("DO NOT POST LOG FILES OR BUG REPORTS FOR THIS VERSION!!!", 0x000000FF);
#endif

#ifdef PRE_LOADER_EXEC
	preLoader_Exec();
#endif

#ifdef RESET_HOME_LANG
	// Reset language and button assignment for the HOME screen to system defaults
	resetHomeSettings();
#endif

#if defined(FORCE_FIRST_LOG) || defined(DEBUG)
	// Some exploits need to "trigger" file I/O in order for HBL to work, not sure why
	logstr("Loader running\n");
#endif

#ifdef FORCE_CLOSE_FILES_IN_LOADER
	CloseFiles();
#endif

	if ((hbl_file = sceIoOpen(HBL_PATH, PSP_O_RDONLY, 0777)) < 0)
		exit_with_log(" FAILED TO LOAD HBL ", &hbl_file, sizeof(hbl_file));

	LOGSTR("Loading HBL\n");
	load_hbl(hbl_file);

	//reset the contents of the debug file;
	init_debug();

	//init global variables
	init_globals();

#ifndef VITA
	int fw_ver = get_fw_ver();
	switch (fw_ver)
	{
		case 0:
		case 1:
			puts_scr("Unknown Firmware :(");
			break;
		default:
			prtstr("Firmware %d.%dx detected", fw_ver / 100,  (fw_ver % 100) / 10);
			break;
	}

	if (getPSPModel() == PSP_GO) {
		puts_scr("PSP Go Detected");
#ifndef DISABLE_KERNEL_DUMP
		// If PSPGo on 6.20+, do a kmem dump
		if (get_fw_ver() >= 620)
			get_kmem_dump();
#endif
	}
#endif

	// Build NID table
	puts_scr("Building NIDs table");
	num_nids = build_nid_table();
	LOGSTR("NUM NIDS: %d\n", num_nids);

	if(num_nids <= 0)
		exit_with_log("No Nids ???", NULL, 0);

	LOGSTR("Resolving HBL stubs\n");
	resolve_stubs();
	LOGSTR("HBL stubs copied, running eLoader\n");
	run_eloader();
}

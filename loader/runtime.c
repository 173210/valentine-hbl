#include <common/stubs/syscall.h>
#include <common/utils/cache.h>
#include <common/utils/scr.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/utils.h>
#include <loader/runtime.h>
#include <exploit_config.h>

#ifndef UTILITY_DONT_USE_sceUtilityLoadModule
static const int nonEmuModules[] = {
#ifdef AVOID_NET_UTILITY
	PSP_MODULE_NET_COMMON, PSP_MODULE_NET_ADHOC, PSP_MODULE_NET_INET,
	PSP_MODULE_NET_PARSEURI, PSP_MODULE_NET_PARSEHTTP, PSP_MODULE_NET_HTTP,
	PSP_MODULE_NET_SSL, PSP_MODULE_NET_UPNP, PSP_MODULE_NET_GAMEUPDATE,
#endif
	PSP_MODULE_IRDA
};
static const int modules[] = {
#ifndef AVOID_NET_UTILITY
	PSP_MODULE_NET_COMMON, PSP_MODULE_NET_ADHOC, PSP_MODULE_NET_INET,
	PSP_MODULE_NET_PARSEURI, PSP_MODULE_NET_PARSEHTTP, PSP_MODULE_NET_HTTP,
	PSP_MODULE_NET_SSL, PSP_MODULE_NET_UPNP, PSP_MODULE_NET_GAMEUPDATE,
#endif
	PSP_MODULE_USB_PSPCM, PSP_MODULE_USB_MIC, PSP_MODULE_USB_CAM, PSP_MODULE_USB_GPS,
	PSP_MODULE_AV_SASCORE, PSP_MODULE_AV_ATRAC3PLUS, PSP_MODULE_AV_MPEGBASE,
	PSP_MODULE_AV_VAUDIO, PSP_MODULE_AV_AAC, PSP_MODULE_AV_G729, PSP_MODULE_AV_MP4,
	PSP_MODULE_NP_COMMON, PSP_MODULE_NP_SERVICE, PSP_MODULE_NP_MATCHING2, PSP_MODULE_NP_DRM
};
	// PSP_MODULE_AV_AVCODEC <-- removed to cast syscall of sceAudiocodec and sceVideocodec
#endif

#ifdef UTILITY_AV_AVCODEC_PATH
static SceUID avcodec_modid;
#endif
#ifdef UTILITY_AV_SASCORE_PATH
static SceUID sascore_modid;
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
static SceUID atrac3plus_modid;
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
static SceUID mpegbase_modid;
#endif

#ifdef LOAD_MODULES_FOR_SYSCALLS
void load_utils()
{
	int ret;
#ifdef UTILITY_DONT_USE_sceUtilityLoadModule
	int module;

#ifdef UTILITY_USE_sceUtilityLoadNetModule
	for (module = 1; module <= 7; module++) {
		dbg_printf("%s: Loading net module %d\n", module);
		ret = sceUtilityLoadNetModule(module);
		if (ret < 0)
			dbg_printf("%s: Loading net module %d failed 0x%08X\n",
				__func__, module, ret);
	}
#endif
#ifdef UTILITY_USE_sceUtilityLoadUsbModule
	for (module = 1; module <= 5; module++) {
		dbg_printf("%s: Loading USB module %d\n", module);
		ret = sceUtilityLoadUsbModule(module);
		if (ret < 0)
			dbg_printf("%s: Loading USB module %d failed 0x%08X\n",
				__func__, module, ret);
	}
#endif
#ifdef UTILITY_USE_sceUtilityLoadAvModule
	for (module = 1; module <= 7; module++) {
		dbg_printf("%s: Loading AV module %d\n", module);
		ret = sceUtilityLoadAvModule(module);
			dbg_printf("%s: Loading AV module %d failed 0x%08X\n",
				__func__, module, ret);
	}
#else
#ifdef UTILITY_AV_AVCODEC_PATH
	dbg_puts("%s: Loading " UTILITY_AV_AVCODEC_PATH, __func__);
	avcodec_modid = sceKernelLoadModule(UTILITY_AV_AVCODEC_PATH, 0, NULL);
	sceKernelStartModule(avcodec_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_SASCORE_PATH
	dbg_puts("%s: Loading " UTILITY_AV_SASCORE_PATH, __func__);
	sascore_modid = sceKernelLoadModule(UTILITY_AV_SASCORE_PATH, 0, NULL);
	sceKernelStartModule(sascore_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
	dbg_puts("%s: Loading " UTILITY_AV_ATRAC3PLUS_PATH, __func__);
	atrac3plus_modid = sceKernelLoadModule(UTILITY_AV_ATRAC3PLUS_PATH, 0, NULL);
	sceKernelStartModule(atrac3plus_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
	dbg_puts("%s: Loading " UTILITY_AV_MPEGBASE_PATH, __func__);
	mpegbase_modid = sceKernelLoadModule(UTILITY_AV_MPEGBASE_PATH, 0, NULL);
	sceKernelStartModule(mpegbase_modid, 0, NULL, NULL, NULL);
#endif
#endif
#else
	unsigned i;

	// Load modules in order
	if (!globals->isEmu)
		for(i = 0; i < sizeof(nonEmuModules) / sizeof(int); i++) {
			dbg_printf("%s: Loading module 0x%08X",
				__func__, nonEmuModules[i]);
			ret = sceUtilityLoadModule(nonEmuModules[i]);
			if (ret < 0)
				dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
					__func__, nonEmuModules[i], ret);
		}

	for(i = 0; i < sizeof(modules) / sizeof(int); i++) {
		dbg_printf("%s: Loading module 0x%08X", __func__, modules[i]);
		ret = sceUtilityLoadModule(modules[i]);
		if (ret < 0)
			dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
				__func__, modules[i], ret);
	}

	dbg_printf("Loading module 0x%08X", PSP_MODULE_AV_MP3);
	ret = sceUtilityLoadModule(PSP_MODULE_AV_MP3);
	if (ret < 0)
		dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
			__func__, PSP_MODULE_AV_MP3, ret);
#endif
}
#endif
#ifndef DISABLE_UNLOAD_UTILITY_MODULES
void unload_utils()
{
	int ret;
#if defined(UTILITY_DONT_USE_sceUtilityLoadModule)
	int module;

#ifdef UTILITY_USE_sceUtilityLoadNetModule
	for (module = 7; module >= 1; module--) {
		dbg_printf("%s: Unloading net module %d\n", module);
		ret = sceUtilityUnloadNetModule(module);
		if (ret < 0)
			dbg_printf("%s: Unloading net module %d failed 0x%08X\n",
				__func__, module, ret);
	}
#endif
#ifdef UTILITY_USE_sceUtilityLoadUsbModule
	for (module = 5; module >= 1; module--) {
		dbg_printf("%s: Unloading USB module %d\n", module);
		ret = sceUtilityUnloadUsbModule(module);
		if (ret < 0)
			dbg_printf("%s: Unloading USB module %d failed 0x%08X\n",
				__func__, module, ret);
	}
#endif
#ifdef UTILITY_USE_sceUtilityLoadAvModule
	for (module = 7; module >= 5; module--) {
		dbg_printf("%s: Unloading AV module %d\n", module);
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			dbg_printf("%s: Unloading AV module %d failed 0x%08X\n",
				__func__, module, ret);
	}

	if (globals->module_sdk_version <= 0x06020010)
		module--; // Skip PSP_AV_MODULE_MP3

	while (module >= 1) {
		dbg_printf("%s: Unloading AV module %d\n", module);
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			dbg_printf("%s: Unloading AV module %d failed 0x%08X\n",
				__func__, module, ret);

		module--;
	}
#else
#ifdef UTILITY_AV_AVCODEC_PATH
	dbg_printf("%s: Unloading " UTILITY_AV_AVCODEC_PATH, __func__);
	sceKernelStopModule(avcodec_modid, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(avcodec_modid);
#endif
#ifdef UTILITY_AV_SASCORE_PATH
	dbg_printf("%s: Unloading " UTILITY_AV_SASCORE_PATH, __func__);
	sceKernelStopModule(sascore_modid, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(sascore_modid);
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
	dbg_printf("%s: Unloading " UTILITY_AV_ATRAC3PLUS_PATH, __func__);
	sceKernelStopModule(atrac3plus_modid, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(atrac3plus_modid);
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
	dbg_printf("%s: Unloading " UTILITY_AV_MPEGBASE_PATH, __func__);
	sceKernelStopModule(mpegbase_modid, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(mpegbase_modid);
#endif
#endif
#else
	int i;

	//Unload modules in reverse order
	if (globals->module_sdk_version > 0x06020010) {
		dbg_printf("%s: Unloading module 0x%08X",
			__func__, PSP_MODULE_AV_MP3);
		ret = sceUtilityUnloadModule(PSP_MODULE_AV_MP3);
		if (ret < 0)
			dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
				__func__, PSP_MODULE_AV_MP3, ret);
	}

	for(i = sizeof(modules) / sizeof(int) - 1; i >= 0; i--) {
		dbg_printf("%s: Unloading module 0x%08X",
			__func__, modules[i]);
		ret = sceUtilityUnloadModule(modules[i]);
		if (ret < 0)
			dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
				__func__, modules[i], ret);
	}

	if (!globals->isEmu)
		for(i = sizeof(nonEmuModules) / sizeof(int) - 1; i >= 0; i--) {
			dbg_printf("%s: Unloading module 0x%08X",
				__func__, nonEmuModules[i]);
			ret = sceUtilityUnloadModule(nonEmuModules[i]);
			if (ret < 0)
				dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
					__func__, nonEmuModules[i], ret);
		}
#endif
}
#endif

// Returns !=0 if stub entry is valid, 0 if it's not
int elf_check_stub_entry(tStubEntry *pentry)
{

	return valid_umem_pointer(pentry->lib_name) &&
		valid_umem_pointer(pentry->nid_p) &&
		valid_umem_pointer(pentry->jump_p) &&
		pentry->import_stubs && pentry->import_stubs < 256 &&
		pentry->stub_size && pentry->stub_size < 256;
}

int p2_add_stubs()
{
	tStubEntry *pentry;
	int num = 0;

	for (pentry = (tStubEntry *)0x08800000;
		(int)pentry < 0x0A000000;
		pentry = (tStubEntry *)((int)pentry + 4)) {

		// While it's a valid stub header
		while (elf_check_stub_entry(pentry)) {
			if (*(char *)pentry->lib_name &&
				(pentry->import_flags == 0x11 || !pentry->import_flags))
				num += add_stub(pentry);

			// Next entry
			pentry++;
		}
	}

	return num;
}

// Initializes the savedata dialog loop, opens p5
static void p5_open_savedata(int mode)
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
static void p5_close_savedata()
{
	int status;
	int last_status = -1;

	dbg_printf("entering savedata dialog loop\n");

	while(1) {
		status = sceUtilitySavedataGetStatus();

		if (status != last_status) {
			dbg_printf("status changed from %d to %d\n", last_status, status);
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
				dbg_printf("dialog has shut down\n");
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

static int p5_find_add_stubs(const char *modname, void *p, size_t size)
{
	int num = 0;
	tStubEntry *pentry = *(tStubEntry **)(findstr(modname, p, size) + 40);

	// While it's a valid stub header
	while (elf_check_stub_entry(pentry)) {
		if (pentry->import_flags == 0x11 || !pentry->import_flags)
			num += add_stub(pentry);

		// Next entry
		pentry++;
	}
	
	return num;
}

int p5_add_stubs()
{
	int num;

#ifndef HOOK_sceKernelVolatileMemUnlock_WITH_dummy
	sceKernelVolatileMemUnlock(0);
#endif

	p5_open_savedata(PSP_UTILITY_SAVEDATA_AUTOLOAD);

	num = p5_find_add_stubs("sceVshSDAuto_Module", (void *)0x08410000, 0x00010000);

	p5_close_savedata();

	p5_open_savedata(PSP_UTILITY_SAVEDATA_SAVE);

	num += p5_find_add_stubs("scePaf_Module", (void *)0x084C0000, 0x00010000);
	num += p5_find_add_stubs("sceVshCommonUtil_Module", (void *)0x08760000, 0x00010000);
	num += p5_find_add_stubs("sceDialogmain_Module", (void *)0x08770000, 0x00010000);

	p5_close_savedata();
	scr_init();

	return num;
}

// Autoresolves HBL stubs
int resolve_hbl_stubs(const tStubEntry *top, const tStubEntry *end)
{
	const tStubEntry *ent;
	int i, ret;
	int *stub, *nid;
	int call;
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	const char *lib_name;
#endif

	if (top == NULL || end == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	for (ent = top; ent != end; ent++) {
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
		lib_name = ent->lib_name;
#endif
		stub = (int *)((int)ent->jump_p | 0x40000000);
		nid = (int *)ent->nid_p;

		for (i = 0; i < ent->stub_size; i++) {
			dbg_printf("-Resolving import 0x%08X: 0x%08X\n",
				(int)stub, *nid);

			// Is it known by HBL?
			ret = get_nid_index(*nid);

			if (ret < 0) {
				*stub++ = JR_RA_OPCODE;
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
				dbg_printf("HBL Function missing, this can lead to trouble\n");
				*stub++ = NOP_OPCODE;
#else
				*stub++ = MAKE_SYSCALL(estimate_syscall(lib_name, *nid));
#endif
			} else {
				dbg_printf("-Found in NID table, using real call\n");
				call = globals->nid_table[ret].call;
				if (call & 0x0C000000) {
					*stub++ = call;
					*stub++ = NOP_OPCODE;
				} else {
					*stub++ = JR_RA_OPCODE;
					*stub++ = call;
				}
			}

			nid++;
		}
	}

	dbg_printf(" ****STUBS SEARCHED\n");
	end--;
	hblIcacheFillRange(top->jump_p, (void *)(*(int *)end->jump_p + end->stub_size));

	return 0;
}


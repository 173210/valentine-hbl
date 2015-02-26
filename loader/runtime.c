#include <common/stubs/syscall.h>
#include <common/utils/cache.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/utils.h>
#include <loader/runtime.h>
#include <exploit_config.h>

static const int netModules[] = {
	PSP_MODULE_NET_COMMON, PSP_MODULE_NET_ADHOC, PSP_MODULE_NET_INET,
	PSP_MODULE_NET_PARSEURI, PSP_MODULE_NET_PARSEHTTP, PSP_MODULE_NET_HTTP,
	PSP_MODULE_NET_SSL, PSP_MODULE_NET_UPNP, PSP_MODULE_NET_GAMEUPDATE
};

static const int modules[] = {
	PSP_MODULE_IRDA,
	PSP_MODULE_USB_PSPCM, PSP_MODULE_USB_MIC, PSP_MODULE_USB_CAM, PSP_MODULE_USB_GPS,
	PSP_MODULE_AV_SASCORE, PSP_MODULE_AV_ATRAC3PLUS, PSP_MODULE_AV_MPEGBASE,
	PSP_MODULE_AV_VAUDIO, PSP_MODULE_AV_AAC, PSP_MODULE_AV_G729, PSP_MODULE_AV_MP4,
	PSP_MODULE_NP_COMMON, PSP_MODULE_NP_SERVICE, PSP_MODULE_NP_MATCHING2, PSP_MODULE_NP_DRM
	// PSP_MODULE_AV_AVCODEC <-- removed to cast syscall of sceAudiocodec and sceVideocodec
};

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

static int sceNetIsImported = 0;

void load_utils()
{
	int ret;
	int module;
	unsigned i;

	if (isImported(sceUtilityLoadModule)) {
		// Load modules in order
		if (!globals->isEmu || sceNetIsImported)
			for(i = 0; i < sizeof(netModules) / sizeof(int); i++) {
				dbg_printf("%s: Loading module 0x%08X\n",
					__func__, netModules[i]);
				ret = sceUtilityLoadModule(netModules[i]);
				if (ret < 0)
					dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
						__func__, netModules[i], ret);
			}

		for(i = 0; i < sizeof(modules) / sizeof(int); i++) {
			dbg_printf("%s: Loading module 0x%08X\n", __func__, modules[i]);
			ret = sceUtilityLoadModule(modules[i]);
			if (ret < 0)
				dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
					__func__, modules[i], ret);
		}

		dbg_printf("Loading module 0x%08X\n", PSP_MODULE_AV_MP3);
		ret = sceUtilityLoadModule(PSP_MODULE_AV_MP3);
		if (ret < 0)
			dbg_printf("%s: Loading 0x%08X failed 0x%08X\n",
				__func__, PSP_MODULE_AV_MP3, ret);
	} else {
		if (isImported(sceUtilityLoadNetModule))
			for (module = 1; module <= 7; module++) {
				dbg_printf("%s: Loading net module %d\n", module);
				ret = sceUtilityLoadNetModule(module);
				if (ret < 0)
					dbg_printf("%s: Loading net module %d failed 0x%08X\n",
						__func__, module, ret);
			}

		if (isImported(sceUtilityLoadUsbModule))
			for (module = 1; module <= 5; module++) {
				dbg_printf("%s: Loading USB module %d\n", module);
				ret = sceUtilityLoadUsbModule(module);
				if (ret < 0)
					dbg_printf("%s: Loading USB module %d failed 0x%08X\n",
						__func__, module, ret);
			}

		if (isImported(sceUtilityLoadAvModule))
			for (module = 1; module <= 7; module++) {
				dbg_printf("%s: Loading AV module %d\n", module);
				ret = sceUtilityLoadAvModule(module);
				dbg_printf("%s: Loading AV module %d failed 0x%08X\n",
					__func__, module, ret);
			}
		else {
#ifdef UTILITY_AV_AVCODEC_PATH
			dbg_puts("%s: Loading " UTILITY_AV_AVCODEC_PATH "\n", __func__);
			avcodec_modid = sceKernelLoadModule(UTILITY_AV_AVCODEC_PATH, 0, NULL);
			sceKernelStartModule(avcodec_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_SASCORE_PATH
			dbg_puts("%s: Loading " UTILITY_AV_SASCORE_PATH "\n", __func__);
			sascore_modid = sceKernelLoadModule(UTILITY_AV_SASCORE_PATH, 0, NULL);
			sceKernelStartModule(sascore_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
			dbg_puts("%s: Loading " UTILITY_AV_ATRAC3PLUS_PATH "\n", __func__);
			atrac3plus_modid = sceKernelLoadModule(UTILITY_AV_ATRAC3PLUS_PATH, 0, NULL);
			sceKernelStartModule(atrac3plus_modid, 0, NULL, NULL, NULL);
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
			dbg_puts("%s: Loading " UTILITY_AV_MPEGBASE_PATH "\n", __func__);
			mpegbase_modid = sceKernelLoadModule(UTILITY_AV_MPEGBASE_PATH, 0, NULL);
			sceKernelStartModule(mpegbase_modid, 0, NULL, NULL, NULL);
#endif
		}
	}
}

void unload_utils()
{
	int module;
	int ret;
	int i;

	if (isImported(sceUtilityUnloadModule)) {
		//Unload modules in reverse order
		if (globals->module_sdk_version > 0x06020010) {
			dbg_printf("%s: Unloading module 0x%08X\n",
				__func__, PSP_MODULE_AV_MP3);
			ret = sceUtilityUnloadModule(PSP_MODULE_AV_MP3);
			if (ret < 0)
				dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
					__func__, PSP_MODULE_AV_MP3, ret);
		}

		for(i = sizeof(modules) / sizeof(int) - 1; i >= 0; i--) {
			dbg_printf("%s: Unloading module 0x%08X\n",
				__func__, modules[i]);
			ret = sceUtilityUnloadModule(modules[i]);
			if (ret < 0)
				dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
					__func__, modules[i], ret);
		}

		if (!globals->isEmu || sceNetIsImported)
			for(i = sizeof(netModules) / sizeof(int) - 1; i >= 0; i--) {
				dbg_printf("%s: Unloading module 0x%08X\n",
					__func__, netModules[i]);
				ret = sceUtilityUnloadModule(netModules[i]);
				if (ret < 0)
					dbg_printf("%s: Unloading 0x%08X failed 0x%08X\n",
						__func__, netModules[i], ret);
			}
	} else {
		if (isImported(sceUtilityUnloadNetModule))
			for (module = 7; module >= 1; module--) {
				dbg_printf("%s: Unloading net module %d\n", module);
				ret = sceUtilityUnloadNetModule(module);
				if (ret < 0)
					dbg_printf("%s: Unloading net module %d failed 0x%08X\n",
						__func__, module, ret);
			}

		if (isImported(sceUtilityUnloadUsbModule))
			for (module = 5; module >= 1; module--) {
				dbg_printf("%s: Unloading USB module %d\n", module);
				ret = sceUtilityUnloadUsbModule(module);
				if (ret < 0)
					dbg_printf("%s: Unloading USB module %d failed 0x%08X\n",
						__func__, module, ret);
			}

		if (isImported(sceUtilityUnloadAvModule)) {
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
		} else {
#ifdef UTILITY_AV_AVCODEC_PATH
			dbg_printf("%s: Unloading " UTILITY_AV_AVCODEC_PATH "\n", __func__);
			sceKernelStopModule(avcodec_modid, 0, NULL, NULL, NULL);
			sceKernelUnloadModule(avcodec_modid);
#endif
#ifdef UTILITY_AV_SASCORE_PATH
			dbg_printf("%s: Unloading " UTILITY_AV_SASCORE_PATH "\n", __func__);
			sceKernelStopModule(sascore_modid, 0, NULL, NULL, NULL);
			sceKernelUnloadModule(sascore_modid);
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
			dbg_printf("%s: Unloading " UTILITY_AV_ATRAC3PLUS_PATH "\n", __func__);
			sceKernelStopModule(atrac3plus_modid, 0, NULL, NULL, NULL);
			sceKernelUnloadModule(atrac3plus_modid);
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
			dbg_printf("%s: Unloading " UTILITY_AV_MPEGBASE_PATH "\n", __func__);
			sceKernelStopModule(mpegbase_modid, 0, NULL, NULL, NULL);
			sceKernelUnloadModule(mpegbase_modid);
#endif
		}
	}
}

// Returns !=0 if stub entry is valid, 0 if it's not
int elf_check_stub_entry(const tStubEntry *pentry)
{

	return valid_umem_pointer(pentry->lib_name) &&
		valid_umem_pointer(pentry->nid_p) &&
		valid_umem_pointer(pentry->jump_p) &&
		pentry->import_stubs && pentry->import_stubs < 256 &&
		pentry->stub_size && pentry->stub_size < 256;
}

int p2_add_stubs()
{
	const tStubEntry *pentry;
	int num = 0;

	for (pentry = (tStubEntry *)0x08800000;
		pentry != libStubTop;
		pentry = (tStubEntry *)((int)pentry + 4)) {

		while (elf_check_stub_entry(pentry)) {
			if (*(char *)pentry->lib_name &&
				(pentry->import_flags == 0x11 || !pentry->import_flags)) {
				if (strcmp("sceNet", pentry->lib_name))
					num += add_stub(pentry);
				else
					sceNetIsImported = 1;
			}

			pentry++;
		}
	}

	for (pentry = libStubBtm;
		(int)pentry < 0x0A000000;
		pentry = (tStubEntry *)((int)pentry + 4)) {

		while (elf_check_stub_entry(pentry)) {
			if (*(char *)pentry->lib_name &&
				(pentry->import_flags == 0x11 || !pentry->import_flags)) {
				if (strcmp("sceNet", pentry->lib_name))
					num += add_stub(pentry);
				else
					sceNetIsImported = 1;
			}

			pentry++;
		}
	}

	return num;
}

// Initializes the savedata dialog loop, opens p5
static void p5_open_savedata(int mode)
{
	void (* f)(SceUInt delay);

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
	if (isImported(sceDisplayWaitVblankStart))
		f = (void *)sceDisplayWaitVblankStart;
	else if (isImported(sceDisplayWaitVblankStartCB))
		f = (void *)sceDisplayWaitVblankStartCB;
	else if (isImported(sceKernelDelayThread))
		f = (void *)sceKernelDelayThread;
	else
		return;

	while (sceUtilitySavedataGetStatus() < 2)
		f(256);
}

// Runs the savedata dialog loop
static void p5_close_savedata()
{
	void (* f)(SceUInt delay);
	int status;
	int last_status = -1;

	if (isImported(sceDisplayWaitVblankStart))
		f = (void *)sceDisplayWaitVblankStart;
	else if (isImported(sceDisplayWaitVblankStartCB))
		f = (void *)sceDisplayWaitVblankStartCB;
	else if (isImported(sceKernelDelayThread))
		f = (void *)sceKernelDelayThread;
	else
		return;

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

		f(256);
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

	if (isImported(sceKernelVolatileMemUnlock))
		sceKernelVolatileMemUnlock(0);

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

	if (top == NULL || end == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	for (ent = top; ent != end; ent++) {
		stub = (int *)((int)ent->jump_p | 0x40000000);
		nid = (int *)ent->nid_p;

		for (i = 0; i < ent->stub_size; i++) {
			dbg_printf("-Resolving import 0x%08X: 0x%08X\n",
				(int)stub, *nid);

			// Is it known by HBL?
			ret = get_nid_index(*nid);

			if (ret < 0) {
				dbg_printf("HBL Function missing, this can lead to trouble\n");
				*stub++ = JR_ASM(REG_RA);
				*stub++ = globals->isEmu ?
					NOP_ASM :
					estimate_syscall(ent->lib_name, *nid);
			} else {
				dbg_printf("-Found in NID table, using real call\n");
				call = globals->nid_table[ret].call;
				if (call & 0x0C000000) {
					*stub++ = call;
					*stub++ = NOP_ASM;
				} else {
					*stub++ = JR_ASM(REG_RA);
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


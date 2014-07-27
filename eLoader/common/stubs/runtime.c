#include <common/stubs/runtime.h>
#include <common/stubs/syscall.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/utils.h>
#include <exploit_config.h>
#include <loader.h>

#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
static int modules[] = {
#if VITA < 310 || !defined(AVOID_NET_UTILITY)
	PSP_MODULE_NET_COMMON, PSP_MODULE_NET_ADHOC, PSP_MODULE_NET_INET,
	PSP_MODULE_NET_PARSEURI, PSP_MODULE_NET_PARSEHTTP, PSP_MODULE_NET_HTTP,
	PSP_MODULE_NET_SSL, PSP_MODULE_NET_UPNP, PSP_MODULE_NET_GAMEUPDATE,
#endif
#ifndef VITA
	PSP_MODULE_IRDA,
#endif
	PSP_MODULE_USB_PSPCM, PSP_MODULE_USB_MIC, PSP_MODULE_USB_CAM, PSP_MODULE_USB_GPS,
	PSP_MODULE_AV_SASCORE, PSP_MODULE_AV_ATRAC3PLUS, PSP_MODULE_AV_MPEGBASE, PSP_MODULE_AV_MP3,
	PSP_MODULE_AV_VAUDIO, PSP_MODULE_AV_AAC, PSP_MODULE_AV_G729, PSP_MODULE_AV_MP4,
	PSP_MODULE_NP_COMMON, PSP_MODULE_NP_SERVICE, PSP_MODULE_NP_MATCHING2, PSP_MODULE_NP_DRM
};
	// PSP_MODULE_AV_AVCODEC <-- removed to cast syscall of sceAudiocodec and sceVideocodec
#endif

void load_utils()
{
	int ret;
#ifdef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
	int module;

#ifdef USE_NET_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 7; module++) {
		ret = sceUtilityLoadNetModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Loading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 5; module++) {
		ret = sceUtilityLoadUsbModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Loading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 7; module++) {
		ret = sceUtilityLoadAvModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Loading av module 0x%08X\n", ret, module);
	}
#endif
#else
	unsigned i;

	// Load modules in order
	for(i = 0; i < sizeof(modules) / sizeof(int); i++) {
		ret = sceUtilityLoadModule(modules[i]);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Loading 0x%08X\n", ret, modules[i]);
	}
#endif
}

void unload_utils()
{
	int ret;
#ifdef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
	int module;

#ifdef USE_NET_MODULE_LOAD_FUNCTION
	for (module = 7; module >= 1; module--) {
		ret = sceUtilityUnloadNetModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Unloading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 5; module >= 1; module--) {
		ret = sceUtilityUnloadUsbModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Unloading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 7; module >= 5; module--) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);
	}
#ifndef VITA
	if (get_fw_ver() <= 620)
		module--; // Skip PSP_AV_MODULE_MP3
#endif
	while (module >= 1) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);

		module--;
	}
#endif
#else
	int i;

	//Unload modules in reverse order
	for(i = sizeof(modules) / sizeof(int) - 1; i >= 0; i--) {
#ifndef VITA
		if(modules[i] == PSP_MODULE_AV_MP3 && get_fw_ver() <= 620)
			continue;
#endif
		ret = sceUtilityUnloadModule(modules[i]);
		if (ret < 0)
			LOG_PRINTF("...Error 0x%08X Unloading 0x%08X\n", ret, modules[i]);
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
				num += add_stub_to_table(pentry);

			// Next entry
			pentry++;
		}
	}

	return num;
}

// Autoresolves HBL stubs
void resolve_stubs()
{
	int index, ret;
	int num_nids;
	int *cur_stub = (int *)HBL_STUBS_START;
	int nid = 0, syscall = 0;
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	int num_libs, count;
	tImportedLib cur_lib;
#endif

	ret = cfg_init();

	if (ret < 0) {
		log_printf(" ERROR INITIALIZING CONFIG 0x%08X", ret);
		sceKernelExitGame();
	}

	num_nids = cfg_num_nids();
	cfg_first_nid(&nid);

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	num_libs = cfg_num_libs();
	cfg_first_lib(&cur_lib);
	count = cur_lib.num_imports;
#endif

	for (index = 0; index < num_nids; index++) {
		LOG_PRINTF("-Resolving import 0x%08X: 0x%08X\n", index * 8, nid);

		// Is it known by HBL?
		ret = get_nid_index(nid);

		// If it's known, get the call
		if (ret > 0) {
			LOG_PRINTF("-Found in NID table, using real call\n");
			syscall = globals->nid_table.table[ret].call;
		} else {
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
			LOG_PRINTF("HBL Function missing at 0x%08X, this can lead to trouble\n",  (int)cur_stub);
			syscall = NOP_OPCODE;
#else
			// If not, estimate
			while (index > count) {
				cfg_next_lib(&cur_lib);
				count += cur_lib.num_imports;
			}

			syscall = estimate_syscall(lib_name, nid, globals->syscalls_known ? FROM_LOWEST : FROM_CLOSEST);
#endif
		}

		*cur_stub++ = JR_RA_OPCODE;
		*cur_stub++ = syscall;

		// NID & library for next import
		cfg_next_nid(&nid);
	}

	CLEAR_CACHE;

	cfg_close();

	LOG_PRINTF(" ****STUBS SEARCHED\n");
}


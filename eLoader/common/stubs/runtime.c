#include <common/stubs/runtime.h>
#include <common/stubs/syscall.h>
#include <common/utils/scr.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/utils.h>
#include <exploit_config.h>
#include <loader.h>

#ifndef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
static const int modules[] = {
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

#ifdef LOAD_MODULES_FOR_SYSCALLS
void load_utils()
{
	int ret;
#ifdef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
	int module;

#ifdef USE_NET_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 7; module++) {
		ret = sceUtilityLoadNetModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Loading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 5; module++) {
		ret = sceUtilityLoadUsbModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Loading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 7; module++) {
		ret = sceUtilityLoadAvModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Loading av module 0x%08X\n", ret, module);
	}
#endif
#else
	unsigned i;

	// Load modules in order
	for(i = 0; i < sizeof(modules) / sizeof(int); i++) {
		ret = sceUtilityLoadModule(modules[i]);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Loading 0x%08X\n", ret, modules[i]);
	}
#endif
}
#endif
#ifndef DISABLE_UNLOAD_UTILITY_MODULES
void unload_utils()
{
	int ret;
#if defined(USE_EACH_UTILITY_MODULE_LOAD_FUNCTION)
	int module;

#ifdef USE_NET_MODULE_LOAD_FUNCTION
	for (module = 7; module >= 1; module--) {
		ret = sceUtilityUnloadNetModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Unloading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 5; module >= 1; module--) {
		ret = sceUtilityUnloadUsbModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Unloading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 7; module >= 5; module--) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);
	}
#ifndef VITA
	if (globals->module_sdk_version <= 0x06020010)
		module--; // Skip PSP_AV_MODULE_MP3
#endif
	while (module >= 1) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);

		module--;
	}
#endif
#else
	int i;

	//Unload modules in reverse order
	for(i = sizeof(modules) / sizeof(int) - 1; i >= 0; i--) {
#ifndef VITA
		if(modules[i] == PSP_MODULE_AV_MP3 && globals->module_sdk_version <= 0x06020010)
			continue;
#endif
		ret = sceUtilityUnloadModule(modules[i]);
		if (ret < 0)
			dbg_printf("...Error 0x%08X Unloading 0x%08X\n", ret, modules[i]);
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

// Autoresolves HBL stubs
void resolve_hbl_stubs()
{
	unsigned u;
	int i, ret;
	int *stub, *nid;
	int syscall;
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	const char *lib_name;
#endif

	for (u = 0; u < sizeof(hbl_stub_entires) / sizeof(tStubEntry); u++) {
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
		lib_name = hbl_stub_entires[u].lib_name;
#endif
		stub = (int *)((int)hbl_stub_entires[u].jump_p | 0x40000000);
		nid = (int *)hbl_stub_entires[u].nid_p;

		for (i = 0; i < hbl_stub_entires[u].stub_size; i++) {
			dbg_printf("-Resolving import 0x%08X: 0x%08X\n",
				(int)stub, *nid);

			// Is it known by HBL?
			ret = get_nid_index(*nid);

			if (ret < 0) {
#ifdef DEACTIVATE_SYSCALL_ESTIMATION
				dbg_printf("HBL Function missing, this can lead to trouble\n");
				syscall = NOP_OPCODE;
#else
				syscall = MAKE_SYSCALL(estimate_syscall(
					lib_name, *nid, globals->syscalls_known ? FROM_LOWEST : FROM_CLOSEST));
#endif
			} else {
				dbg_printf("-Found in NID table, using real call\n");
				syscall = globals->nid_table[ret].call;
			}

			*stub++ = JR_RA_OPCODE;
			*stub++ = syscall;
			nid++;
		}
	}

	dbg_printf(" ****STUBS SEARCHED\n");
}


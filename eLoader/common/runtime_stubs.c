#include <common/sdk.h>
#include <common/debug.h>
#include <common/utils.h>
#include <common/runtime_stubs.h>
#include <exploit_config.h>

int load_util(int module)
{
	LOGSTR("Loading 0x%08X\n", module);

#ifdef USE_EACH_UTILITY_MODULE_LOAD_FUNCTION
#ifdef USE_NET_MODULE_LOAD_FUNCTION
	if (module <= PSP_MODULE_NET_SSL)
		return sceUtilityLoadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);
	else
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	if (module == PSP_MODULE_USB_PSPCM)
		return sceUtilityLoadUsbModule(PSP_USB_MODULE_PSPCM);
	else if (module <= PSP_MODULE_USB_GPS)
		return sceUtilityLoadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);
	else
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	if (module <= PSP_MODULE_AV_G729)
		return sceUtilityLoadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);
	else
#endif
	return SCE_KERNEL_ERROR_ERROR;
#else
	return sceUtilityLoadModule(module);
#endif
}

int unload_util(int module)
{
	LOGSTR("Unloading 0x%08X\n", module);

#ifdef USE_EACH_UTILITY_MODULE_UNLOAD_FUNCTION
#ifdef USE_NET_MODULE_UNLOAD_FUNCTION
	if (module <= PSP_MODULE_NET_SSL)
		return sceUtilityUnloadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);
	else
#endif
#ifdef USE_USB_MODULE_UNLOAD_FUNCTION
	if (module == PSP_MODULE_USB_PSPCM)
		return sceUtilityUnloadUsbModule(PSP_USB_MODULE_PSPCM);
	else if (module <= PSP_MODULE_USB_GPS)
		return sceUtilityUnloadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);
	else
#endif
#ifdef USE_AV_MODULE_UNLOAD_FUNCTION
	if (module <= PSP_MODULE_AV_G729)
		return sceUtilityUnloadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);
	else
#endif
	return SCE_KERNEL_ERROR_ERROR;
#else
		return sceUtilityUnloadModule(module);
#endif
}


#if defined(LOAD_MODULES_FOR_SYSCALLS) && !defined(USE_EACH_UTILITY_MODULE_LOAD_FUNCTION)
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
			LOGSTR("...Error 0x%08X Loading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 5; module++) {
		ret = sceUtilityLoadUsbModule(module);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Loading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 1; module <= 7; module++) {
		ret = sceUtilityLoadAvModule(module);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Loading av module 0x%08X\n", ret, module);
	}
#endif
#else
	unsigned i;

	// Load modules in order
	for(i = 0; i < sizeof(modules) / sizeof(int); i++) {
		ret = sceUtilityLoadModule(modules[i]);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Loading 0x%08X\n", ret, modules[i]);
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
			LOGSTR("...Error 0x%08X Unloading net module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_USB_MODULE_LOAD_FUNCTION
	for (module = 5; module >= 1; module--) {
		ret = sceUtilityUnloadUsbModule(module);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Unloading usb module 0x%08X\n", ret, module);
	}
#endif
#ifdef USE_AV_MODULE_LOAD_FUNCTION
	for (module = 7; module >= 5; module--) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);
	}
#ifndef VITA
	if (get_fw_ver() <= 620)
		module--; // Skip PSP_AV_MODULE_MP3
#endif
	while (module >= 1) {
		ret = sceUtilityUnloadAvModule(module);
		if (ret < 0)
			LOGSTR("...Error 0x%08X Unloading av module 0x%08X\n", ret, module);

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
			LOGSTR("...Error 0x%08X Unloading 0x%08X\n", ret, modules[i]);
	}
#endif
}
#endif

#ifdef AUTO_SEARCH_STUBS
int search_stubs(tStubEntry **stub_pointers)
{
	tStubEntry *pentry;
	int end = 0x0A000000;
	int cur = 0;

	for (pentry = (tStubEntry *)0x08800000;
		(int)pentry < end;
		pentry = (tStubEntry *)((int)pentry + 4)) {

		if (elf_check_stub_entry(pentry) &&
			pentry->import_stubs && pentry->import_stubs < 100 &&
			pentry->stub_size && pentry->stub_size < 100 &&
			(pentry->import_flags == 0x11 || pentry->import_flags == 0)) {

			//boundaries check
			if (cur >= MAX_RUNTIME_STUB_HEADERS) {
				LOGSTR("More stubs than Maximum allowed number, won't enumerate all stubs\n");
				return cur;
			}
			stub_pointers[cur] = pentry;
			cur++;
			LOGSTR("Found Stubs at 0x%08X\n", (int)pentry);

			// We found a valid pentry, skip consecutive valid pentry objects
			while ((int)pentry < end && elf_check_stub_entry(pentry))
				pentry++;
		}
	}

	return cur;
}
#endif

#include <common/sdk.h>
#include <common/debug.h>
#include <common/utils.h>
#include <common/runtime_stubs.h>
#include <exploit_config.h>

// If we want to load additional modules in advance to use their syscalls
#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef AUTO_SEARCH_STUBS
#define AUTO_SEARCH_STUBS
#endif

int load_utility_module(int module) {
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

int unload_utility_module(int module) {
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

void load_utility_modules(unsigned int moduleIDs[]) {
    int i, result;

	// Load modules in order
	for(i = 0; (u32)i < sizeof(moduleIDs)/sizeof(u32); i++)
	{
		unsigned int modid = moduleIDs[i];
		result = load_utility_module(modid);
		if (result < 0)
		{
			LOGSTR("...Error 0x%08X Loading 0x%08X\n", (u32)result, (u32)(modid) );
		}
	}
}
#endif

void unload_utility_modules(unsigned int moduleIDs[]) {
	int i, result;
	//Unload modules in reverse order
	for(i = sizeof(moduleIDs)/sizeof(u32) - 1 ; i >= 0; i--)
	{
		unsigned int modid = moduleIDs[i];
#ifndef VITA
		if( !(modid == PSP_MODULE_AV_MP3 && get_fw_ver() <= 620) )
#endif
		{
			result = unload_utility_module(modid);
			if (result < 0)
			{
				LOGSTR("...Error 0x%08X Unloading 0x%08X\n", (u32)result, (u32)(modid) );
			}
		}
	}
}

#ifdef AUTO_SEARCH_STUBS
int strong_check_stub_entry(tStubEntry* pentry)
{
	return (
    elf_check_stub_entry(pentry) &&
    (pentry->import_stubs && (pentry->import_stubs < 100)) &&
    (pentry->stub_size && (pentry->stub_size < 100)) &&
    ((pentry->import_flags == 0x11) || (pentry->import_flags == 0))
    );
}

int search_stubs(tStubEntry **stub_pointers) {
	tStubEntry *pentry;
	int end = 0x0A000000;
	int cur = 0;

	for (pentry = (tStubEntry *)0x08800000;
		(int)pentry < end;
		pentry = (tStubEntry *)((int)pentry + 4)) {

		if (strong_check_stub_entry(pentry)) {
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

void load_modules_for_stubs() {
#ifdef MODULES
 	unsigned int moduleIDs[] = MODULES;
	load_utility_modules(moduleIDs);
#endif
}

void unload_modules_for_stubs() {
#ifdef MODULES
    unsigned int moduleIDs[] = MODULES;
    unload_utility_modules(moduleIDs);
#endif
}

#else
// Dummy functions
int search_stubs(u32 * stub_addresses) {
    return stub_addresses ? -1 : -2;
}
void load_modules_for_stubs() {}
void unload_modules_for_stubs() {}
#endif
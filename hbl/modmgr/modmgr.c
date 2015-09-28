#include <common/stubs/syscall.h>
#include <common/utils/cache.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/path.h>
#include <common/prx.h>
#include <common/sdk.h>
#include <hbl/modmgr/elf.h>
#include <hbl/modmgr/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/stubs/resolve.h>
#include <hbl/settings.h>
#include <config.h>

static UtilModInfo av_mpegbase = {
	PSP_MODULE_AV_MPEGBASE,
	"sceMpeg_library"
};
static UtilModInfo av_atrac3plus = {
	PSP_MODULE_AV_ATRAC3PLUS,
	"sceATRAC3plus_Library"
};
static UtilModInfo av_mp3 = {
	PSP_MODULE_AV_MP3,
	"sceMp3_Library"
};
static UtilModInfo net_inet = {
	PSP_MODULE_NET_INET,
	"sceNetInet_Library"
};
static UtilModInfo net_apctl = {
	PSP_MODULE_NET_INET,
	"sceNetApctl_Library"
};
static UtilModInfo net_resolver = {
	PSP_MODULE_NET_INET,
	"sceNetResolver_Library"
};
static UtilModInfo net_common = {
	PSP_MODULE_NET_COMMON,
	"sceNet_Library"
};
static UtilModInfo net_adhoc = {
	PSP_MODULE_NET_ADHOC,
	"sceNetAdhoc_Library"
};
static UtilModInfo net_adhocctl = {
	PSP_MODULE_NET_ADHOC,
	"sceNetAdhocctl_Library"
};
static UtilModInfo net_adhoc_matching = {
	PSP_MODULE_NET_ADHOC,
	"sceNetAdhocMatching_Library"
};
static UtilModInfo net_adhoc_download = {
	PSP_MODULE_NET_ADHOC,
	"sceNetAdhocDownload_Library"
};
static UtilModInfo net_adhoc_discover = {
	PSP_MODULE_NET_ADHOC,
	"sceNetAdhocDiscover_Library"
};
static UtilModInfo net_http = {
	PSP_MODULE_NET_HTTP,
	"SceHttp_Library"
};
/*
static UtilModInfo net_ssl = {
	PSP_MODULE_NET_SSL,
	"sceSsl_Module"
};
*/

/******************************************************************************/
/* Menu API definition By noobz. We'll pass the address of this struct to the */
/* menu, and it can use it if it understands it.                              */
/*                                                                            */
/* We guarantee that any future revisions to the struct will increment        */
/* the version number, and just add new fields to the end.  Existing fields   */
/* must not be changed, for backwards-compatibility.                          */
/******************************************************************************/
typedef struct
{
	long api_ver;
	char credits[32];
	char ver_name[32];
	char *bg_fname; // set to NULL to let menu choose.
	char *fname; // The menu will write the selected filename there
}	tMenuApi;

static tMenuApi menu_api = {
	.api_ver = 1,
	.credits = "m0skit0,ab5000,wololo,davee,jjs",
	.ver_name = "HBL " VER_STR,
	.bg_fname = NULL
};

static int mod_loaded_num = 0;			// Loaded modules
static HBLModInfo mod_table[MAX_MODULES];	// List of loaded modules info struct
#ifndef DISABLE_UNLOAD_UTILITY_MODULES
static int mod_utils_num = 0;			// Loaded utility modules
static int mod_utils[MAX_MODULES]; 		// List of ID for utility modules loaded
#endif

#ifdef DEBUG
static void log_mod_entry(HBLModInfo modinfo)
{
	dbg_printf("\n->Module entry:\n"
		"ID: 0x%08X\n"
		"State: %d\n"
		"Entry point: 0x%08X\n"
		"GP: 0x%08X\n",
		modinfo.id,
		modinfo.state,
		(int)modinfo.text_entry,
		(int)modinfo.gp);
}
#endif

static void *modmgrMalloc(const char *name, SceSize size, void *p)
{
	SceUID blockid;

	dbg_printf("%s: size: %d, p: 0x%08X\n", __func__, size, (int)p);

	if (name == NULL)
		return NULL;

	if (p == NULL) {
		blockid = _hook_sceKernelAllocPartitionMemory(2, name,
			PSP_SMEM_Low, size + (1 << 16), NULL);
		if (blockid < 0) {
			dbg_printf("FAILED: 0x%08X\n", blockid);
			return NULL;
		}

		p = sceKernelGetBlockHeadAddr(blockid);
		if ((int)p & ((1 << 16) - 1))
			p = (void *)(((int)p & ~((1 << 16) - 1)) + (1 << 16));
	} else {
		blockid = _hook_sceKernelAllocPartitionMemory(2, name,
			PSP_SMEM_Addr, size, p);
		if (blockid < 0) {
			dbg_printf("FAILED: 0x%08X\n", blockid);
			return NULL;
		}
	}

	return p;
}

// Loads a module to memory
SceUID load_module(SceUID fd, const char *path, void *addr, SceOff off)
{
	_sceModuleInfo modinfo;
	Elf32_Ehdr ehdr;
	Elf32_Phdr *phdrs;
	tStubEntry *stubs;
	SceUID phdrs_block;
	SceUID modid = mod_loaded_num;
	size_t phdrs_size, mod_size, stubs_size;
	int i, ret;

	dbg_printf("\n\n->Entering load_module...\n");

	for (i = 0; path[i]; i++) {
		if (i >= sizeof(mod_table[modid].path) - 1)
			return SCE_KERNEL_ERROR_ILLEGAL_ARGUMENT;

		mod_table[modid].path[i] = path[i];
	}
	mod_table[modid].path[i] = '\0';

	//dbg_printf("mod_table address: 0x%08X\n", mod_table);

	// Read ELF header
	sceIoLseek(fd, off, PSP_SEEK_SET);
	sceIoRead(fd, &ehdr, sizeof(ehdr));

	// Check for module encryption
	if (!strncmp(ehdr.e_ident, "~PSP", 4))
		return SCE_KERNEL_ERROR_UNSUPPORTED_PRX_TYPE;

	dbg_printf("\n->ELF header:\n"
		"Type: 0x%08X\n"
		"Code entry: 0x%08X\n"
		"Program header table offset: 0x%08X\n"
		"Program header size: 0x%08X\n"
		"Number of program headers: 0x%08X\n"
		"Section header table offset: 0x%08X\n"
		"Section header size: 0x%08X\n"
		"Number of section headers: 0x%08X\n",
		ehdr.e_type,
		(int)ehdr.e_entry,
		ehdr.e_phoff,
		ehdr.e_phentsize,
		ehdr.e_phnum,
		ehdr.e_shoff,
		ehdr.e_shentsize,
		ehdr.e_shnum);

	phdrs_size = ehdr.e_phentsize * ehdr.e_phnum;

	ret = sceKernelAllocPartitionMemory(
		2, "HBL Module Program Headers", PSP_SMEM_High, phdrs_size, NULL);
	if (ret < 0)
		goto fail;
	else
		phdrs_block = ret;

	phdrs = sceKernelGetBlockHeadAddr(phdrs_block);
	if (phdrs == NULL) {
		ret = SCE_KERNEL_ERROR_ERROR;
		goto fail;
	}

	ret = sceIoLseek(fd, off + ehdr.e_phoff, PSP_SEEK_SET);
	if (ret < 0)
		goto fail;

	ret = sceIoRead(fd, phdrs, phdrs_size);
	if (ret < 0)
		goto fail;

	switch (ehdr.e_type) {
		case ELF_STATIC:
			if (modid > 0) {
				ret = SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;
				goto fail;
			}

			// Load ELF program section into memory
			ret = elf_load(fd, off, phdrs, ehdr.e_phnum, modmgrMalloc);
			if (ret < 0)
				goto fail;
			else
				mod_size = ret;

			// Locate ELF's .lib.stubs section
			stubs = elf_find_imports(fd, off, &ehdr, &stubs_size);
			if (stubs == NULL) {
				ret = SCE_KERNEL_ERROR_ERROR;
				goto fail;
			}

			mod_table[modid].text_entry = (u32 *)ehdr.e_entry;
			ret = elf_get_gp(fd, off, &ehdr, &mod_table[modid].gp);
			if (ret < 0)
				goto fail;

			break;

		case ELF_RELOC:
			if (modid >= MAX_MODULES) {
				ret = SCE_KERNEL_ERROR_EXCLUSIVE_LOAD;
				goto fail;
			}

			dbg_printf("load_module -> Offset: 0x%08X\n", off);

			// Load PRX program section
			ret = prx_load(fd, off, &ehdr, phdrs,
				&modinfo, &addr, modmgrMalloc);
			if (ret < 0)
				goto fail;
			else
				mod_size = ret;

			stubs = (void *)((int)modinfo.stub_top + (int)addr);
			stubs_size = (int)modinfo.stub_end - (int)modinfo.stub_top;

			// Relocate ELF entry point and GP register
			mod_table[modid].text_entry = (u32 *)((u32)ehdr.e_entry + (int)addr);
			mod_table[modid].gp = (void *)((int)modinfo.gp_value + (int)addr);

			break;

		default:
			dbg_printf("Uknown ELF type: 0x%08X\n", ehdr.e_type);
			ret = SCE_KERNEL_ERROR_UNSUPPORTED_PRX_TYPE;
			goto fail;
	}

	dbg_printf("resolve stubs\n");
	// Resolve ELF's stubs with game's stubs and syscall estimation
	ret = resolve_imports(stubs, stubs_size);
	if (ret)
		dbg_printf("failed to resolve imports: 0x%08X\n", ret);

	mod_table[modid].state = LOADED;

	mod_loaded_num++;
	//dbg_printf("Module table updated\n");

	dbg_printf("\n->Actual number of loaded modules: %d\n", mod_loaded_num);
	dbg_printf("Last loaded module [%d]:\n", modid);
#ifdef DEBUG
	log_mod_entry(mod_table[modid]);
#endif

	synci(addr, addr + mod_size);

	ret = modid;
fail:
	sceKernelFreePartitionMemory(phdrs_block);
	return ret;
}

/*
// Thread that launches a module
void launch_module(int modid, void* dummy)
{
	void (*entry_point)(SceSize argc, void* argp) = mod_table[modid].text_entry;
	entry_point(strlen(mod_table[modid].path) + 1, mod_table[modid].path);
	sceKernelExitDeleteThread(0);
}
*/

// Starts an already loaded module
// This will overwrite gp register
SceUID start_module(SceUID modid)
{
	SceUID thid;
	int i;

	SceSize arglen;
	char argbuf[sizeof(mod_table[modid].path) + 9];
	char *argp;

	dbg_printf("\n\n-->Starting module ID: 0x%08X\n", modid);

	
	if (modid < 0 || modid >= mod_loaded_num)
		return SCE_KERNEL_ERROR_UNKNOWN_MODULE;

	if (mod_table[modid].state == RUNNING)
		return SCE_KERNEL_ERROR_ALREADY_STARTED;

#ifdef DEBUG
	log_mod_entry(mod_table[modid]);
#endif

	__asm__("lw $gp, %0" :: "m" (mod_table[modid].gp));

	// Attempt at launching the module without thread creation (crashes on sceSystemMemoryManager ?)
	/*
	void (*launch_module)(int argc, char* argv) = mod_table[modid].text_entry;
	//launch_module(mod_table[modid].path + 1, mod_table[modid].path);
	launch_module(strlen(mod_table[modid].path) + 1, mod_table[modid].path);
	*/

	//The hook is called here to handle thread moniotoring
	thid = _hook_sceKernelCreateThread("hblmodule", mod_table[modid].text_entry, 0x30, 0x1000, 0xF0000000, NULL);
	if (thid < 0) {
		dbg_printf(" HB Thread couldn't be created. Error 0x%08X\n", thid);
		return thid;
	}

	arglen = strlen(mod_table[modid].path) + 1;
	if (strcmp(mod_table[modid].path, EBOOT_PATH))
		argp = mod_table[modid].path;
	else {
		/* menu code thanks to Noobz & Fanjita
		 *****************************************************************
		 * If this is the menu, then put the pointer to the menu API block
		 * into argv[1]
		 */

		for (i = 0; mod_table[modid].path[i]; i++)
			argbuf[i] = mod_table[modid].path[i];
		argbuf[i] = '\0';
		_sprintf(argbuf + i + 1, "%08X", (int)&menu_api);
		argp = argbuf;

		arglen += 9;
		dbg_printf("new arglen: %08X\n", arglen);

		menu_api.fname = hb_fname;
	}

	dbg_printf("argp: \"%s\"\n", argp);
	dbg_printf("arglen: %08X\n", arglen);

	dbg_printf("->MODULE MAIN THID: 0x%08X ", thid);
	//The hook is called here to handle thread monitoring
	thid = _hook_sceKernelStartThread(thid, arglen, argp);
	if (thid < 0) {
		dbg_printf(" HB Thread couldn't start. Error 0x%08X\n", thid);
		return thid;
	}

	mod_table[modid].state = RUNNING;

	return modid;
}

int find_module_by_path(const char *path)
{
	int i;

	for (i = 0; i < mod_loaded_num; i++)
		if (!strcmp(mod_table[i].path, path))
			return i;

	return SCE_KERNEL_ERROR_UNKNOWN_MODULE;
}

static int load_util(int module)
{
#ifdef UTILITY_AV_AVCODEC_PATH
	SceUID ret;
#endif

	dbg_printf("Loading 0x%08X\n", module);

	if (isImported(sceUtilityLoadModule))
		return sceUtilityLoadModule(module);

	else if (module <= PSP_MODULE_NET_SSL && isImported(sceUtilityLoadNetModule))
		return sceUtilityLoadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);

	else if (module == PSP_MODULE_USB_PSPCM && isImported(sceUtilityLoadUsbModule))
		return sceUtilityLoadUsbModule(PSP_USB_MODULE_PSPCM);

	else if (module <= PSP_MODULE_USB_GPS && isImported(sceUtilityLoadUsbModule))
		return sceUtilityLoadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);

	else if (module <= PSP_MODULE_AV_G729 && isImported(sceUtilityLoadAvModule))
		return sceUtilityLoadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);

	else
#ifdef UTILITY_AV_AVCODEC_PATH
	if (module == PSP_MODULE_AV_AVCODEC) {
		ret = sceKernelLoadModule(UTILITY_AV_AVCODEC_PATH, 0, NULL);
		return ret < 0 ? ret : sceKernelStartModule(ret, 0, NULL, NULL, NULL);
	} else
#endif
#ifdef UTILITY_AV_SASCORE_PATH
	if (module == PSP_MODULE_AV_SASCORE) {
		ret = sceKernelLoadModule(UTILITY_AV_SASCORE_PATH, 0, NULL);
		return ret < 0 ? ret : sceKernelStartModule(ret, 0, NULL, NULL, NULL);
	} else
#endif
#ifdef UTILITY_AV_ATRAC3PLUS_PATH
	if (module == PSP_MODULE_AV_ATRAC3PLUS) {
		ret = sceKernelLoadModule(UTILITY_AV_ATRAC3PLUS_PATH, 0, NULL);
		return ret < 0 ? ret : sceKernelStartModule(ret, 0, NULL, NULL, NULL);
	} else
#endif
#ifdef UTILITY_AV_MPEGBASE_PATH
	if (module == PSP_MODULE_AV_MPEGBASE) {
		ret = sceKernelLoadModule(UTILITY_AV_MPEGBASE_PATH, 0, NULL);
		return ret < 0 ? ret : sceKernelStartModule(ret, 0, NULL, NULL, NULL);
	} else
#endif
		return SCE_KERNEL_ERROR_ERROR;
}

#ifndef DISABLE_UNLOAD_UTILITY_MODULES
static int unload_util(int module)
{
#ifdef UTILITY_UNLOAD_MODULE_FILE
	int ret;
#endif
	dbg_printf("Unloading 0x%08X\n", module);

	if (isImported(sceUtilityUnloadModule))
		return sceUtilityUnloadModule(module);
	else if (module <= PSP_MODULE_NET_SSL && isImported(sceUtilityUnloadNetModule))
		return sceUtilityUnloadNetModule(module + PSP_NET_MODULE_COMMON - PSP_MODULE_NET_COMMON);
	else if (module == PSP_MODULE_USB_PSPCM && isImported(sceUtilityUnloadUsbModule))
		return sceUtilityUnloadUsbModule(PSP_USB_MODULE_PSPCM);
	else if (module <= PSP_MODULE_USB_GPS && isImported(sceUtilityUnloadUsbModule))
		return sceUtilityUnloadUsbModule(module + PSP_USB_MODULE_MIC - PSP_MODULE_USB_MIC);
	else if (module <= PSP_MODULE_AV_G729 && isImported(sceUtilityUnloadAvModule))
		return sceUtilityUnloadAvModule(module + PSP_MODULE_AV_AVCODEC - PSP_AV_MODULE_AVCODEC);
	else
#ifdef UTILITY_UNLOAD_MODULE_FILE
	{
		ret = sceKernelStopModule(module, 0, NULL, NULL, NULL);
		return ret ? ret : sceKernelUnloadModule(module);
	}
#else
		return SCE_KERNEL_ERROR_ERROR;
#endif
}
#endif

#ifndef DISABLE_UNLOAD_UTILITY_MODULES
static int add_util_table(int module)
{
	int idx;

	// Check if max utilities allowed is reached
	if (mod_utils_num >= MAX_MODULES)
		return -1;

	idx = mod_utils_num;
	mod_utils[idx] = module;
	mod_utils_num++;

	return idx;
}

static int rm_util_table(int module)
{
	int idx;

	for (idx = 0; idx < mod_utils_num; idx++)
		if (mod_utils[idx] == module) {
			mod_utils_num--;
			mod_utils[idx] = mod_utils[mod_utils_num];
			return 0;
		}

	return -1;
}
#endif

void unload_modules()
{
#ifdef DISABLE_UNLOAD_UTILITY_MODULES
	UnloadModules();
#else
	//unload utility modules
	int i, ret;
	for (i = mod_utils_num - 1; i >= 0; i--) {
		dbg_printf("UNLoad utility module id  0x%08X\n", mod_utils[i]);
		ret = unload_util(mod_utils[i]);
		if (ret)
			scr_printf("WARNING! error unloading module 0x%X: 0x%08X\n",
				mod_utils[i], ret);
	}
#endif
	mod_loaded_num = 0;
}

UtilModInfo *get_util_mod_info(const char *lib)
{
	const struct {
		UtilModInfo *mod;
		const char libname[26];
	} libs[] = {
		// Do NOT change the order!

		// sceATRAC3plus_Library
		{ &av_atrac3plus, "sceAtrac3plus" },

		// sceMp3_Library
		{ &av_mp3, "sceMp3" },

		// sceMpeg_library
		{ &av_mpegbase, "sceMpeg" },

		// SceHttp_Library
		{ &net_http, "sceHttp" },

		// sceNetAdhoc_Library
		{ &net_adhoc, "sceNetAdhoc_lib" },
		{ &net_adhoc, "sceNetAdhoc" },

		// sceNetAdhocctl_Library
		{ &net_adhocctl, "sceNetAdhocctl_lib" },
		{ &net_adhocctl, "sceNetAdhocctl" },

		// sceNetAdhocDiscover_Library
		{ &net_adhoc_discover, "sceNetAdhocDiscover" },

		// sceNetAdhocDownload_Library
		{ &net_adhoc_download, "sceNetAdhocDownload" },

		// sceNetAdhocMatching_Library
		{ &net_adhoc_matching, "sceNetAdhocMatching" },

		// sceNetApctl_Library
		{ &net_apctl, "sceNetApctl_internal_user"},
		{ &net_apctl, "sceNetApctl_lib2" },
		{ &net_apctl, "sceNetApctl_lib" },
		{ &net_apctl, "sceNetApctl" },

		// sceNetInet_Library
		{ &net_inet, "sceNetInet_lib" },
		{ &net_inet, "sceNetInet" },

		// sceNetResolver_Library
		{ &net_resolver, "sceNetResolver" },

		// sceNet_Library
		{ &net_common, "sceNet_lib" },
		{ &net_common, "sceNet" },
/*
		// sceSsl_Module
		{ &net_ssl, "sceSsl_lib" },
		{ &net_ssl, "sceSsl" },
*/
		{ NULL, ""}
	};

	unsigned i, j;

	if (lib == NULL)
		return NULL;

	i = j = 0;
	do {
		while (libs[j].libname[i] != lib[i]) {
			if (!libs[j].libname[i])
				return NULL;
			j++;
		}
	} while (lib[i++]);

	return libs[j].mod;
}

// Returns pointer to first export entry for a given module name and library
static SceLibraryEntryTable *find_exports(const char *module, const char *lib)
{
	// Search for module name
	char *p, *foundModule;

	foundModule = (void *)GAME_MEMORY_START;
	do {
		foundModule = findstr(module, foundModule,
			0x0A000000 - (uintptr_t)foundModule);
		if (foundModule == NULL) {
			return NULL;
		}

		// Search for library name next to module name (1 KiB size enough I guess)
		p = findstr(lib, foundModule, 1024);
		if (p != NULL) {
			// Search for pointer to library name close to library name
			p = findw((uintptr_t)p, (void *)(p - 1024), 1024);
		}

		foundModule += 1024;
	} while (p == NULL);

	return (void *)p;
}

// Loads and registers exports from an utility module
SceLibraryEntryTable *load_export_util(UtilModInfo *util_mod, const char *lib)
{
	SceLibraryEntryTable *exports;
	int *p;
	int ret, nid;

	if (util_mod == NULL || lib == NULL)
		return NULL;

	dbg_printf("Loading utility module for library %s\n", lib);

		//force load PSP_MODULE_AV_AVCODEC if we request a specific audio module
	if (util_mod->id > PSP_MODULE_AV_AVCODEC && util_mod->id <= PSP_MODULE_AV_G729) {
		dbg_printf("Force-Loading AVCODEC\n");
		if (isImported(sceUtilityLoadModule))
			sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC);
		else if (isImported(sceUtilityLoadAvModule))
			sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
	} else if(util_mod->id == PSP_MODULE_NET_HTTP) {
		dbg_printf("Force-Loading HTTP\n");
		if (isImported(sceUtilityLoadModule)) {
			sceUtilityLoadModule(PSP_MODULE_NET_COMMON);
			sceUtilityLoadModule(PSP_MODULE_NET_INET);
			sceUtilityLoadModule(PSP_MODULE_NET_PARSEURI);
			sceUtilityLoadModule(PSP_MODULE_NET_PARSEHTTP);
		} else if (isImported(sceUtilityLoadNetModule)) {
			sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
			sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
			sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI);
			sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP);
		}
	}

	ret = load_util(util_mod->id);
	if (ret < 0 && ret != 0x80111102)
		return NULL;

#ifndef DISABLE_UNLOAD_UTILITY_MODULES
#ifdef UTILITY_UNLOAD_MODULE_FILE
	if (!ret)
		ret = util_mod->id;
#endif
	// PSP_MODULE_AV_AVCODEC -> cast syscall of sceAudiocodec and sceVideocodec
	// PSP_MODULE_AV_MP3 -> On 6.20 OFW, libmp3 has a bug when unload it.
	if ((util_mod->id != PSP_MODULE_AV_AVCODEC || globals->isEmu)
		&& (util_mod->id != PSP_MODULE_AV_MP3
			|| globals->module_sdk_version > 0x06020010)) {
#ifdef UTILITY_UNLOAD_MODULE_FILE
		add_util_table(ret);
#else
		add_util_table(util_mod->id);
#endif
	}
#endif

	// Get module exports
	exports = find_exports(util_mod->name, lib);
	if (exports == NULL) {
		dbg_printf("->ERROR: could not find module exports for %s\n", util_mod->name);
#ifndef DISABLE_UNLOAD_UTILITY_MODULES
#ifdef UTILITY_UNLOAD_MODULE_FILE
		unload_util(ret);
		rm_util_table(ret);
#else
		unload_util(util_mod->id);
		rm_util_table(ret);
#endif
#endif
		return NULL;
	}

	dbg_printf("Number of export functions: %d\n", exports->stubcount);
	dbg_printf("Pointer to exports: 0x%08X\n", (int)exports->entrytable);

	switch (util_mod->id) {
		case PSP_MODULE_NET_INET:
			if (util_mod == &net_apctl)
				nid = 0xB3EDD0EC; // sceNetApctlTerm
			else if (util_mod == &net_resolver)
				nid = 0x6138194A; // sceNetResolverTerm
			else if (util_mod == &net_inet)
				nid = 0xA9ED66B9; // sceNetInetTerm
			else
				return exports;
			break;
		case PSP_MODULE_NET_COMMON:
			nid = 0x281928A9; // sceNetTerm
			break;
		default:
			return exports;
	}

	for (p = (int *)exports->entrytable;
		(int)p < (int)exports->entrytable + exports->stubcount;
		p++)
		if (*p == nid) {
			net_term_func[net_term_num] = (void *)p[exports->stubcount];
			net_term_num++;
			break;
		}

	return exports;
}

#include "sdk.h"
#include "eloader.h"
#include "debug.h"

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() in resolve.c

#ifdef FAKE_THREADS
SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority,
                             	   int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{ 
    u32 gp_bak = 0;
	SceUID res;
	
    if (gp) 
	{
        GET_GP(gp_bak);
        SET_GP(gp);
    }
	
    entry_point = entry;
    res = sceKernelCreateThread(name, entry, runThread, stackSize, attr, option);
	
    if (gp) 
	{
        SET_GP(gp_bak);
    }
	
    return res;
}
#endif

#ifdef RETURN_TO_MENU_ON_EXIT
void  _hook_sceKernelExitGame() 
{
	SceUID thid = sceKernelCreateThread("HBL", main_loop, 0x18, 0x10000, 0, NULL);
	
	if (thid >= 0)
	{
		thid = sceKernelStartThread(thid, 0, NULL);
    }
    sceKernelExitDeleteThread(0);
}
#endif

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
    LOGSTR5("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08lX\n", partitionid,name, type, size, addr);
    SceUID uid = sceKernelAllocPartitionMemory(partitionid,name, type, size, addr);
    if (uid <=0)
        LOGSTR1("failed with result: 0x%08lX\n", uid);
    return uid;
}

#ifdef LOAD_MODULE
SceUID _hook_sceKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{	
	LOGSTR0("_hook_sceKernelLoadModule\n");
	LOGSTR1("Attempting to load %s\n", path);
	
	SceUID elf_file = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (elf_file < 0)
	{
		LOGSTR2("Error 0x%08lX opening requested module %s\n", elf_file, path);
		return elf_file;
	}

	SceOff offset = 0;
	LOGSTR1("_hook_sceKernelLoadModule offset: 0x%08lX\n", offset);
	SceUID ret = load_module(elf_file, path, NULL, offset);

	LOGSTR1("load_module returned 0x%08lX\n", ret);

	return ret;
}

int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	LOGSTR0("_hook_sceKernelStartModule\n");
	
	SceUID ret = start_module(modid);
	LOGSTR1("start_module returned 0x%08lX\n", ret);
	
	return ret;
}
#endif

/* WIP
// A function that just returns "ok" but does nothing
int _hook_ok()
{
    return 1;
}
*/

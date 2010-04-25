#include "sdk.h"
#include "eloader.h"
#include "debug.h"

/* Hooks for some functions used by Homebrews */

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


/* WIP
// A function that just returns "ok" but does nothing
int _hook_ok()
{
    return 1;
}
*/

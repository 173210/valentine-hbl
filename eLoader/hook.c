#include "sdk.h"
#include "eloader.h"

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

#ifdef FAKEMEM
int  _hook_sceKernelMaxFreeMemSize () 
{
 return 0x09EC8000 - PRX_LOAD_ADDRESS - hbsize;
}

void *  _hook_sceKernelGetBlockHeadAddr (SceUID mid) 
{
    if (mid == 0x05B8923F)
        return PRX_LOAD_ADDRESS + hbsize;
    return 0;
}

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
	SceUID ret = 0x05B8923F;

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

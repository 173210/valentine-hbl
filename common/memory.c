#include <common/debug.h>
#include <common/sdk.h>
#include <exploit_config.h>

#define MODULES_START_ADDRESS 0x08804000
#define MAX_MODULES_TO_FREE 0x20

int kill_thread(SceUID thid)
{
#ifdef HOOK_sceKernelTerminateThread_WITH_sceKernelTerminateDeleteThread
    int ret = sceKernelTerminateDeleteThread(thid);
#else
    int ret = sceKernelTerminateThread(thid);
#endif
    if (ret < 0)
    {
        if (ret != (int)0x800201A2) //if thread already dormant, let's assume it's not really an error, we still want to delete it!
        {
            dbg_printf("--> ERROR 0x%08X TERMINATING THREAD ID 0x%08X\n", ret, thid);
            return ret;
        }
    }

#ifndef HOOK_sceKernelTerminateThread_WITH_sceKernelTerminateDeleteThread
    ret = sceKernelDeleteThread(thid);
    if (ret < 0)
    {
        dbg_printf("--> ERROR 0x%08X DELETING THREAD ID 0x%08X\n", ret, thid);
    }
#endif

    return ret;
}

void UnloadModules()
{
#ifdef KILL_MODULE_MEMSET
	if( unload_memset_flag != 0){
		dbg_printf("memory.c:memset start\n");
		KILL_MODULE_MEMSET;
		dbg_printf("memory.c:memset done\n");
		unload_memset_flag = 0;
	}
#endif


	// Set inital UID to -1 and the current UID to 0
	int i;
	SceUID uids[MAX_MODULES_TO_FREE];
	uids[0] = -1;
	SceUID cur_uid = 0;

	/* scan through user memory looking for modules ;) */
	for (i = 0; i < (24 << 20); i += 0x400)
	{
		SceUID modid;

		/* check if we've got a UID */
		if ((modid = sceKernelGetModuleIdByAddress(MODULES_START_ADDRESS + i)) >= 0)
		{
			/* we do, make sure it's not just the same one */
			if (uids[cur_uid - ((cur_uid == 0) ? (0) : (1))] != modid)
			{
				/* okay add it */
				uids[cur_uid++] = modid;
			}

			if (cur_uid == MAX_MODULES_TO_FREE)
			{
				dbg_printf("\n->WARNING: Max number of modules to unload reached\n");
				break;
			}
		}
	}

	/* shutdown the modules in usermode */
	for (i = cur_uid - 1; (int)i >= 0; i--)
	{
		sceKernelStopModule(uids[i], 0, NULL, NULL, NULL);
		int ret = sceKernelUnloadModule(uids[i]);
		if (ret < 0)
		{
			dbg_printf("--> ERROR 0x%08X UNLOADING MODULE ID 0x%08X\n", ret, uids[i]);
		}
	}
}

// Those 2 functions are heavy but this avoids 2 extra syscalls that might fail
// In the future if we can have access to the "real" functions, let's remove this
SceSize sceKernelMaxFreeMemSize()
{
    SceSize size, sizeblock;
    SceUID uid;

    dbg_printf("Call to sceKernelMaxFreeMemSize()\n");
    // Init variables
    size = 0;
    sizeblock = 1024 * 1024;

    // Check loop
    while (sizeblock)
    {
        // Increment size
        size += sizeblock;

        /* Allocate block */
        uid = sceKernelAllocPartitionMemory(2, "ValentineFreeMemMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
        if (uid < 0) // Memory allocation failed
        {
            // Restore old size
            size -= sizeblock;

            // Size block / 2
            sizeblock >>= 1;
        }
        else
            sceKernelFreePartitionMemory(uid);
    }

    return size;
}

SceSize sceKernelTotalFreeMemSize()
{
    SceUID blocks[1024];
    u32 count,i;
    SceSize size, x;

    dbg_printf("Call to sceKernelTotalFreeMemSize()\n");
    // Init variables
    size = 0;
    count = 0;

    // Check loop
    for (;;)
    {
        if (count >= sizeof(blocks)/sizeof(blocks[0]))
        {
            dbg_printf("Too many blocks in sceKernelTotalFreeSize, return value will be approximate\n");
            return size;
        }

        // Find max linear size available
        x = sceKernelMaxFreeMemSize();
        if (!(x)) break;

        // Allocate ram
        blocks[count] = sceKernelAllocPartitionMemory(2, "ValentineFreeMemMalloc", PSP_SMEM_Low, x, NULL);
        if (!(blocks[count]))
        {
            dbg_printf("Discrepency between sceKernelMaxFreeMemSize and sceKernelTotalFreeSize, return value will be approximate\n");
            return size;
        }

        // Update variables
        size += x;
        count++;
    }

    for (i=0;i<count;++i)
        sceKernelFreePartitionMemory(blocks[i]);

    return size;
}

#include <common/debug.h>
#include <common/sdk.h>
#include <config.h>

#define MODULES_START_ADDRESS 0x08804000
#define MAX_MODULES_TO_FREE 0x20

int kill_thread(SceUID thid)
{
	int ret;

	if (isImported(sceKernelTerminateDeleteThread)) {
		ret = sceKernelTerminateDeleteThread(thid);
		if (ret)
			dbg_printf("%s: Killing thread 0x%08X failed 0x%08X\n",
				__func__, thid, ret);
		return ret;
	} else {
		if (isImported(sceKernelTerminateThread)) {
			ret = sceKernelTerminateThread(thid);
			if (ret < 0 && ret != SCE_KERNEL_ERROR_DORMANT) {
				dbg_printf("%s: Terminating thread 0x%08X failed 0x%08X\n",
					__func__, thid, ret);
				return ret;
			}
		}

		if (isImported(sceKernelDeleteThread)) {
			ret = sceKernelDeleteThread(thid);
			if (ret < 0)
				dbg_printf("%s: Deleting thread 0x%08X failed 0x%08X\n",
					__func__, thid, ret);
			return ret;
		}

		return 0;
	}
}

// Release all subinterrupt handler
void subinterrupthandler_cleanup()
{
	int i, j;

	if (!isImported(sceKernelReleaseSubIntrHandler))
		return;

	dbg_printf("Subinterrupthandler Cleanup\n");
	// 66 is the highest value of enum PspInterrupts
	for (i = 0; i <= 66; i++) {
		// 30 is the highest value of enum PspSubInterrupts
		for (j = 0; j <= 30; j++) {
			if (sceKernelReleaseSubIntrHandler(i, j) > -1) {
				dbg_printf("Subinterrupt handler released for %d, %d\n", i, j);
			}
		}
	}
	dbg_printf("Subinterrupthandler Cleanup Done\n");
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
SceSize hblKernelMaxFreeMemSize()
{
    SceSize size, sizeblock;
    SceUID uid;

    dbg_printf("Call to hblKernelMaxFreeMemSize()\n");
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

SceSize hblKernelTotalFreeMemSize()
{
    SceUID blocks[1024];
    u32 count,i;
    SceSize size, x;

    dbg_printf("Call to hblKernelTotalFreeMemSize()\n");
    // Init variables
    size = 0;
    count = 0;

    // Check loop
    for (;;)
    {
        if (count >= sizeof(blocks)/sizeof(blocks[0]))
        {
            dbg_printf("Too many blocks in hblKernelTotalFreeSize, return value will be approximate\n");
            return size;
        }

        // Find max linear size available
        x = hblKernelMaxFreeMemSize();
        if (!(x)) break;

        // Allocate ram
        blocks[count] = sceKernelAllocPartitionMemory(2, "ValentineFreeMemMalloc", PSP_SMEM_Low, x, NULL);
        if (!(blocks[count]))
        {
            dbg_printf("Discrepency between hblKernelMaxFreeMemSize and hblKernelTotalFreeSize, return value will be approximate\n");
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

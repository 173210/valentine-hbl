#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "modmgr.h"

#define MODULES_START_ADDRESS 0x08804000
#define MAX_MODULES 0x20

int kill_thread(SceUID thid) 
{
    int ret = sceKernelTerminateThread(thid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX TERMINATING THREAD ID 0x%08lX\n", ret, thid);
        return ret;
    }

    ret = sceKernelDeleteThread(thid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX DELETING THREAD ID 0x%08lX\n", ret, thid);
    }
        
    return ret;
}

int kill_event_flag(SceUID flid)
{
    int ret = sceKernelDeleteEventFlag(flid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX DELETING EVENT FLAG 0x%08lX\n", ret, flid);
        return 0;
    }
    
    return 1;
} 

int kill_sema(SceUID sema)
{
    int ret = sceKernelDeleteSema(sema);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX DELETING SEMAPHORE 0x%08lX\n", ret, sema);
        return 0;
    }
    
    return 1;
} 

int kill_module(SceUID modid) 
{
	sceKernelStopModule(modid, 0, NULL, NULL, NULL);	
	int ret = sceKernelUnloadModule(modid);
	if (ret < 0)
	{
		LOGSTR2("--> ERROR 0x%08lX UNLOADING MODULE ID 0x%08lX\n", ret, modid);
        return 0;
	}
    return 1;
}

void DeleteAllThreads(void)
{
	int i;
	SceUID thids[8];
	
	/* sgx threads */
	thids[0] = *(SceUID*)(0x08B46140);
	thids[1] = *(SceUID*)(0x08C38224);
	thids[2] = *(SceUID*)(0x08C32174);
	thids[3] = *(SceUID*)(0x08C2C0C4);
	thids[4] = *(SceUID*)(0x08C26014);
	thids[5] = *(SceUID*)(0x08B465E4);
	/* file thread */
	thids[6] = *(SceUID*)(0x08B7BBF4);
	// user_main
	thids[7] = *(SceUID*)(0x08B44C28);
	
	/* lets kill these threads now */
	for (i = 0; i < (sizeof(thids)/sizeof(u32)); i++)
	{
		kill_thread(thids[i]);
	}
}

void DeleteAllEventFlags(void)
{
	int i;
	SceUID evids[3];
	
	/* sgx event ids */
	evids[0] = *(SceUID*)(0x08B46634);
	evids[1] = *(SceUID*)(0x08B465F4);
	/* callback event id */
	evids[2] = *(SceUID*)(0x08B7BC00);
	
	/* killin' tiem */
	for (i = 0; i < (sizeof(evids)/sizeof(u32)); i++)
	{
		kill_event_flag(evids[i]);
	}
}

void DeleteAllSemaphores(void)
{
	int i;
	SceUID semaids[28];
	
	/* sgx semaphores */
	semaids[0] = *(SceUID*)(0x08C3822C);
	semaids[1] = *(SceUID*)(0x08C38228);
	semaids[2] = *(SceUID*)(0x08C3217C);
	semaids[3] = *(SceUID*)(0x08C32178);
	semaids[4] = *(SceUID*)(0x08C2C0CC);
	semaids[5] = *(SceUID*)(0x08C2C0C8);
	semaids[6] = *(SceUID*)(0x08C2601C);
	semaids[7] = *(SceUID*)(0x08C26018);
	semaids[8] = *(SceUID*)(0x08B4656C);
	semaids[9] = *(SceUID*)(0x08B46594);
	semaids[10] = *(SceUID*)(0x08B46630);
	semaids[11] = *(SceUID*)(0x08B465F0);
	semaids[12] = *(SceUID*)(0x08B465EC);
	semaids[13] = *(SceUID*)(0x08B465E8);
	semaids[14] = *(SceUID*)(0x08B4612C);
	semaids[15] = *(SceUID*)(0x08C24C90);
	semaids[16] = *(SceUID*)(0x08C24C60);
	
	/* this annoying "Semaphore.cpp" */
	semaids[17] = *(SceUID*)(0x09E658C8);
	semaids[18] = *(SceUID*)(0x09E641C4);
	semaids[19] = *(SceUID*)(0x09E640B4);
	semaids[20] = *(SceUID*)(0x08B7BC18);
	semaids[21] = *(SceUID*)(0x08B64AB0);
	semaids[22] = *(SceUID*)(0x08B64AA4);
	semaids[23] = *(SceUID*)(0x08B64A98);
	semaids[24] = *(SceUID*)(0x08B64A8C);
	semaids[25] = *(SceUID*)(0x08B64A80);
	semaids[26] = *(SceUID*)(0x08B64A74);
	semaids[27] = *(SceUID*)(0x08B64A68);
	
	/* lets destroy these now */
	for (i = 0; i < (sizeof(semaids)/sizeof(u32)); i++)
	{
		/* boom headshot */
		kill_sema(semaids[i]);
	}
}

void DeleteAndUnassignCallbacks(void)
{
	SceUID ret; 
	
	/* clear msevent */
	u32 msevent_cbid = *(SceUID*)(0x08B7BC04);
	sceIoDevctl("fatms0:", 0x02415822, &msevent_cbid, sizeof(msevent_cbid), 0, 0);
	ret = sceKernelDeleteCallback(msevent_cbid);
	
	if (ret < 0)
    {
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", msevent_cbid, ret);
    }
    
	/* there is another msevent callback */
	msevent_cbid = *(SceUID*)(0x08B82C0C);
	sceIoDevctl("fatms0:", 0x02415822, &msevent_cbid, sizeof(msevent_cbid), 0, 0);
	ret = sceKernelDeleteCallback(msevent_cbid);

	if (ret < 0)
    {
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", msevent_cbid, ret);
	}
    
	/* umd callback */
	SceUID umd_cbid = *(SceUID*)(0x08B70D9C);
	sceUmdUnRegisterUMDCallBack(umd_cbid);
	ret = sceKernelDeleteCallback(umd_cbid);

	if (ret < 0)
    {
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", umd_cbid, ret);
    }
}

void free_game_memory()
{
#ifdef DEBUG
    int free;
    int max_free;
	free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	LOGSTR2(" FREE MEM BEFORE CLEANING: %d (max: %d)\n ", free, max_free);
#endif

	DeleteAllThreads();

	DeleteAllEventFlags();

	DeleteAllSemaphores();

	DeleteAndUnassignCallbacks();

	// Set inital UID to -1 and the current UID to 0
	int i;
	SceUID uids[MAX_MODULES];
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

			if (cur_uid == MAX_MODULES)
			{
				LOGSTR0("\n->WARNING: Max number of modules to unload reached\n");
				break;
			}
		}
	}
	
	/* shutdown the modules in usermode */
	for (i = cur_uid - 1; (int)i >= 0; i--)
	{
		kill_module(uids[i]);
	}

#ifdef DEBUG
	free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	LOGSTR2(" FREE MEM AFTER CLEANING: %d (max: %d)\n ", free, max_free);
#endif  

	return;
}

// Those 2 functions are heavy but this avoids 2 extra syscalls that might fail
// In the future if we can have access to the "real" functions, let's remove this
SceSize sceKernelMaxFreeMemSize()
{
    SceSize size, sizeblock;
    u8 *ram;
    SceUID uid;

    LOGSTR0("Call to sceKernelMaxFreeMemSize()\n");
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
    u32 count;
    SceSize size, x;
    int i;

    LOGSTR0("Call to sceKernelTotalFreeMemSize()\n");
    // Init variables
    size = 0;
    count = 0;

    // Check loop
    for (;;)
    {
        if (count >= sizeof(blocks)/sizeof(blocks[0]))
        {
            LOGSTR0("Too many blocks in sceKernelTotalFreeSize, return value will be approximate\n");
            return size;
        }

        // Find max linear size available
        x = sceKernelMaxFreeMemSize();
        if (!(x)) break;

        // Allocate ram
        blocks[count] = sceKernelAllocPartitionMemory(2, "ValentineFreeMemMalloc", PSP_SMEM_Low, x, NULL);
        if (!(blocks[count]))
        {
            LOGSTR0("Discrepency between sceKernelMaxFreeMemSize and sceKernelTotalFreeSize, return value will be approximate\n");
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

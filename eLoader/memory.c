#include "sdk.h"
#include "eloader.h"
#include "thread.h"
#include "debug.h"
#include "modmgr.h"

// Find a FPL by name
SceUID find_fpl(const char *name) 
{
	SceUID readbuf[256];
	int idcount;
	SceKernelFplInfo info;
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Fpl, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);

	for(info.size=sizeof(info); idcount>0; idcount--)
	{		
		if(sceKernelReferFplStatus(readbuf[idcount-1], &info) < 0)
			return -1;
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

// ab5000 system
#ifdef AB5000_FREEMEM
/* Free memory allocated by Game */
/* Warning: MUST NOT be called from Game main thread */
void free_game_memory()
{
    LOGSTR0("ENTER FREE MEMORY\n");
    
#ifdef DEBUG    
    DumpThreadmanObjects();
    //DumpModuleList();
#endif
    
	SceUID id;
	
	const char* const threads[] = {"user_main", "sgx-psp-freq-thr", "sgx-psp-at3-th", "sgx-psp-at3-th", "sgx-psp-at3-th", "sgx-psp-at3-th", "sgx-psp-pcm-th", "FileThread"};
	
	const char* const semas[] = {"sgx-flush-mutex", "sgx-tick-mutex", "sgx-di-sema", "sgx-psp-freq-left-sema", "sgx-psp-freq-free-sema", "sgx-psp-freq-que-sema",
	"sgx-ao-sema", "sgx-psp-sas-attrsema", "sgx-psp-at3-slotsema", "sgx-psp-at3-reqsema", "sgx-psp-at3-bufsema", "sgx-psp-at3-reqsema", "sgx-psp-at3-bufsema", "sgx-psp-at3-reqsema", "sgx-psp-at3-bufsema",
	"sgx-psp-at3-reqsema", "sgx-psp-at3-bufsema"};
	
	const char* const loop_semas[] = {"Semaphore.cpp"};
	
	const char* const evflags[] = {"sgx-ao-evf", "sgx-psp-freq-wait-evf", "loadEndCallbackEventId"};
	
	int nb_threads = sizeof(threads) / sizeof(threads[0]);
	int nb_semas = sizeof(semas) / sizeof(semas[0]);
	int nb_loop_semas = sizeof(loop_semas) / sizeof(loop_semas[0]);
	int nb_evflags = sizeof(evflags) / sizeof(evflags[0]);
	int i, ret, status = 0;

#ifdef DEBUG
	int free;
#endif

    int success = 1;

    sceKernelDelayThread(100);
    
#ifdef DEBUG
	free = sceKernelTotalFreeMemSize();
	write_debug(" FREE MEM BEFORE CLEANUP", &free, sizeof(free));
#endif
	
	// Freeing threads
	for (i = 0; i < nb_threads; ++i)
	{
		id = find_thread(threads[i]);
		if(id >= 0)
		{
			int res = sceKernelTerminateThread(id);			
            if (res < 0) {
                print_to_screen("  Cannot terminate thread, probably syscall failure");
                LOGSTR2("CANNOT TERMINATE %s (err:0x%08lX)\n", threads[i], res);
                success = 0;
            }
			
			res = sceKernelDeleteThread(id);
            if (res < 0) {
                print_to_screen("  Cannot delete thread, probably syscall failure");
                LOGSTR2("CANNOT TERMINATE %s (err:0x%08lX)\n", threads[i], res);
                success = 0;
            } 
			else 
			{
                sceKernelDelayThread(100);
            }
		} 
		else 
		{
            print_to_screen("  Cannot find thread, probably syscall failure");
            LOGSTR2("CANNOT FIND THREAD TO DELETE %s (err:0x%08lX)\n", threads[i], id);
            success = 0;
        }
	}

    LOGSTR0("FREEING SEMAPHORES\n");
	// Freeing semaphores
	for (i = 0; i < nb_semas; ++i)
	{
		id = find_sema(semas[i]);
		if(id >= 0)
		{
			sceKernelDeleteSema(id);
			sceKernelDelayThread(100);
		}
	}
	
	for(i = 0; i < nb_loop_semas; ++i)
	{
		while((id = find_sema(loop_semas[i])) >= 0)
		{
			sceKernelDeleteSema(id);
			sceKernelDelayThread(100);
		}
	}
	
    LOGSTR0("FREEING EVENT FLAGS\n");
	// Freeing event flags
	for (i = 0; i < nb_evflags; ++i)
	{
		id = find_evflag(evflags[i]);
		if(id >= 0)
		{
			sceKernelDeleteEventFlag(id);
			sceKernelDelayThread(100);
		}
	}

    LOGSTR0("FREEING MEMORY PARTITION\n");
	// Free memory partition created by the GAME (UserSbrk)
	sceKernelFreePartitionMemory(*((SceUID *) MEMORY_PARTITION_POINTER));
	
	//sceKernelDelayThread(100000);

    LOGSTR0("FREEING MAIN HEAP\n");
	// Free MAINHEAP FPL
	id = find_fpl("MAINHEAP");
	if(id >= 0)
	{
		sceKernelDeleteFpl(id);
		sceKernelDelayThread(100);
	}
	
#ifdef DEBUG
	free = sceKernelTotalFreeMemSize();
	write_debug(" FREE MEM AFTER NORMAL CLEANUP", &free, sizeof(free));
#endif


#ifdef UNLOAD_MODULE
    if (!success)
        return; //Don't try to unload the module if threads couldn't be stoped
        
	// Stop & Unload game module
	id = find_module("Labo");
	if (id >= 0)
	{		
		ret = sceKernelStopModule(id, 0, NULL, &status, NULL);
		if (ret >= 0)
		{
			ret = sceKernelUnloadModule(id);
			if (ret < 0)
				LOGSTR0("\nERROR UNLOADING GAME MODULE\n");
		}
		else
			DEBUG_PRINT(" ERROR STOPPING GAME MODULE ", &ret, sizeof(ret));
	}
	else
		DEBUG_PRINT(" ERROR: GAME MODULE NOT FOUND ", &id, sizeof(id));

#ifdef DEBUG
	free = sceKernelTotalFreeMemSize();
	write_debug(" FREE MEM AFTER MODULE UNLOADING ", &free, sizeof(free));
#endif    
#endif
  
}
#endif

// Davee system
#ifdef DAVEE_FREEMEM

#define MODULES_START_ADDRESS 0x08804000
#define MAX_MODULES 0x20

int kill_thread(SceUID thid) 
{
    int ret = sceKernelTerminateThread(thid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX TERMINATING THREAD ID 0x%08lX\n", ret, thid);
        return 0;
    }

    ret = sceKernelDeleteThread(thid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX DELETING THREAD ID 0x%08lX\n", ret, thid);
        return 0;
    }    

    return 1;
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
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", msevent_cbid, ret);

	/* there is another msevent callback */
	msevent_cbid = *(SceUID*)(0x08B82C0C);
	sceIoDevctl("fatms0:", 0x02415822, &msevent_cbid, sizeof(msevent_cbid), 0, 0);
	ret = sceKernelDeleteCallback(msevent_cbid);

	if (ret < 0)
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", msevent_cbid, ret);
	
	/* umd callback */
	SceUID umd_cbid = *(SceUID*)(0x08B70D9C);
	sceUmdUnRegisterUMDCallBack(umd_cbid);
	ret = sceKernelDeleteCallback(umd_cbid);

	if (ret < 0)
		LOGSTR2("Unable to delete callback 0x%08lX, error 0x%08lX\n", umd_cbid, ret);
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
#endif


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
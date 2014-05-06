#include <common/sdk.h>
#include <hbl/eloader.h>
#include <common/debug.h>
#include <hbl/mod/modmgr.h>
#include <hbl/stubs/hook.h>
#include <common/globals.h>
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
            LOGSTR2("--> ERROR 0x%08lX TERMINATING THREAD ID 0x%08lX\n", ret, thid);
            return ret;
        }
    }

#ifndef HOOK_sceKernelTerminateThread_WITH_sceKernelTerminateDeleteThread
    ret = sceKernelDeleteThread(thid);
    if (ret < 0)
    {
        LOGSTR2("--> ERROR 0x%08lX DELETING THREAD ID 0x%08lX\n", ret, thid);
    }
#endif
        
    return ret;
}
#ifdef SUSPEND_THEN_DELETE_THREADS
void SuspendAllThreads()
{
    LOGSTR0("memory.c:SuspendAllThreads\n");
	u32 i;
	u32 thaddrs[] = TH_ADDR_LIST;
	SceUID thuids[] = TH_ADDR_LIST;
	
	// Suspend all threads and remember their UIDs
	for (i = 0; i < (sizeof(thaddrs)/sizeof(u32)); i++)
	{
		thuids[i] = *(SceUID*)(thaddrs[i]);
		int result = sceKernelSuspendThread(thuids[i]);
		LOGSTR2("Suspending thread 0x%08lX, result is 0x%08lX\n", thuids[i], result);
	}

	LOGSTR0("All threads suspended\n");
}


void SuicideAllThreads(void)
{
	u32 i;
	u32 thaddrs[] = TH_ADDR_LIST;
	SceUID thuids[] = TH_ADDR_LIST;
	
	// Suspend all threads and remember their UIDs
	for (i = 0; i < (sizeof(thaddrs)/sizeof(u32)); i++)
	{
		thuids[i] = *(SceUID*)(thaddrs[i]);
		int result = sceKernelSuspendThread(thuids[i]);
		LOGSTR2("Suspending thread 0x%08lX, result is 0x%08lX\n", thuids[i], result);
	}

	LOGSTR0("All threads suspended\n");

	unsigned int* address = (unsigned int*)0x09A00000;
	
	// Get call for sceKernelExitDeleteThread
	int nid_index = get_nid_index(0x809CE29B); // sceKernelExitDeleteThread
	LOGSTR1("Index for NID sceKernelExitDeleteThread is: %d\n", nid_index);
	
	tGlobals * g = get_globals();
	unsigned int syscall = g->nid_table.table[nid_index].call;
	LOGSTR2("Call for NID sceKernelExitDeleteThread is: 0x%08lX 0x%08lX\n", GET_SYSCALL_NUMBER(syscall), syscall);
	
	// Write syscall instruction to memory and empty the memory
	*address =  syscall; 
	*((unsigned int*)0x09A00004) = 0x34840000; // xori $a0, $a0, 0 ($a0 = 0)
	
	// Zero memory from top to bottom	
	for (i = (unsigned int)address - 1; i >= 0x08804000; i--)
	  *((char*)i) = 0;
	 
	// Resume threads
	for (i = 0; i < (sizeof(thaddrs)/sizeof(u32)); i++)
	{
		int result = sceKernelResumeThread(thuids[i]);
		LOGSTR2("Resuming thread 0x%08lX, result is 0x%08lX\n", thuids[i], result);
	}
	
	LOGSTR0("All threads resumed\n");
}
#endif

#ifdef TH_ADDR_LIST
void DeleteAllThreads(void)
{
    LOGSTR0("memory.c:DeleteAllThreads\n");
	u32 i;
	u32 thaddrs[] = TH_ADDR_LIST;
	
	/* lets kill these threads now */
	for (i = 0; i < (sizeof(thaddrs)/sizeof(u32)); i++)
	{
		kill_thread(*(SceUID*)(thaddrs[i]));
	}
}
#endif

#ifdef ALARM_ADDR_LIST
void CancelAllAlarms(void)
{
    LOGSTR0("memory.c:CancelAllAlarms\n");
	u32 i;
	u32 alarmaddrs[] = ALARM_ADDR_LIST;


	for (i = 0; i < (sizeof(alarmaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelCancelAlarm(*(SceUID*)(alarmaddrs[i]));
		if (ret < 0)
		{
			LOGSTR2("--> ERROR 0x%08lX CANCELING ALARM 0x%08lX\n", ret, *(SceUID*)(alarmaddrs[i]));
		}
	}
}
#endif

#ifdef LWMUTEX_ADDR_LIST
void DeleteAllLwMutexes(void)
{
    LOGSTR0("memory.c:DeleteAllLwMutexes\n");
	u32 i;
	u32 lwmutexaddrs[] = LWMUTEX_ADDR_LIST;

	for (i = 0; i < (sizeof(lwmutexaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelDeleteLwMutex((SceLwMutexWorkarea *)(lwmutexaddrs[i]));
		if (ret < 0)
		{
			LOGSTR2("--> ERROR 0x%08lX DELETING LWMUTEX 0x%08lX\n", ret, lwmutexaddrs[i]);
		}
	}
}
#endif

#ifdef EV_ADDR_LIST
void DeleteAllEventFlags(void)
{
    LOGSTR0("memory.c:DeleteAllEventFlags\n");
	u32 i;
	u32 evaddrs[] = EV_ADDR_LIST;
	
	/* killin' tiem */
	for (i = 0; i < (sizeof(evaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelDeleteEventFlag(*(SceUID*)(evaddrs[i]));
		if (ret < 0)
		{
			LOGSTR2("--> ERROR 0x%08lX DELETING EVENT FLAG 0x%08lX\n", ret, *(SceUID*)(evaddrs[i]));
		}
	}
}
#endif

#ifdef SEMA_ADDR_LIST
void DeleteAllSemaphores(void)
{
#ifndef HOOK_sceKernelDeleteSema_WITH_dummy
    LOGSTR0("memory.c:DeleteAllSemaphores\n");
	u32 i;
	u32 semaaddrs[] = SEMA_ADDR_LIST;
	
	/* sgx semaphores */

	
	/* lets destroy these now */
	for (i = 0; i < (sizeof(semaaddrs)/sizeof(u32)); i++)
	{
#ifdef SIGNAL_SEMA_BEFORE_DELETE
		sceKernelSignalSema(*(SceUID*)(semaaddrs[i]), -1);
#endif
		/* boom headshot */
		int ret = sceKernelDeleteSema(*(SceUID*)(semaaddrs[i]));
		if (ret < 0)
		{
			LOGSTR2("--> ERROR 0x%08lX DELETING SEMAPHORE 0x%08lX\n", ret, *(SceUID*)(semaaddrs[i]));
		}
	}
#endif
}
#endif

#ifdef KILL_MODULE_MEMSET
static int unload_memset_flag = 0;
#endif

void UnloadModules()
{
#ifndef LOAD_MODULES_FOR_SYSCALLS
#ifdef UNLOAD_ADDITIONAL_MODULES
    LOGSTR0("memory.c:UnloadModules\n");
	// Unload user modules first
	// The more basic modules got a lower ID, so this should be the correct 
	// order to unload
    
	SceUID result;
	int m = PSP_MODULE_AV_G729;
	while (m >= PSP_MODULE_AV_AVCODEC)
	{
		if( !( (m == PSP_MODULE_AV_AVCODEC || m == PSP_MODULE_AV_MP3) && getFirmwareVersion() <= 620 ) )
		{
			result = sceUtilityUnloadModule(m);
			LOGSTR2("unloading utility module 0x%08lX, result 0x%08lX\n", m, result);
			m--;
		}
	}

	m = PSP_MODULE_NET_SSL;
	while (m >= PSP_MODULE_NET_COMMON)
	{
        LOGSTR1("unloading utility module 0x%08lX...", m);
		result = sceUtilityUnloadModule(m);
		LOGSTR1("result 0x%08lX\n", result);
		m--;
	}
#endif
#endif

#ifdef KILL_MODULE_MEMSET
	if( unload_memset_flag != 0){
		LOGSTR0("memory.c:memset start\n");
		KILL_MODULE_MEMSET;
		LOGSTR0("memory.c:memset done\n");
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
				LOGSTR0("\n->WARNING: Max number of modules to unload reached\n");
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
			LOGSTR2("--> ERROR 0x%08lX UNLOADING MODULE ID 0x%08lX\n", ret, uids[i]);
		}
	}
}

#if defined(GAME_FREEMEM_ADDR) || defined(GAME_FREEMEM_BRUTEFORCE_NUM)
void FreeMem()
{
	unsigned i;

	LOGSTR0("memory.c:FreeMem\n");

#ifdef GAME_FREEMEM_ADDR
    SceUID memids[] = GAME_FREEMEM_ADDR;
   
    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelFreePartitionMemory(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            LOGSTR2("--> ERROR 0x%08lX FREEING PARTITON MEMORY ID 0x%08lX\n", ret, *(SceUID*)memids[i]);
        }
    }
#endif
#ifdef GAME_FREEMEM_BRUTEFORCE_NUM
	SceUID blockid, init;
	int freed = 0;

#ifdef GAME_FREEMEM_BRUTEFORCE_OVER_ADDR
	SceUID overaddr[] = GAME_FREEMEM_BRUTEFORCE_OVER_ADDR;

	for (i = 0; i < sizeof(overaddr) / sizeof(SceUID); i++)
	{
		init = *(SceUID *)overaddr[i];
		for (blockid = init; GAME_FREEMEM_BRUTEFORCE_OVER_CONDITION(blockid, init); blockid++)
		{
			if (!sceKernelFreePartitionMemory(blockid))
			{
				freed++;
				if (freed >= GAME_FREEMEM_BRUTEFORCE_NUM)
				{
					return;
				}
			}
		}
	}
#endif
#ifdef GAME_FREEMEM_BRUTEFORCE_UNDER_ADDR
	SceUID underaddr[] = GAME_FREEMEM_BRUTEFORCE_UNDER_ADDR;

	for (i = 0; i < sizeof(overaddr) / sizeof(SceUID); i++)
	{
		init = *(SceUID *)underaddr[i];
		for (blockid = init; GAME_FREEMEM_BRUTEFORCE_UNDER_CONDITION(blockid, init); blockid++)
		{
			if (!sceKernelFreePartitionMemory(blockid))
			{
				freed++;
				if (freed >= GAME_FREEMEM_BRUTEFORCE_NUM)
				{
					return;
				}
			}
		}
	}
#endif
#endif
}
#endif


#ifdef VPL_ADDR_LIST
void FreeVpl()
{
    LOGSTR0("memory.c:FreeVpl\n");
    u32 i;
    SceUID memids[] = VPL_ADDR_LIST;

    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelDeleteVpl(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            LOGSTR2("--> ERROR 0x%08lX Deleting VPL ID 0x%08lX\n", ret, *(SceUID*)memids[i]);
        }
    }
}
#endif

#ifdef FPL_ADDR_LIST
void FreeFpl()
{
    LOGSTR0("memory.c:FreeFpl\n");
    u32 i;
    SceUID memids[] = FPL_ADDR_LIST;

    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelDeleteFpl(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            LOGSTR2("--> ERROR 0x%08lX Deleting FPL ID 0x%08lX\n", ret, *(SceUID*)memids[i]);
        }
    }
}
#endif

// Iterate through possible file descriptors to close all files left
// open by the exploitet game
void CloseFiles()
{
    LOGSTR0("memory.c:CloseFiles\n");
	SceUID result;
	int i;

	for (i = 0; i < 20; i++) // How many files can be open at once?
	{
		result = sceIoClose(i);
		if (result != (int)0x80020323) // bad file descriptor
		{
			LOGSTR2("tried closing file %d, result 0x%08lX\n", i, result);
		}
	}
}

void free_game_memory()
{
#ifdef DEBUG
    int is_free;
    int max_free;
	is_free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	LOGSTR2(" FREE MEM BEFORE CLEANING: %d (max: %d)\n ", is_free, max_free);
#endif

#if defined(GAME_FREEMEM_ADDR) || defined(GAME_FREEMEM_BRUTEFORCE_NUM)
	FreeMem();
#endif

#ifdef SUB_INTR_HANDLER_CLEANUP
	subinterrupthandler_cleanup();
#endif
    
#ifdef SUSPEND_THEN_DELETE_THREADS
	SuspendAllThreads();
#else
#ifdef TH_ADDR_LIST
	DeleteAllThreads();
#endif
#endif

#ifdef ALARM_ADDR_LIST
	CancelAllAlarms();
#endif

#ifdef LWMUTEX_ADDR_LIST
	DeleteAllLwMutexes();
#endif

#ifdef EV_ADDR_LIST
	DeleteAllEventFlags();
#endif

#ifdef SEMA_ADDR_LIST
	DeleteAllSemaphores();
#endif


#ifdef VPL_ADDR_LIST
    FreeVpl();
#endif

#ifdef FPL_ADDR_LIST
    FreeFpl();
#endif


#ifdef KILL_MODULE_MEMSET
unload_memset_flag = 1;
#endif

#ifdef SUSPEND_THEN_DELETE_THREADS
	// Delete module here before cleaning the threads,
	// otherwise the main module cannot be unloaded
	UnloadModules();
	SuicideAllThreads();
	UnloadModules();
#else
	UnloadModules();
#endif
	
	CloseFiles();

#ifdef DEBUG
	is_free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	LOGSTR2(" FREE MEM AFTER CLEANING: %d (max: %d)\n ", is_free, max_free);
#endif  

	return;
}

// Those 2 functions are heavy but this avoids 2 extra syscalls that might fail
// In the future if we can have access to the "real" functions, let's remove this
SceSize sceKernelMaxFreeMemSize()
{
    SceSize size, sizeblock;
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
    u32 count,i;
    SceSize size, x;

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

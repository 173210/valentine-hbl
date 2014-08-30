#include <common/debug.h>
#include <common/memory.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <loader/bruteforce.h>
#include <exploit_config.h>

#ifdef SUSPEND_THEN_DELETE_THREADS
void SuspendAllThreads()
{
    dbg_printf("memory.c:SuspendAllThreads\n");
	u32 i;
	u32 thaddrs[] = TH_ADDR_LIST;
	SceUID thuids[] = TH_ADDR_LIST;

	// Suspend all threads and remember their UIDs
	for (i = 0; i < (sizeof(thaddrs)/sizeof(u32)); i++)
	{
		thuids[i] = *(SceUID*)(thaddrs[i]);
		int result = sceKernelSuspendThread(thuids[i]);
		dbg_printf("Suspending thread 0x%08X, result is 0x%08X\n", thuids[i], result);
	}

	dbg_printf("All threads suspended\n");
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
		dbg_printf("Suspending thread 0x%08X, result is 0x%08X\n", thuids[i], result);
	}

	dbg_printf("All threads suspended\n");

	unsigned int* address = (unsigned int*)0x09A00000;

	// Get call for sceKernelExitDeleteThread
	int nid_index = get_nid_index(0x809CE29B); // sceKernelExitDeleteThread
	dbg_printf("Index for NID sceKernelExitDeleteThread is: %d\n", nid_index);

	int syscall = globals->nid_table[nid_index].call;
	dbg_printf("Call for NID sceKernelExitDeleteThread is: 0x%08X 0x%08X\n", GET_SYSCALL_NUMBER(syscall), syscall);

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
		dbg_printf("Resuming thread 0x%08X, result is 0x%08X\n", thuids[i], result);
	}

	dbg_printf("All threads resumed\n");
}
#endif

#ifdef TH_ADDR_LIST
void DeleteAllThreads(void)
{
    dbg_printf("memory.c:DeleteAllThreads\n");
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
    dbg_printf("memory.c:CancelAllAlarms\n");
	u32 i;
	u32 alarmaddrs[] = ALARM_ADDR_LIST;


	for (i = 0; i < (sizeof(alarmaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelCancelAlarm(*(SceUID*)(alarmaddrs[i]));
		if (ret < 0)
		{
			dbg_printf("--> ERROR 0x%08X CANCELING ALARM 0x%08X\n", ret, *(SceUID*)(alarmaddrs[i]));
		}
	}
}
#endif

#ifdef LWMUTEX_ADDR_LIST
void DeleteAllLwMutexes(void)
{
    dbg_printf("memory.c:DeleteAllLwMutexes\n");
	u32 i;
	u32 lwmutexaddrs[] = LWMUTEX_ADDR_LIST;

	for (i = 0; i < (sizeof(lwmutexaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelDeleteLwMutex((SceLwMutexWorkarea *)(lwmutexaddrs[i]));
		if (ret < 0)
		{
			dbg_printf("--> ERROR 0x%08X DELETING LWMUTEX 0x%08X\n", ret, lwmutexaddrs[i]);
		}
	}
}
#endif

#ifdef EV_ADDR_LIST
void DeleteAllEventFlags(void)
{
    dbg_printf("memory.c:DeleteAllEventFlags\n");
	u32 i;
	u32 evaddrs[] = EV_ADDR_LIST;

	/* killin' tiem */
	for (i = 0; i < (sizeof(evaddrs)/sizeof(u32)); i++)
	{
		int ret = sceKernelDeleteEventFlag(*(SceUID*)(evaddrs[i]));
		if (ret < 0)
		{
			dbg_printf("--> ERROR 0x%08X DELETING EVENT FLAG 0x%08X\n", ret, *(SceUID*)(evaddrs[i]));
		}
	}
}
#endif

#ifdef SEMA_ADDR_LIST
void DeleteAllSemaphores(void)
{
#ifndef HOOK_sceKernelDeleteSema_WITH_dummy
    dbg_printf("memory.c:DeleteAllSemaphores\n");
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
			dbg_printf("--> ERROR 0x%08X DELETING SEMAPHORE 0x%08X\n", ret, *(SceUID*)(semaaddrs[i]));
		}
	}
#endif
}
#endif

#ifdef KILL_MODULE_MEMSET
static int unload_memset_flag = 0;
#endif

#ifdef GAME_FREEMEM_ADDR
void FreeMem()
{
	unsigned i;

	dbg_printf("memory.c:FreeMem\n");

    SceUID memids[] = GAME_FREEMEM_ADDR;

    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelFreePartitionMemory(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            dbg_printf("--> ERROR 0x%08X FREEING PARTITON MEMORY ID 0x%08X\n", ret, *(SceUID*)memids[i]);
        }
    }
}
#endif


#ifdef VPL_ADDR_LIST
void FreeVpl()
{
    dbg_printf("memory.c:FreeVpl\n");
    u32 i;
    SceUID memids[] = VPL_ADDR_LIST;

    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelDeleteVpl(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            dbg_printf("--> ERROR 0x%08X Deleting VPL ID 0x%08X\n", ret, *(SceUID*)memids[i]);
        }
    }
}
#endif

#ifdef FPL_ADDR_LIST
void FreeFpl()
{
    dbg_printf("memory.c:FreeFpl\n");
    u32 i;
    SceUID memids[] = FPL_ADDR_LIST;

    for(i = 0; i < sizeof(memids)/sizeof(u32); i++)
    {
        int ret = sceKernelDeleteFpl(*(SceUID*)memids[i]);
        if (ret < 0)
        {
            dbg_printf("--> ERROR 0x%08X Deleting FPL ID 0x%08X\n", ret, *(SceUID*)memids[i]);
        }
    }
}
#endif

// Iterate through possible file descriptors to close all files left
// open by the exploitet game
void CloseFiles()
{
    dbg_printf("memory.c:CloseFiles\n");
	SceUID result;
	int i;

	for (i = 0; i < 20; i++) // How many files can be open at once?
	{
		result = sceIoClose(i);
		if (result != (int)0x80020323) // bad file descriptor
		{
			dbg_printf("tried closing file %d, result 0x%08X\n", i, result);
		}
	}
}

void ReleaseAudioCh()
{
	int i;

	for (i = 0; i <= PSP_AUDIO_CHANNEL_MAX; i++)
		sceAudioChRelease(i);
}

void preload_free_game_memory()
{
	dbg_printf(" FREE MEM BEFORE CLEANING: %d (max: %d)\n ",
		sceKernelTotalFreeMemSize(), sceKernelMaxFreeMemSize());

#ifdef GAME_FREEMEM_ADDR
	FreeMem();
#endif

#ifdef GAME_FREEMEM_BRUTEFORCE_INIT
	dbg_printf("Freeing memory with brute force...\n");
	freemem_bruteforce(GAME_FREEMEM_BRUTEFORCE_INIT,
		(void *)GAME_FREEMEM_BRUTEFORCE_ADDR);
#endif

#ifdef SUB_INTR_HANDLER_CLEANUP
	subinterrupthandler_cleanup();
#endif

#ifdef DEBUG
	dbg_printf(" FREE MEM AFTER CLEANING: %d (max: %d)\n ",
		 sceKernelTotalFreeMemSize(), sceKernelMaxFreeMemSize());
#endif
}

void free_game_memory()
{
#ifdef DEBUG
    int is_free;
    int max_free;
	is_free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	dbg_printf(" FREE MEM BEFORE CLEANING: %d (max: %d)\n ", is_free, max_free);
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

#ifndef LOAD_MODULES_FOR_SYSCALLS
	unload_utils();
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

	ReleaseAudioCh();

#ifdef DEBUG
	is_free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	dbg_printf(" FREE MEM AFTER CLEANING: %d (max: %d)\n ", is_free, max_free);
#endif

	return;
}

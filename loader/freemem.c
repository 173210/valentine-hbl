#include <common/debug.h>
#include <common/globals.h>
#include <common/memory.h>
#include <common/sdk.h>
#include <loader/bruteforce.h>
#include <loader/runtime.h>
#include <config.h>

#ifdef TH_ADDR_LIST
void DeleteAllThreads(void)
{
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
	u32 i;
	u32 alarmaddrs[] = ALARM_ADDR_LIST;
#ifdef ALARM_HANDLERS_LIST
	u32 alarmhandlers[] = ALARM_HANDLERS_LIST;

	for (i = 0; i < sizeof(alarmhandlers) / sizeof(u32); i++) {
		// Write to uncached memory
		((u32 *)(alarmhandlers[i] | 0x40000000))[0] = JR_ASM(REG_RA);
		((u32 *)(alarmhandlers[i] | 0x40000000))[1] = LUI_ASM(REG_V0, 0);
		// Fill instruction cache
		__asm__("cache 0x14, 0(%0)" : "=r"(alarmhandlers[i]));
	}
#endif
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
	u32 i;
	u32 semaaddrs[] = SEMA_ADDR_LIST;

	if (!isImported(sceKernelDeleteSema))
		return;

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
}
#endif

#ifdef KILL_MODULE_MEMSET
static int unload_memset_flag = 0;
#endif

#ifdef GAME_FREEMEM_ADDR
void FreeMem()
{
	unsigned i;

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
	dbg_printf("%s: Before cleaning: %d (max: %d)\n", __func__,
		hblKernelTotalFreeMemSize(), hblKernelMaxFreeMemSize());

#ifdef GAME_FREEMEM_ADDR
	dbg_printf("%s: Freeing partiton memory\n", __func__);
	FreeMem();
#endif

#ifdef GAME_FREEMEM_BRUTEFORCE_INIT
	dbg_printf("%s: Freeing memory with brute force\n", __func__);
	freemem_bruteforce(GAME_FREEMEM_BRUTEFORCE_INIT,
		(void *)GAME_FREEMEM_BRUTEFORCE_ADDR);
#endif

	subinterrupthandler_cleanup();

	dbg_printf("%s: After cleaning: %d (max: %d)\n", __func__,
		hblKernelTotalFreeMemSize(), hblKernelMaxFreeMemSize());
}

void free_game_memory()
{
	dbg_printf("%s: Before cleaning: %d (max: %d)\n", __func__,
		hblKernelTotalFreeMemSize(), hblKernelMaxFreeMemSize());
\
#ifdef TH_ADDR_LIST
	dbg_printf("%s: Deleting all threads\n", __func__);
	DeleteAllThreads();
#endif

#ifdef ALARM_ADDR_LIST
	dbg_printf("%s: Canceling all alarms\n", __func__);
	CancelAllAlarms();
#endif

#ifdef LWMUTEX_ADDR_LIST
	dbg_printf("%s: Deleting all LwMutexes\n", __func__);
	DeleteAllLwMutexes();
#endif

#ifdef EV_ADDR_LIST
	dbg_printf("%s: Deleting all eventFlags\n", __func__);
	DeleteAllEventFlags();
#endif

#ifdef SEMA_ADDR_LIST
	dbg_printf("%s: Deleting all semaphores\n", __func__);
	DeleteAllSemaphores();
#endif


#ifdef VPL_ADDR_LIST
	dbg_printf("%s: Freeing VPL\n", __func__);
	FreeVpl();
#endif

#ifdef FPL_ADDR_LIST
	dbg_printf("%s: Freeing FPL\n", __func__);
	FreeFpl();
#endif


#ifdef KILL_MODULE_MEMSET
unload_memset_flag = 1;
#endif

	dbg_printf("%s: Unloading modules\n", __func__);
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

	dbg_printf("%s: Closing files\n", __func__);
	CloseFiles();

	dbg_printf("%s: Releasing audio channels\n", __func__);
	ReleaseAudioCh();

	dbg_printf("%s: After cleaning: %d (max: %d)\n", __func__,
		hblKernelTotalFreeMemSize(), hblKernelMaxFreeMemSize());

	return;
}

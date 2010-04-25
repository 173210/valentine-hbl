#include "sdk.h"
#include "eloader.h"
#include "thread.h"
#include "debug.h"
#include "modmgr.h"

/* Find a FPL by name */
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
		DEBUG_PRINT(" GAME MODULE ID ", &id, sizeof(id));
		
		ret = sceKernelStopModule(id, 0, NULL, &status, NULL);
		if (ret >= 0)
		{
			ret = sceKernelUnloadModule(id);
			if (ret < 0)
				DEBUG_PRINT(" ERROR UNLOADING GAME MODULE ", &ret, sizeof(ret));
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

#ifdef DAVEE_FREEMEM

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

int kill_module(const char * name, SceUID modid) {
	sceKernelStopModule(modid, 0, NULL, NULL, NULL);	
	int ret = sceKernelUnloadModule(modid);
	if (ret < 0)
	{
		LOGSTR2("--> ERROR 0x%08lX UNLOADING %s\n", ret, name);
        return 0;
	}
    return 1;
}

// Davee's addresses
void free_game_memory()
{
#ifdef DEBUG
    int free;
    int max_free;
	free = sceKernelTotalFreeMemSize();
    max_free = sceKernelMaxFreeMemSize();
	LOGSTR2(" FREE MEM BEFORE CLEANING: %d (max: %d)\n ", free, max_free);
#endif  

	int i, ret;
	SceUID sgx_thids[6];
	SceUID sgx_evids[2];

	// It's killin' tiem, lets raeps sum threads
	sgx_thids[0] = *(SceUID*)0x08B46140;
	sgx_thids[1] = *(SceUID*)0x08C38224;
	sgx_thids[2] = *(SceUID*)0x08C32174;
	sgx_thids[3] = *(SceUID*)0x08C2C0C4;
	sgx_thids[4] = *(SceUID*)0x08C26014;
	sgx_thids[5] = *(SceUID*)0x08B465E4;
	
	// lets kill these threads now
	for (i = 0; i < (sizeof(sgx_thids)/sizeof(u32)); i++)
        kill_thread(sgx_thids[i]);

	// thread "FileThread"	(terminate + delete)
    kill_thread(*(SceUID*)0x08B7BBF4);

	// thread user_main (terminate + delete)
    kill_thread(*(SceUID*)0x08B44C28);	

	// lets kill some eventflags
	sgx_evids[0] = *(SceUID*)0x08B46634;
	sgx_evids[1] = *(SceUID*)0x08B465F4;
	
	// killin' tiem
	for (i = 0; i < (sizeof(sgx_evids)/sizeof(u32)); i++)
        kill_event_flag(sgx_evids[i]);
        
	// kill callback evid
	kill_event_flag(*(SceUID*)0x08B7BC00);
	
	// lets kill some semaphores
	SceUID sgx_semaids[17];
	SceUID semaids[11];
	
	// lets load the ids... ALL OF THEM
	sgx_semaids[0] = *(SceUID*)0x08c3822c;
	sgx_semaids[1] = *(SceUID*)0x08c38228;
	sgx_semaids[2] = *(SceUID*)0x08c3217c;
	sgx_semaids[3] = *(SceUID*)0x08c32178;
	sgx_semaids[4] = *(SceUID*)0x08c2c0cc;
	sgx_semaids[5] = *(SceUID*)0x08c2c0c8;
	sgx_semaids[6] = *(SceUID*)0x08c2601c;
	sgx_semaids[7] = *(SceUID*)0x08c26018;
	sgx_semaids[8] = *(SceUID*)0x08b4656c;
	sgx_semaids[9] = *(SceUID*)0x08b46594;
	sgx_semaids[10] = *(SceUID*)0x08b46630;
	sgx_semaids[11] = *(SceUID*)0x08b465f0;
	sgx_semaids[12] = *(SceUID*)0x08b465ec;
	sgx_semaids[13] = *(SceUID*)0x08b465e8;
	sgx_semaids[14] = *(SceUID*)0x08b4612c;
	sgx_semaids[15] = *(SceUID*)0x08c24c90;
	sgx_semaids[16] = *(SceUID*)0x08c24c60;
	
	// lets destroy these now
	for (i = 0; i < (sizeof(sgx_semaids)/sizeof(u32)); i++)
		kill_sema(sgx_semaids[i]);
	
	// this annoying "Semaphore.cpp"
	semaids[0] = *(SceUID*)0x09E658C8;
	semaids[1] = *(SceUID*)0x09e641c4;
	semaids[2] = *(SceUID*)0x09e640b4;
	semaids[3] = *(SceUID*)0x08b7bc18;
	semaids[4] = *(SceUID*)0x08b64ab0;
	semaids[5] = *(SceUID*)0x08b64aa4;
	semaids[6] = *(SceUID*)0x08b64a98;
	semaids[7] = *(SceUID*)0x08b64a8c;
	semaids[8] = *(SceUID*)0x08b64a80;
	semaids[9] = *(SceUID*)0x08b64a74;
	semaids[10] = *(SceUID*)0x08b64a68;
	
	/* lets destroy these now */
	for (i = 0; i < (sizeof(semaids)/sizeof(u32)); i++)
        kill_sema(semaids[i]);
	
	/* delete callbacks */
	u32 msevent_cbid = *(u32*)0x8b7bc04;
	sceIoDevctl("fatms0:", 0x02415822, &msevent_cbid, sizeof(msevent_cbid), 0, 0);	
	ret = sceKernelDeleteCallback(msevent_cbid);
	if (ret < 0)
		LOGSTR1("--> ERROR 0x%08lX DELETING MS CALLBACK\n", ret);
	
	/* umd callback */
	SceUID umd_cbid = *(SceUID*)0x8b70d9c;
	sceUmdUnRegisterUMDCallBack(umd_cbid);	
	sceKernelDeleteCallback(umd_cbid);
	if (ret < 0)
		LOGSTR1("--> ERROR 0x%08lX DELETING UMD CALLBACK\n", ret);
	
	/* there is another msevent callback */
	msevent_cbid = *(SceUID*)0x8b82c0c;
	ret = sceIoDevctl("fatms0:", 0x02415822, &msevent_cbid, sizeof(msevent_cbid), 0, 0);
	if (ret < 0)
		LOGSTR1("--> ERROR 0x%08lX GETTING MS CALLBACK 2\n", ret);
	
	ret = sceKernelDeleteCallback(msevent_cbid);
	if (ret < 0)
		LOGSTR1("--> ERROR 0x%08lX DELETING MS CALLBACK 2\n", ret);
	
	/* shutdown the modules in usermode */
	
	/* sceFont_Library */
	kill_module("sceFont_Library", *(SceUID*)0x8b8cbb8);
	
	/* scePsmf_library */
    kill_module("scePsmf_library", *(SceUID*)0x8b8ca7c);

	/* sceCcc_Library */
    kill_module("sceCcc_Library", *(SceUID*)0x8b8c940);
	
	/* sceNetAdhocDiscover_Library */
    kill_module("sceNetAdhocDiscover_Library", sceKernelGetModuleIdByAddress(0x09EB3A00));
	
	/* sceNetAdhocDownload_Library */
    kill_module("sceNetAdhocDownload_Library", sceKernelGetModuleIdByAddress(0x09EAFC00));

	/* sceNetAdhocMatching_Library */
    kill_module("sceNetAdhocMatching_Library", sceKernelGetModuleIdByAddress(0x09EAB200));
	
	/* sceNetAdhocctl_Library */
    kill_module("sceNetAdhocctl_Library", sceKernelGetModuleIdByAddress(0x09EA3300));
	
	/* sceNetAdhoc_Library */
    kill_module("sceNetAdhoc_Library", sceKernelGetModuleIdByAddress(0x09E9B800));
	
	/* sceNet_Library */
    kill_module("sceNet_Library", sceKernelGetModuleIdByAddress(0x09E87800));
	
	/* sceMpeg_library */
    kill_module("sceMpeg_library", sceKernelGetModuleIdByAddress(0x09E7B800));
	
	/* sceATRAC3plus_Library */
    kill_module("sceATRAC3plus_Library", sceKernelGetModuleIdByAddress(0x09E73800));
	
	/* Labo */
#ifdef UNLOAD_MODULE
    kill_module("Labo", sceKernelGetModuleIdByAddress(0x08804000));
#endif

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

        // Find max lineare size available
        x = sceKernelMaxFreeMemSize();
        if (!(x)) break;

        // Allocate ram
        blocks[count] = sceKernelAllocPartitionMemory(2, "ValentineFreeMemMalloc", PSP_SMEM_Low, x, NULL);
        if (!(blocks[count]))
        {
            LOGSTR0("Discrepency between  sceKernelMaxFreeMemSize and sceKernelTotalFreeSize, return value will be approximate\n");
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
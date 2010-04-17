#include "sdk.h"
#include "eloader.h"
#include "thread.h"
#include "debug.h"
#include "modmgr.h"

/* Find a FPL by name */
SceUID find_fpl(const char *name) {
	SceUID readbuf[256];
	int idcount;
	SceKernelFplInfo info;
	
	/*
	#ifdef DEBUG
		SceUID fd;
	#endif
	
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GET ID LIST ", strlen(" GET ID LIST "));
			sceIoClose(fd);
		}
	#endif
	*/
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Fpl, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
	
	/*
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GOT ID LIST, COUNT ", strlen(" GOT ID LIST, COUNT "));
			sceIoWrite(fd, &idcount, sizeof(idcount));
			sceIoClose(fd);
		}
	#endif
	*/

	for(info.size=sizeof(info);idcount>0;idcount--)
	{
		
		if(sceKernelReferFplStatus(readbuf[idcount-1], &info) < 0)
			return -1;
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

/* Free memory allocated by Game */
/* Warning: MUST NOT be called from Game main thread */
void free_game_memory()
{
    DEBUG_PRINT(" ENTER FREE MEMORY ", NULL, 0);
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
			int res = sceKernelTerminateDeleteThread(id);
            if (res < 0) {
                DebugPrint("  Cannot Terminate thread, probably syscall failure");
                DEBUG_PRINT(" CANNOT TERMINATE: ", threads[i], strlen(threads[i]) + 1);
                DEBUG_PRINT(" ERROR: ", &res, sizeof(int));
                success = 0;
            } 
			else 
			{
                sceKernelDelayThread(100);
            }
		} 
		else 
		{
            DebugPrint("  Cannot find thread, probably syscall failure");
            DEBUG_PRINT(" CANNOT FIND THREAD TO DELETE ", threads[i], strlen(threads[i]) + 1);
			DEBUG_PRINT(" ERROR: ", &id, sizeof(id));
            success = 0;
        }
	}

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

	/* Free memory partition created by the GAME (UserSbrk) */
	sceKernelFreePartitionMemory(*((SceUID *) MEMORY_PARTITION_POINTER));
	
	//sceKernelDelayThread(100000);

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

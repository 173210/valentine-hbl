#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "malloc.h"
#include "hook.h"
#include "settings.h"
#include "utils.h"
#include "md5.h"
#include "memory.h"
#include "resolve.h"
#include "globals.h"
#include "syscall.h"
#include <exploit_config.h>

//Note: most of the Threads monitoring code largely inspired (a.k.a. copy/pasted) by Noobz-Fanjita-Ditlew. Thanks guys!

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() calling setup_hook()


//Checks if the homebrew should return to xmb on exit
// yes if user specified it in the cfg file AND it is not the menu
int force_return_to_xmb()
{
    tGlobals * g = get_globals();
    unsigned int i = g->mod_table.num_loaded_mod;
    if (!g->return_to_xmb_on_exit) return 0;
    if (strcmp(g->mod_table.table[i].path, g->menupath) == 0) return 0;
    return 1;
}

#ifdef FORCE_HARDCODED_VRAM_SIZE
// Hardcode edram size if the function is not available
u32 _hook_sceGeEdramGetSize() {
    return 0x00200000;
}
#endif

//
// Thread Hooks
//

// Forward declarations (functions used before they are defined lower down the file)
int _hook_sceAudioChRelease(int channel);
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq);
SceUID sceIoDopen_Vita(const char *dirname);
u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context* ctx);


// Thread hooks original code thanks to Noobz & Fanjita, adapted by wololo
/*****************************************************************************/
/* Special exitThread handling:                                              */
/*                                                                           */
/*   Delete the thread from the list of tracked threads, then pass on the    */
/*   call.                                                                   */
/*                                                                           */
/* See also hookExitDeleteThread below                                       */
/*****************************************************************************/
int _hook_sceKernelExitThread(int status)
{
	u32 i, ii;
	int lfoundit = 0;
	int lthreadid = sceKernelGetThreadId();
    
    tGlobals * g = get_globals();

	LOGSTR1("Enter hookExitThread : %08lX\n", lthreadid);

	WAIT_SEMA(g->thrSema, 1, 0);

	for (ii = 0; ii < g->numRunThreads; ii++)
	{
		if (g->runningThreads[ii] == lthreadid)
		{
			lfoundit = 1;
			g->numRunThreads --;
			LOGSTR1("Running threads: %d\n", g->numRunThreads);
		}

		if ((lfoundit) && (ii <= (SIZE_THREAD_TRACKING_ARRAY - 2)))
		{
			g->runningThreads[ii] = g->runningThreads[ii+1];
		}
	}

	g->runningThreads[g->numRunThreads] = 0;

    
#ifdef DELETE_EXIT_THREADS
	LOGSTR1("Num exit thr: %08lX\n", g->numExitThreads);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (g->numExitThreads < SIZE_THREAD_TRACKING_ARRAY)
	{
		LOGSTR0("Set array\n");
		g->exitedThreads[g->numExitThreads] = lthreadid;
		g->numExitThreads++;

		LOGSTR1("Exited threads: %d\n", g->numExitThreads);
	}
#endif    
    
#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i=0;i<8;i++)
	{
		WAIT_SEMA(g->audioSema, 1, 0);
		if (g->audio_threads[i] == lthreadid)
		{
			_hook_sceAudioChRelease(i);
		}
		sceKernelSignalSema(g->audioSema, 1);
	}
#endif
    
	sceKernelSignalSema(g->thrSema, 1);

	LOGSTR0("Exit hookExitThread\n");

	return (sceKernelExitThread(status));
}

/*****************************************************************************/
/* Special exitThread handling:                                              */
/*                                                                           */
/*   Delete the thread from the list of tracked threads, then pass on the    */
/*   call.                                                                   */
/*                                                                           */
/* See also hookExitThread above                                             */
/*****************************************************************************/
int _hook_sceKernelExitDeleteThread(int status)
{
	u32 i, ii;
	int lfoundit = 0;
	int lthreadid = sceKernelGetThreadId();
    
    tGlobals * g = get_globals();

	LOGSTR1("Enter hookExitDeleteThread : %08lX\n", lthreadid);

	WAIT_SEMA(g->thrSema, 1, 0);

	for (ii = 0; ii < g->numRunThreads; ii++)
	{
		if (g->runningThreads[ii] == lthreadid)
		{
			lfoundit = 1;
			g->numRunThreads --;
			LOGSTR1("Running threads: %d\n", g->numRunThreads);
		}

		if ((lfoundit) && (ii <= (SIZE_THREAD_TRACKING_ARRAY - 2)))
		{
			g->runningThreads[ii] = g->runningThreads[ii+1];
		}
	}

	g->runningThreads[g->numRunThreads] = 0;



	LOGSTR0("Exit hookExitDeleteThread\n");

#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i=0;i<8;i++)
	{
		WAIT_SEMA(g->audioSema, 1, 0);
		if (g->audio_threads[i] == lthreadid)
		{
			_hook_sceAudioChRelease(i);
		}
		sceKernelSignalSema(g->audioSema, 1);
	}
#endif   
    
	//return (sceKernelExitDeleteThread(status));
	// (Patapon does not import this function)
	// But modules on p5 do.
    
#ifdef DELETE_EXIT_THREADS
	LOGSTR1("Num exit thr: %08lX\n", g->numExitThreads);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (g->numExitThreads < SIZE_THREAD_TRACKING_ARRAY)
	{
		LOGSTR0("Set array\n");
		g->exitedThreads[g->numExitThreads] = lthreadid;
		g->numExitThreads++;

		LOGSTR1("Exited threads: %d\n", g->numExitThreads);
	}
#endif       
    
	sceKernelSignalSema(g->thrSema, 1);    
    return (sceKernelExitDeleteThread(status));

}


/*****************************************************************************/
/* Special createThread handling:                                            */
/*                                                                           */
/*   Pass on the call                                                        */
/*   Add the thread to the list of pending threads.                          */
/*****************************************************************************/
SceUID _hook_sceKernelCreateThread(const char *name, void * entry,
        int initPriority, int stackSize, SceUInt attr,
        SceKernelThreadOptParam *option)
{

    tGlobals * g = get_globals();

	SceUID lreturn = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);

    if (lreturn < 0)
    {
        return lreturn;
    }
    

    LOGSTR1("API returned %08lX\n", lreturn);

    /*************************************************************************/
    /* Add to pending list                                                   */
    /*************************************************************************/
    WAIT_SEMA(g->thrSema, 1, 0);
    if (g->numPendThreads < SIZE_THREAD_TRACKING_ARRAY)
    {
      LOGSTR0("Set array\n");
      g->pendingThreads[g->numPendThreads] = lreturn;
      g->numPendThreads++;

      LOGSTR1("Pending threads: %d\n", g->numPendThreads);
    }

    sceKernelSignalSema(g->thrSema, 1);
    return(lreturn);

}

/*****************************************************************************/
/* Special startThread handling:                                             */
/*                                                                           */
/*   Remove the thread from the list of pending threads.                     */
/*   Add the thread to the list of tracked threads, then pass on the call.   */
/*****************************************************************************/
int _hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
  u32 ii;
  int lfoundit = 0;
    tGlobals * g = get_globals();
  
  LOGSTR1("Enter hookRunThread: %08lX\n", thid);

  WAIT_SEMA(g->thrSema, 1, 0);

  LOGSTR1("Number of pending threads: %08lX\n", g->numPendThreads);

  /***************************************************************************/
  /* Remove from pending list                                                */
  /***************************************************************************/
  for (ii = 0; ii < g->numPendThreads; ii++)
  {
    if (g->pendingThreads[ii] == thid)
    {
      lfoundit = 1;
      g->numPendThreads --;
      LOGSTR1("Pending threads: %d\n", g->numPendThreads);
    }

    if ((lfoundit) && (ii <= (SIZE_THREAD_TRACKING_ARRAY - 2)))
    {
      g->pendingThreads[ii] = g->pendingThreads[ii+1];
    }
  }

	if (lfoundit)
	{
		g->pendingThreads[g->numPendThreads] = 0;

		/***************************************************************************/
		/* Add to running list                                                     */
		/***************************************************************************/
		LOGSTR1("Number of running threads: %08lX\n", g->numRunThreads);
		if (g->numRunThreads < SIZE_THREAD_TRACKING_ARRAY)
		{
			g->runningThreads[g->numRunThreads] = thid;
			g->numRunThreads++;
				LOGSTR1("Running threads: %d\n", g->numRunThreads);
		}
	}

  sceKernelSignalSema(g->thrSema, 1);


  LOGSTR1("Exit hookRunThread: %08lX\n", thid);

  return sceKernelStartThread(thid, arglen, argp);

}



//Cleans up Threads and allocated Ram
void threads_cleanup()
{

    tGlobals * g = get_globals();
    int lthisthread = sceKernelGetThreadId();
    u32 i;
    LOGSTR0("Threads cleanup\n");
    WAIT_SEMA(g->thrSema, 1, 0);
#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
    LOGSTR0("cleaning audio threads\n");
	for (i=0;i<8;i++)
	{
		//WAIT_SEMA(g->audioSema, 1, 0);
		_hook_sceAudioChRelease(i);
		//sceKernelSignalSema(g->audioSema, 1);
	}
#endif    
    
    LOGSTR0("Running Threads cleanup\n");
    while (g->numRunThreads > 0)
    {
        LOGSTR1("%d running threads remain\n", g->numRunThreads);
        if (g->runningThreads[g->numRunThreads - 1] != lthisthread)
        {
            LOGSTR1("Kill thread ID %08lX\n", g->runningThreads[g->numRunThreads - 1]);
            kill_thread(g->runningThreads[g->numRunThreads - 1]);
        }
        else
        {
        LOGSTR0("Not killing myself - yet\n");
        }
        g->numRunThreads --;
    }

  
    LOGSTR0("Pending Threads cleanup\n");
    while (g->numPendThreads > 0)
    {
        LOGSTR1("%d pending threads remain\n", g->numPendThreads);
        /*************************************************************************/
        /* This test shouldn't really apply to pending threads, but do it        */
        /* anyway                                                                */
        /*************************************************************************/
        if (g->pendingThreads[g->numPendThreads - 1] != lthisthread)
        {
            LOGSTR1("Kill thread ID %08lX\n", g->pendingThreads[g->numPendThreads - 1]);
            sceKernelDeleteThread(g->pendingThreads[g->numPendThreads - 1]);
        }
        else
        {
            LOGSTR0("Not killing myself - yet\n");
        }
        g->numPendThreads --;
    }
      
#ifdef DELETE_EXIT_THREADS
    LOGSTR0("Sleeping Threads cleanup\n");
    /***************************************************************************/
    /* Delete the threads that exitted but haven't been deleted yet            */
    /***************************************************************************/
    while (g->numExitThreads > 0)
    {
        LOGSTR1("%d exited threads remain\n", g->numExitThreads);
        if (g->exitedThreads[g->numExitThreads - 1] != lthisthread)
        {
            LOGSTR1("Delete thread ID %08lX\n", g->exitedThreads[g->numExitThreads - 1]);
            sceKernelDeleteThread(g->exitedThreads[g->numExitThreads - 1]);
        }
        else
        {
            LOGSTR0("Not killing myself - yet\n");
        }
        g->numExitThreads --;
    }
#endif  

    sceKernelSignalSema(g->thrSema, 1);
    LOGSTR0("Threads cleanup Done\n");
}

//
// File I/O manager Hooks
//
// returns 1 if a string is an absolute file path, 0 otherwise
int path_is_absolute(const char * file)
{
    int i;
    for (i = 0; i < strlen(file) && i < 9; ++i)
        if(file[i] == ':')
            return 1;

    return 0;
}

// converts a relative path to an absolute one based on cwd
// it is the responsibility of the caller to
// 1 - ensure that the passed path is actually relative
// 2 - delete the returned char *
char * relative_to_absolute(const char * file)
{
    tGlobals * g = get_globals();
    if (!g->module_chdir)
    {
        char * buf = malloc_hbl(strlen(file) + 1);
        strcpy(buf, file);
        return buf;
    }
#ifdef VITA_DIR_FIX
    else if (!strcmp(".", file))
    {
        char * buf = malloc_hbl(strlen(g->module_chdir) + 1);
        strcpy(buf, g->module_chdir);
        return buf;
    }
    else if (!strcmp("..", file))
    {
		char *ret;
		
		if ((ret = strrchr(g->module_chdir, '/')) != NULL && ret[-1] != ':')
		{
		    ret[0] = 0;
		}
		
        char * buf = malloc_hbl(strlen(g->module_chdir) + 1);
        strcpy(buf, g->module_chdir);
        return buf;
    }
#endif

    int len = strlen(g->module_chdir);
    
    char * buf = malloc_hbl( len +  1 +  strlen(file) + 1);

    strcpy(buf, g->module_chdir);
    if(buf[len-1] !='/')
        strcat(buf, "/");
        
    strcat(buf,file);
    
    return buf;
}


//useful ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoOpenForChDirFailure(const char *file, int flags, SceMode mode)
{
    tGlobals * g = get_globals();
    if (g->chdir_ok || path_is_absolute(file))
        return sceIoOpen(file, flags, mode);
        
    char * buf = relative_to_absolute(file);
    LOGSTR2("sceIoOpen override: %s become %s\n", (u32)file, (u32)buf);
    SceUID ret = sceIoOpen(buf, flags, mode);
    free(buf);
    return ret;
}


SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode)
{
    tGlobals * g = get_globals();

    WAIT_SEMA(g->ioSema, 1, 0);
	SceUID result = _hook_sceIoOpenForChDirFailure(file, flags, mode);

	// Add to tracked files array if files was opened successfully
	if (result > -1)
	{
		if (g->numOpenFiles < SIZE_IO_TRACKING_ARRAY - 1)
		{
			int i;
			for (i = 0; i < SIZE_IO_TRACKING_ARRAY; i++)
			{
				if (g->openFiles[i] == 0)
				{
					g->openFiles[i] = result;
					g->numOpenFiles++;
					break;
				}
			}
		}
		else
		{
			LOGSTR0("WARNING: file list full, cannot add newly opened file\n");
		}
	}

	sceKernelSignalSema(g->ioSema, 1);
	//LOGSTR4("_hook_sceIoOpen(%s, %d, %d) = 0x%08lX\n", (u32)file, (u32)flags, (u32)mode, (u32)result);
	return result;
}


int _hook_sceIoClose(SceUID fd)
{
    tGlobals * g = get_globals();

	WAIT_SEMA(g->ioSema, 1, 0);
	SceUID result = sceIoClose(fd);

	// Remove from tracked files array if closing was successfull
	if (result > -1)
	{
		int i;
		for (i = 0; i < SIZE_IO_TRACKING_ARRAY; i++)
		{
			if (g->openFiles[i] == fd)
			{
				g->openFiles[i] = 0;
				g->numOpenFiles--;
				break;
			}
		}
	}

	sceKernelSignalSema(g->ioSema, 1);
	//LOGSTR2("_hook_sceIoClose(0x%08lX) = 0x%08lX\n", (u32)fd, (u32)result);
	return result;
}


// Close all files that remained open after the homebrew quits
void files_cleanup()
{
	LOGSTR0("Files Cleanup\n");
    tGlobals * g = get_globals();

	WAIT_SEMA(g->ioSema, 1, 0);
	int i;
	for (i = 0; i < SIZE_IO_TRACKING_ARRAY; i++)
	{
		if (g->openFiles[i] != 0)
		{
			sceIoClose(g->openFiles[i]);
			LOGSTR1("closing file UID 0x%08lX\n", (u32)g->openFiles[i]);
			g->openFiles[i] = 0;
		}
	}

	g->numOpenFiles = 0;
	sceKernelSignalSema(g->ioSema, 1);
	LOGSTR0("Files Cleanup Done\n");
}




//Resolve the call for a function and runs it without parameters
// This is quite ugly, so if you find a better way of doing this I'm all ears
int run_nid (u32 nid){
    tGlobals * g = get_globals();
    // Is it known by HBL?
    int ret = get_nid_index(nid);
    LOGSTR1("NID: 0x%08lX\n", nid);
    if (ret <= 0)
    {
        LOGSTR1("Unknown NID: 0x%08lX\n", nid);
        return 0;
    }
    u32 syscall = g->nid_table.table[ret].call;

    if (!syscall) 
    {
        LOGSTR1("No syscall for NID: 0x%08lX\n", nid);
        return 0;
    }
    
    u32 cur_stub[2];
    resolve_call(cur_stub, syscall);
    CLEAR_CACHE;
	
	// This debug line must also be present in the release build!
	// Otherwise the PSP will freeze and turn off when jumping to function().
    logstr1("call is: 0x%08lX\n", cur_stub[0]);

    void (*function)() = (void *)(&cur_stub);
    function();
    return 1;
}

//Force close Net modules
void net_term()
{
    //Call the closing functions only if the matching library is loaded
    
    if (get_library_index("sceNetApctl") >= 0)
    {
        run_nid(0xB3EDD0EC); //sceNetApctlTerm();
    }
    
    if (get_library_index("sceNetResolver") >= 0)
    {        
        run_nid(0x6138194A); //sceNetResolverTerm();
    }    
    if (get_library_index("sceNetInet") >= 0)
    {   
        run_nid(0xA9ED66B9); //sceNetInetTerm();
    }
    
    if (get_library_index("sceNet") >= 0)
    {   
        run_nid(0x281928A9); //sceNetTerm();
    }
}

// Release the kernel audio channel
void audio_term()
{
	tGlobals * g = get_globals();

	if (g->syscalls_known)
	{
		// sceAudioSRCChRelease
		if (!run_nid(0x5C37C0AE))
		{
			estimate_syscall("sceAudio", 0x5C37C0AE, FROM_LOWEST);
			run_nid(0x5C37C0AE);
		}
	}
}
	

void ram_cleanup() 
{
    LOGSTR0("Ram Cleanup\n");
    tGlobals * g = get_globals();
    u32 ii;
    WAIT_SEMA(g->memSema, 1, 0);
    for (ii=0; ii < g->osAllocNum; ii++)
    {
        sceKernelFreePartitionMemory(g->osAllocs[ii]);
    }
    g->osAllocNum = 0;
    sceKernelSignalSema(g->memSema, 1);
    LOGSTR0("Ram Cleanup Done\n");  
}


// Release all subinterrupt handler
void subinterrupthandler_cleanup()
{
#ifdef SUB_INTR_HANDLER_CLEANUP
    LOGSTR0("Subinterrupthandler Cleanup\n");
	int i, j;
	
	for (i = 0; i <= 66; i++) // 66 is the highest value of enum PspInterrupts
	{
		for (j = 0; j <= 30; j++) // 30 is the highest value of enum PspSubInterrupts 
		{
			if (sceKernelReleaseSubIntrHandler(i, j) > -1)
			{
				LOGSTR2("Subinterrupt handler released for %d, %d\n", i, j);
			}
		}
	}
    LOGSTR0("Subinterrupthandler Cleanup Done\n");
#endif
}


void exit_everything_but_me()
{
    net_term();
    audio_term();
	subinterrupthandler_cleanup();
    threads_cleanup();
    ram_cleanup();
	files_cleanup();
}

//To return to the menu instead of exiting the game
void  _hook_sceKernelExitGame() 
{
    /***************************************************************************/
    /* Call any exit callback first. Or not, it should only be called when     */
	/* quitting through the HOME exit screen.                                  */
    /***************************************************************************/
/*
	tGlobals * g = get_globals();
    if (!g->calledexitcb && g->exitcallback)
    {
        LOGSTR1("Call exit CB: %08lX\n", (u32) g->exitcallback);
        g->calledexitcb = 1;
        g->exitcallback(0,0,NULL);
    }
*/
	LOGSTR0("_hook_sceKernelExitGame called\n");
    exit_everything_but_me();
    sceKernelExitDeleteThread(0);
}

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
    tGlobals * g = get_globals();
    LOGSTR5("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08lX\n", partitionid, (u32)name, type, size, (u32)addr);
    WAIT_SEMA(g->memSema, 1, 0);

	// Try to allocate the requested memory. If the allocation fails due to an insufficient
	// amount of free memory try again with 10kB less until the allocation succeeds.
	// Don't allow to go under 80 % of the initial amount of memory.
	SceUID uid;
	SceSize original_size = size;
	do 
	{
		uid = sceKernelAllocPartitionMemory(partitionid,name, type, size, addr);
		if (uid <= 0)
		{
			// Allocation failed, check if we can go lower for another try
			if ((size > 10000) && ((size - 10000) > original_size - (original_size / 5)))
			{
				// Try again with 10kB less
				size -= 10000;
			}
			else
			{
				// Limit reached, break out of loop
				break;
			}
		}
	}
    while (uid <= 0);

	LOGSTR3("-> final allocation made for %d of %d requested bytes with result 0x%08lX\n", size, original_size, uid);

	if (uid > 0)
	{
        /***********************************************************************/
        /* Succeeded OS alloc.  Record the block ID in the tracking list.      */
        /* (Don't worry if there's no space to record it, we'll just have to   */
        /* leak it).                                                           */
        /***********************************************************************/
        if (g->osAllocNum < MAX_OS_ALLOCS)
        {
            g->osAllocs[g->osAllocNum] = uid;
            g->osAllocNum ++;
            LOGSTR1("Num tracked OS blocks now: %08lX\n", g->osAllocNum);
        }
        else
        {
            LOGSTR0("!!! EXCEEDED OS ALLOC TRACKING ARRAY\n");
        }
    }
    sceKernelSignalSema(g->memSema, 1);
    return uid;
}

int _hook_sceKernelFreePartitionMemory(SceUID uid)
  {
    tGlobals * g = get_globals();
    u32 ii;
    int lfound = 0;
    WAIT_SEMA(g->memSema, 1, 0);
    int lret = sceKernelFreePartitionMemory(uid);

    /*************************************************************************/
    /* Remove UID from list of alloc'd mem.                                  */
    /*************************************************************************/
    for (ii = 0; ii < g->osAllocNum; ii++)
    {
      if (g->osAllocs[ii] == uid)
      {
        lfound = 1;
      }

      if (lfound && (ii < (MAX_OS_ALLOCS-2)))
      {
        g->osAllocs[ii] = g->osAllocs[ii+1];
      }
    }

    if (lfound)
    {
        g->osAllocNum --;
    }
    sceKernelSignalSema(g->memSema, 1);
    return lret;
    
  }

  
/*****************************************************************************/
/* Create a callback.  Record the details so that we can refer to it if      */
/* need be.                                                                  */
/*****************************************************************************/
int _hook_sceKernelCreateCallback(const char *name, SceKernelCallbackFunction func, void *arg)
{
    tGlobals * g = get_globals();
    int lrc = sceKernelCreateCallback(name, func, arg);

    LOGSTR1("Enter createcallback: %s\n", (u32)name);

    WAIT_SEMA(g->cbSema, 1, 0);
    if (g->callbackcount < MAX_CALLBACKS)
    {
        g->callbackids[g->callbackcount] = lrc;
        g->callbackfuncs[g->callbackcount] = func;
        g->callbackcount ++;
    }
    sceKernelSignalSema(g->cbSema, 1);

    LOGSTR2("Exit createcallback: %s ID: %08lX\n", (u32) name, lrc);

    return(lrc);
}

/*****************************************************************************/
/* Register an ExitGame handler                                              */
/*****************************************************************************/
int _hook_sceKernelRegisterExitCallback(int cbid)
{
    tGlobals * g = get_globals();
    int i;

    LOGSTR1("Enter registerexitCB: %08lX\n", cbid);

    WAIT_SEMA(g->cbSema, 1, 0);
    for (i = 0; i < g->callbackcount; i++)
    {
        if (g->callbackids[i] == cbid)
        {
            LOGSTR1("Found matching CB, func: %08lX\n", (u32) g->callbackfuncs[i]);
            g->exitcallback = g->callbackfuncs[i];
            break;
        }
    }
    sceKernelSignalSema(g->cbSema, 1);

    LOGSTR1("Exit registerexitCB: %08lX\n",cbid);

    return(0);
}  
  

//#############
// RTC
// #############

#define ONE_SECOND_TICK 1000000
#define ONE_DAY_TICK 24 * 3600 * ONE_SECOND_TICK

// always returns 1000000
// based on http://forums.ps2dev.org/viewtopic.php?t=4821
u32 _hook_sceRtcGetTickResolution() 
{
    return ONE_SECOND_TICK;
}

#ifdef HOOK_sceRtcGetCurrentClock_WITH_sceRtcGetCurrentClockLocalTime 
int _hook_sceRtcGetCurrentClock  (pspTime  *time, int UNUSED(tz))
{
    //todo deal with the timezone...
    return sceRtcGetCurrentClockLocalTime(time);
}
#endif


int _hook_sceRtcSetTick (pspTime  *time, const u64  *t)
{
//super approximate, let's hope people call this only for display purposes
    u32 tick = *t; //get the lower 32 bits
    time->year = 2097; //I know...
    tick = tick % (365 * (u32)ONE_DAY_TICK);
    time->month = tick / (30 * (u32)ONE_DAY_TICK);
    tick = tick % (30 * (u32)ONE_DAY_TICK);
    time->day = tick / ONE_DAY_TICK;
    tick = tick % ONE_DAY_TICK;
    time->hour = tick / (3600 * (u32)ONE_SECOND_TICK);
    tick = tick % (3600 * (u32)ONE_SECOND_TICK);
    time->minutes = tick / (60 * ONE_SECOND_TICK);
    tick = tick % (60 * ONE_SECOND_TICK);
    time->seconds = tick / (ONE_SECOND_TICK);
    tick = tick % (ONE_SECOND_TICK);    
    time->microseconds = tick;
    return 0;
}

//dirty, assume we're utc
int _hook_sceRtcConvertUtcToLocalTime  (const u64  *tickUTC, u64  *tickLocal)
{
  *tickLocal = *tickUTC;
  return 1;
}

int _hook_sceRtcGetTick  (const pspTime  *time, u64  *tick)
{
    u64 seconds = 
        (u64) time->day * 24 * 3600
        + (u64) time->hour * 3600 
        + (u64) time->minutes * 60
        + (u64) time->seconds; 
    *tick = seconds * 1000000   + (u64) time->microseconds;
    return 0;
}

#ifdef HOOK_sceRtcGetCurrentTick 
int _hook_sceRtcGetCurrentTick(u64 * tick)
{   	
    pspTime time;
    sceRtcGetCurrentClockLocalTime(&time);
    return _hook_sceRtcGetTick (&time, tick);
}
#endif

//based on http://forums.ps2dev.org/viewtopic.php?t=12490
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence) 
{
    return sceIoLseek(fd, offset, whence); 
}


//audio hooks

int _hook_sceAudioChReserve(int channel, int samplecount, int format)
{
	int lreturn = sceAudioChReserve(channel,samplecount,format);

#ifdef MONITOR_AUDIO_THREADS
    tGlobals * g = get_globals();
	if (lreturn >= 0)
	{
	    if (lreturn >= 8)
		{
			LOGSTR1("AudioChReserve return out of range: %08lX\n", lreturn);
			//waitForButtons();
		}
		else
		{
			WAIT_SEMA(g->audioSema, 1, 0);
			g->audio_threads[lreturn] = sceKernelGetThreadId();
			sceKernelSignalSema(g->audioSema, 1);
		}
	}
#endif

	return lreturn;
}

// see commented code in http://forums.ps2dev.org/viewtopic.php?p=81473
// //   int audio_channel = sceAudioChReserve(0, 2048 , PSP_AUDIO_FORMAT_STEREO);
//   sceAudioSRCChReserve(2048, wma_samplerate, 2); 
int _hook_sceAudioSRCChReserve (int samplecount, int UNUSED(freq), int channels)
{
    
    int format = PSP_AUDIO_FORMAT_STEREO;
    if (channels == 1) 
        format = PSP_AUDIO_FORMAT_MONO;
    //samplecount needs to be 64 bytes aligned, see http://code.google.com/p/valentine-hbl/issues/detail?id=270    
    int result =  _hook_sceAudioChReserve (PSP_AUDIO_NEXT_CHANNEL, PSP_AUDIO_SAMPLE_ALIGN(samplecount), format);
    if (result >=0 && result < 8)
    {
        tGlobals * g = get_globals();
        g->curr_channel_id = result;
    }
    return result;
} 

//         sceAudioSRCOutputBlocking(PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
//   //      sceAudioOutputBlocking(audio_channel, PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]); 
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf)
{
    tGlobals * g = get_globals();
    return sceAudioOutputPannedBlocking(g->curr_channel_id,vol, vol, buf);
}

#ifdef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
//quite straightforward
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(channel,vol, vol, buf);
}
#endif

// Ditlew
int _hook_sceAudioChRelease(int channel)
{
	int lreturn;

	lreturn = sceAudioChRelease( channel );

#ifdef MONITOR_AUDIO_THREADS
	if (lreturn >= 0)
	{
        tGlobals * g = get_globals();
		WAIT_SEMA(g->audioSema, 1, 0);  
		g->audio_threads[channel] = 0;
		sceKernelSignalSema(g->audioSema, 1);
	}
#endif
	return lreturn;
}

//
int _hook_sceAudioSRCChRelease()
{
    tGlobals * g = get_globals();
    if (g->curr_channel_id < 0 || g->curr_channel_id > 7)
    {
        LOGSTR0("FATAL: curr_channel_id < 0 in _hook_sceAudioSRCChRelease\n");
        return -1;
    } 
    int result = _hook_sceAudioChRelease(g->curr_channel_id);
    g->curr_channel_id--;
    return result;
}  

int test_sceIoChdir()
{
#ifndef CHDIR_CRASH
    sceIoChdir(HBL_ROOT);
    if (file_exists(HBL_BIN))
        return 1;
#endif
    return 0;
}

//hook this ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoDopen(const char *dirname)   
{
    if (path_is_absolute(dirname))
        return
#ifdef VITA_DIR_FIX
            sceIoDopen_Vita(dirname);
#else
            sceIoDopen(dirname);
#endif

    char * buf = relative_to_absolute(dirname);
#ifdef VITA_DIR_FIX
    SceUID ret = sceIoDopen_Vita(buf);
#else
    SceUID ret = sceIoDopen(buf);
#endif
    free(buf);
    return ret;
}


#ifdef VITA_DIR_FIX
// Adds Vita's missing "." / ".." entries

SceUID sceIoDopen_Vita(const char *dirname)   
{
    tGlobals * g = get_globals();
	LOGSTR0("sceIoDopen_Vita start\n");
    SceUID id = sceIoDopen(dirname);
	
	if (id >= 0)
	{
		int dirLen = strlen(dirname);
		
		// If directory isn't "ms0:" or "ms0:/", add "." & ".." entries
		if (dirname[dirLen-1] != ':' && dirname[dirLen-2] != ':')
		{
			int i = 0;
			
			while ( i<g->directoryLen && (g->directoryFix[i][0] >= 0) )
				++i;
				
			if (i < MAX_OPEN_DIR_VITA)
			{
				g->directoryFix[i][0] = id;
				g->directoryFix[i][1] = 2;
				
				if (i == g->directoryLen)	++(g->directoryLen);
			}
		}
		
	}
	
	
	LOGSTR0("sceIoDopen_Vita Done\n");
    return id;
}


int sceIoDread_Vita(SceUID id, SceIoDirent *dir)
{
    tGlobals * g = get_globals();
	LOGSTR0("sceIoDread_Vita start\n");
	int i = 0, errCode = 1;
	while ( i<g->directoryLen && id != g->directoryFix[i][0] )
		++i;
	
	
	if (id == g->directoryFix[i][0])
	{
        memset(dir, 0, sizeof(SceIoDirent)); 
		if (g->directoryFix[i][1] == 1)
		{
			strcpy(dir->d_name,"..");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		else if (g->directoryFix[i][1] == 2)
		{
			strcpy(dir->d_name,".");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		g->directoryFix[i][1]--;
		
		if (g->directoryFix[i][1] == 0)
		{
			g->directoryFix[i][0] = -1;
			g->directoryFix[i][1] = -1;
		}
	}
	else
	{
		errCode = sceIoDread(id, dir);
	}
	
	LOGSTR0("sceIoDread_Vita Done\n");
	return errCode;
}

int sceIoDclose_Vita(SceUID id)
{
    tGlobals * g = get_globals();
	LOGSTR0("sceIoDclose_Vita start\n");
	int i = 0;
	while ( i<g->directoryLen && id != g->directoryFix[i][0] )
		++i;
	
	
	if (i<g->directoryLen && id == g->directoryFix[i][0])
	{
			g->directoryFix[i][0] = -1;
			g->directoryFix[i][1] = -1;
	}
	
	LOGSTR0("sceIoDclose_Vita Done\n");
	return sceIoDclose(id);
}
#endif


//hook this ONLY if test_sceIoChdir() fails!
int _hook_sceIoChdir(const char *dirname)   
{
	LOGSTR0("_hook_sceIoChdir start\n");
    tGlobals * g = get_globals();
    // save chDir into global variable
    if (path_is_absolute(dirname))
    {
        if (g->module_chdir)
        {
            free(g->module_chdir);
            g->module_chdir = 0;
        }  
        g->module_chdir = relative_to_absolute(dirname);
    }
    else
    {
        char * result = relative_to_absolute(dirname);
         if (g->module_chdir)
        {
            free(g->module_chdir);
        }
        g->module_chdir = result;
    }
    
    if (!g->module_chdir)
    {
        return -1;
    }
	LOGSTR2("_hook_sceIoChdir: %s becomes %s\n", (u32) dirname, (u32) g->module_chdir);
    
    return 0;
}


// see http://forums.ps2dev.org/viewtopic.php?p=52329
int _hook_scePowerGetCpuClockFrequencyInt()
{
    tGlobals * g = get_globals();
    if (!g->cpufreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return g->cpufreq;
}

int _hook_scePowerGetBusClockFrequency() {
    tGlobals * g = get_globals();
    if (!g->busfreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return g->busfreq;
}


//Alias, see http://forums.ps2dev.org/viewtopic.php?t=11294
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq)
{
    int ret = scePower_EBD177D6(pllfreq, cpufreq, busfreq);
    if (ret >= 0)
    {
        tGlobals * g = get_globals();
        g->pllfreq = pllfreq;
        g->cpufreq = cpufreq;
        g->busfreq = busfreq;
    }
    
    return ret;
}

//Dirty
int _hook_scePowerGetBatteryLifeTime()
{
    return 60;
}

//Dirty
int _hook_scePowerGetBatteryLifePercent()
{
    return 50;
}   

// http://forums.ps2dev.org/viewtopic.php?p=43495
int _hook_sceKernelDevkitVersion()
{
    //There has to be a more efficient way...maybe getFirmwareVersion should directly do this
    u32 version = getFirmwareVersion();
	// 0x0w0y001z <==> firmware w.yz 
    int result = 0x01000000 * (version / 100);
 	result += 0x010000 * ((version % 100) / 10);
    result += 0x0100 * (version % 10);
    result += 0x10;
	
    return result;
}

#ifdef HOOK_sceKernelSelfStopUnloadModule_WITH_ModuleMgrForUser_8F2DF740
// see http://powermgrprx.googlecode.com/svn-history/r2/trunk/include/pspmodulemgr.h
int _hook_sceKernelSelfStopUnloadModule  (int exitcode, SceSize  argsize, void *argp)
{
    int status;
    return ModuleMgrForUser_8F2DF740(exitcode, argsize, argp, &status, NULL);

}
#endif

int _hook_sceKernelGetThreadCurrentPriority()
{
    return 0x18;
    //TODO : real management of threads --> keep track of their priorities ?
}

// Sleep is not delay... can we fix this?
#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
int _hook_sceKernelSleepThreadCB() 
{
    while (1)
        sceKernelDelayThreadCB(1000000);
    return 1;
}
#endif

#ifdef HOOK_sceKernelTrySendMsgPipe_WITH_sceKernelSendMsgPipe
int _hook_sceKernelTrySendMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2)
{
    return sceKernelSendMsgPipe(uid, message, size, unk1, unk2, 0);
}
#endif

#ifdef HOOK_sceKernelReceiveMsgPipe_WITH_sceKernelTryReceiveMsgPipe
int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int UNUSED(timeout))
{
    return sceKernelTryReceiveMsgPipe(uid, message, size, unk1, unk2);
}
#endif

//
// Cache
//
#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
void _hook_sceKernelDcacheWritebackAll (void)
{
     sceKernelDcacheWritebackRange((void*)0x08800000, 0x17FFFFF);
}
#endif

#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackInvalidateRange
void _hook_sceKernelDcacheWritebackInvalidateAll (void)
{
     sceKernelDcacheWritebackInvalidateRange((void*)0x08800000, 0x17FFFFF);
}
#endif


// ###############
// UtilsForUser
// ###############

int _hook_sceKernelUtilsMd5Digest  (u8  *data, u32  size, u8  *digest)
{
    if (md5(data,size, digest))
    {
		return 1;
    }
	return -1;
}


#ifdef LOAD_MODULE
SceUID _hook_sceKernelLoadModule(const char *path, int UNUSED(flags), SceKernelLMOption * UNUSED(option))
{
	LOGSTR0("_hook_sceKernelLoadModule\n");
	LOGSTR1("Attempting to load %s\n", (u32)path);
	
	SceUID elf_file = _hook_sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (elf_file < 0)
	{
		LOGSTR2("Error 0x%08lX opening requested module %s\n", elf_file, (u32)path);
		return elf_file;
	}

	SceOff offset = 0;
	LOGSTR1("_hook_sceKernelLoadModule offset: 0x%08lX\n", offset);
	SceUID ret = load_module(elf_file, path, NULL, offset);
    sceIoClose(elf_file);
	LOGSTR1("load_module returned 0x%08lX\n", ret);

	return ret;
}

int	_hook_sceKernelStartModule(SceUID modid, SceSize UNUSED(argsize), void *UNUSED(argp), int *UNUSED(status), SceKernelSMOption *UNUSED(option))
{
	LOGSTR0("_hook_sceKernelStartModule\n");
	
	SceUID ret = start_module(modid);
	LOGSTR1("start_module returned 0x%08lX\n", ret);
	
	return ret;
}
#endif

#ifdef HOOK_UTILITY

int _hook_sceUtilityLoadModule(int id)
{
/*   if ((id == PSP_MODULE_NET_COMMON)
     || (id == PSP_MODULE_NET_INET)
     || (id == PSP_MODULE_NET_ADHOC)
     || (id == PSP_MODULE_AV_MP3)) */
	if (is_utility_loaded(id))
    {
		return 0;
    }
    return SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityLoadNetModule(int id)
{
	if ((id == PSP_NET_MODULE_COMMON)
		|| (id == PSP_NET_MODULE_INET)
		|| (id == PSP_NET_MODULE_ADHOC))
		return 0;

	return SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityLoadAvModule(int id)
{
	if (id == PSP_AV_MODULE_MP3)
		return 0;
    return SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityLoadUsbModule(int id)
{
	if(id){}; //Avoid compilation errors :P
	return SCE_KERNEL_ERROR_ERROR;
}

int _hook_sceUtilityUnloadModule(int id)
{
	if(id){}; //Avoid compilation errors :P
	return 0;
}

#endif

// A function that just returns "ok" but does nothing
// Note: in Sony's world, "0" means ok
int _hook_generic_ok()
{
    return 0;
}

// A function that return a generic error
int _hook_generic_error()
{
    return SCE_KERNEL_ERROR_ERROR;
}

#ifdef HOOK_sceKernelGetThreadId
// random sceKernelGetThreadId hook (hopefully it's not super necessary);
int _hook_sceKernelGetThreadId()
{
	// Somehow sceKernelGetThreadId isn't imported by the game,
	// and can be called from HBL but not from a homebrew
    return sceKernelGetThreadId();
}
#endif

#ifdef HOOK_sceAudioOutput2GetRestSample_WITH_sceAudioGetChannelRestLength
//	sceAudioOutput2GetRestSample lame hook (hopefully it's not super necessary)
int _hook_sceAudioOutput2GetRestSample()
{
	int sum = 0, i, res;
	
	for (i=0;i<PSP_AUDIO_CHANNEL_MAX;++i)
	{
		res = sceAudioGetChannelRestLength(i);
		if (res >=0)	sum+= res;
	}
	
	
    return sum;
}
#endif


#ifdef HOOK_mersenne_twister_rdm
u32 rdm_seed;

int _hook_sceKernelUtilsMt19937Init(SceKernelUtilsMt19937Context *ctx, u32 seed)
{
	int i;
	rdm_seed = seed;
	
    for (i = 0; i < 624; i++)
        _hook_sceKernelUtilsMt19937UInt(ctx);
	
	return 0;
}

u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context* ctx) {
	if(ctx){}; //Avoid compilation errors :P
	
	// (Bill's generator)
	rdm_seed = (((rdm_seed * 214013L + 2531011L) >> 16) & 0xFFFF);
	rdm_seed |= ((rdm_seed * 214013L + 2531011L) & 0xFFFF0000);
	
	return rdm_seed;
}
#endif


#ifdef HOOK_sceKernelTotalFreeMemSize
// return 10 MB
SceSize _hook_sceKernelTotalFreeMemSize()
{
	return sceKernelTotalFreeMemSize(); //this is the function we hardcoded in memory.c, not the official one
}
#endif


#ifdef HOOK_sceKernelReferThreadStatus
int _hook_sceKernelReferThreadStatus(SceUID thid, SceKernelThreadInfo *info)
{
	if(thid){}; //Avoid compilation errors :P

	memset(info+sizeof(SceSize), 0, info->size-sizeof(SceSize));
	
	return 0;
}
#endif



// Returns a hooked call for the given NID or zero
u32 setup_hook(u32 nid)
{
	u32 hook_call = 0;
    tGlobals * g = get_globals();
	
    //We have 2 switch blocks
    // The first one is for nids that need to be hooked in all cases
    //The second one is for firmwares that don't have perfect syscall estimation
    switch (nid) 
	{

//utility functions, we need those
        case 0x237DBD4F: // sceKernelAllocPartitionMemory
            LOGSTR0(" mem trick ");
            hook_call = MAKE_JUMP(_hook_sceKernelAllocPartitionMemory);
            break;     
            
        case 0xB6D61D02: // sceKernelFreePartitionMemory
            LOGSTR0(" mem trick ");
            hook_call = MAKE_JUMP(_hook_sceKernelFreePartitionMemory);
            break;              
            
        case 0x446D8DE6: // sceKernelCreateThread
            hook_call = MAKE_JUMP(_hook_sceKernelCreateThread);
            break;

		case 0xF475845D: // sceKernelStartThread
			hook_call = MAKE_JUMP(_hook_sceKernelStartThread);
			break;
            
 		case 0xAA73C935: // sceKernelExitThread
			hook_call = MAKE_JUMP(_hook_sceKernelExitThread);
			break;
            
 		case 0x809CE29B: // sceKernelExitDeleteThread
			hook_call = MAKE_JUMP(_hook_sceKernelExitDeleteThread);
			break;
            
        case 0x05572A5F: // sceKernelExitGame
            if (!force_return_to_xmb())
                hook_call = MAKE_JUMP(_hook_sceKernelExitGame);
            break;
        case 0xE81CAF8F: //	sceKernelCreateCallback
            if (!force_return_to_xmb())
                hook_call = MAKE_JUMP(_hook_sceKernelCreateCallback);
            break;   
        case 0x4AC57943: //	sceKernelRegisterExitCallback   
            if (!force_return_to_xmb())
                hook_call = MAKE_JUMP(_hook_sceKernelRegisterExitCallback);
            break;          
//Audio monitors
        case 0x6FC46853: //	sceAudioChRelease  
			if (!force_return_to_xmb())
                hook_call = MAKE_JUMP(_hook_sceAudioChRelease);
            break; 
        case 0x5EC81C55: //	sceAudioChReserve  
            if (!force_return_to_xmb())
                hook_call = MAKE_JUMP(_hook_sceAudioChReserve);
            break; 
            
#ifdef LOAD_MODULE
        case 0x4C25EA72: //kuKernelLoadModule --> CFW specific function? Anyways the call should fail
		case 0x977DE386: // sceKernelLoadModule
			LOGSTR0(" loadmodule trick ");
#ifdef HOOK_sceKernelLoadModule_WITH_error
            hook_call = MAKE_JUMP(_hook_generic_error);            
#else
#ifdef HOOK_sceKernelLoadModule
			hook_call = MAKE_JUMP(_hook_sceKernelLoadModule);
#else
			hook_call = MAKE_JUMP(sceKernelLoadModule);
#endif
#endif
			break;
		
		case 0x50F0C1EC: // sceKernelStartModule
			LOGSTR0(" loadmodule trick ");
			hook_call = MAKE_JUMP(_hook_sceKernelStartModule);
			break;
#endif  


#ifdef HOOK_UTILITY
		case 0xC629AF26: //sceUtilityLoadAvModule
			LOGSTR0(" Hook sceUtilityLoadAvModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadAvModule);
			break;

		case 0x0D5BC6D2: //sceUtilityLoadUsbModule		
			LOGSTR0(" Hook sceUtilityLoadUsbModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadUsbModule);
			break;

		case 0x1579a159: //sceUtilityLoadNetModule
			LOGSTR0(" Hook sceUtilityLoadNetModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadNetModule);
			break;

		case 0x2A2B3DE0: // sceUtilityLoadModule
			LOGSTR0(" Hook sceUtilityLoadModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadModule);
			break;
			
		case 0xF7D8D092: //sceUtilityUnloadAvModule
		case 0xF64910F0: //sceUtilityUnloadUsbModule
		case 0x64d50c56: //sceUtilityUnloadNetModule
		case 0xE49BFE92: // sceUtilityUnloadModule
			LOGSTR0(" Hook sceUtilityUnloadModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityUnloadModule);
			break;	
#endif            

		// Hook these to keep track of open files when the homebrew quits

		case 0x109F50BC: //	sceIoOpen
            LOGSTR0("Hook sceIoOpen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;          

		case 0x810C4BC3: //	sceIoClose
            LOGSTR0("Hook sceIoClose\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoClose);
            break;  

    }
    
    if (hook_call) 
        return hook_call;
            
    // Overrides below this point don't need to be done if we have perfect syscall estimation      
    if (g->syscalls_known)
        return 0;

#ifndef DISABLE_ADDITIONAL_HOOKS

    switch (nid) 
	{ 

// Overrides to avoid syscall estimates, those are not necessary but reduce estimate failures and improve compatibility for now
        case 0x06A70004: //	sceIoMkdir
            if (g->override_sceIoMkdir == GENERIC_SUCCESS)
            {
                LOGSTR0(" sceIoMkdir goes to void because of settings ");
                hook_call = MAKE_JUMP(_hook_generic_ok);
            }
            break; 
        
	    case 0xC8186A58:  //sceKernelUtilsMd5Digest  (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceKernelUtilsMd5Digest);
            break;
            
	    case 0x3FC9AE6A:  //sceKernelDevkitVersion   (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceKernelDevkitVersion);
            break;
            
        case 0xA291F107: // sceKernelMaxFreeMemSize (avoid syscall estimation)
            LOGSTR0(" mem trick ");
            hook_call = MAKE_JUMP(sceKernelMaxFreeMemSize);
            break;
		
        //RTC
        case 0xC41C2853: //	sceRtcGetTickResolution (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceRtcGetTickResolution);
            break;

//Define if function sceRtcGetCurrentTick is not available
#ifdef HOOK_sceRtcGetCurrentTick           
		case 0x3F7AD767: //	sceRtcGetCurrentTick (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceRtcGetCurrentTick);
            break;   
#endif
 
        case 0x7ED29E40: //	sceRtcSetTick (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceRtcSetTick);
            break;       

        case 0x6FF40ACC: //   sceRtcGetTick	 (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceRtcGetTick);
            break;       
        
        case 0x57726BC1: //	sceRtcGetDayOfWeek (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok); //always monday in my world
            break;       
        
        case 0x34885E0D: //sceRtcConvertUtcToLocalTime	 (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceRtcConvertUtcToLocalTime);
            break;       

#ifdef HOOK_sceRtcGetCurrentClock_WITH_sceRtcGetCurrentClockLocalTime        
        case 0x4CFA57B0: //sceRtcGetCurrentClock	 (avoid syscall estimation)
			if (!g->syscalls_from_p5)
				hook_call = MAKE_JUMP(_hook_sceRtcGetCurrentClock);
            break; 
#endif
            
        //others
        case 0x68963324: //	sceIoLseek32 (avoid syscall estimation)
            //based on http://forums.ps2dev.org/viewtopic.php?t=12490
			hook_call = MAKE_JUMP(_hook_sceIoLseek32);
            break;

#ifndef HOOK_sceCtrlReadBufferPositive_WITH_sceCtrlPeekBufferPositive    
        case 0x3A622550: //	sceCtrlPeekBufferPositive (avoid syscall estimation)
			if ((!g->syscalls_from_p5) && (g->override_sceCtrlPeekBufferPositive == OVERRIDE))
            {
                //based on http://forums.ps2dev.org/viewtopic.php?p=27173
                //This will be slow and should not be active for high performance programs...
                hook_call = MAKE_JUMP(sceCtrlReadBufferPositive);
            }
            break;
#endif
            
        case 0x737486F2: // scePowerSetClockFrequency   - yay, that's a pure alias :)
			hook_call = MAKE_JUMP(_hook_scePowerSetClockFrequency);
            break;  
            
        case 0x383F7BCC: // sceKernelTerminateDeleteThread  (avoid syscall estimation)
            hook_call = MAKE_JUMP(kill_thread); //TODO Take into account with thread monitors ?
            break;    

#ifdef HOOK_sceKernelSelfStopUnloadModule_WITH_ModuleMgrForUser_8F2DF740       
        case 0xD675EBB8: // sceKernelSelfStopUnloadModule (avoid syscall estimation) - not sure about this one
            hook_call = MAKE_JUMP(_hook_sceKernelSelfStopUnloadModule);
            break;               
#endif

#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
        case 0x82826F70: // sceKernelSleepThreadCB   (avoid syscall estimation)          
			hook_call = MAKE_JUMP(_hook_sceKernelSleepThreadCB);
            break; 
#endif
            
#ifdef HOOK_sceKernelTrySendMsgPipe_WITH_sceKernelSendMsgPipe
        case 0x884C9F90: //	sceKernelTrySendMsgPipe (avoid syscall estimation)  
            hook_call = MAKE_JUMP(_hook_sceKernelTrySendMsgPipe);
            break;
#endif            

#ifdef HOOK_sceKernelReceiveMsgPipe_WITH_sceKernelTryReceiveMsgPipe
        case 0x74829B76: // sceKernelReceiveMsgPipe (avoid syscall estimation)  
			hook_call = MAKE_JUMP(_hook_sceKernelReceiveMsgPipe);
            break;                 
#endif
           
        case 0x94AA61EE: // sceKernelGetThreadCurrentPriority (avoid syscall estimation)  
			hook_call = MAKE_JUMP(_hook_sceKernelGetThreadCurrentPriority);
            break; 
            
        case 0x24331850: // kuKernelGetModel ?
            hook_call = MAKE_JUMP(_hook_generic_ok);
            break; 	
            
#ifdef HOOK_POWERFUNCTIONS
        case 0xFEE03A2F: //scePowerGetCpuClockFrequency (alias to scePowerGetCpuClockFrequencyInt)
        case 0xFDB5BFE9: //scePowerGetCpuClockFrequencyInt (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_scePowerGetCpuClockFrequencyInt);
            break;  

        case 0xBD681969: //scePowerGetBusClockFrequencyInt (alias to scePowerGetBusClockFrequency)
        case 0x478FE6F5:// scePowerGetBusClockFrequency     (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_scePowerGetBusClockFrequency);
            break;         

        case 0x2085D15D: //scePowerGetBatteryLifePercent  (avoid syscall estimation)
			if (!g->syscalls_from_p5)
				hook_call = MAKE_JUMP(_hook_scePowerGetBatteryLifePercent);
            break;   

        case 0x8EFB3FA2: //scePowerGetBatteryLifeTime   (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_scePowerGetBatteryLifeTime);
            break;

        case 0x28E12023: // scePowerBatteryTemp (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_scePowerGetBatteryLifePercent);
            break;              
            
// Hooks to a function that does nothing but says "ok"   
        case 0xB4432BC8: //scePowerGetBatteryChargingStatus (avoid syscall estimation)
        case 0x0AFD0D8B: //scePowerIsBatteryExists (avoid syscall estimation)
        case 0x1E490401: //scePowerIsbatteryCharging (avoid syscall estimation)
        case 0xD3075926: //scePowerIsLowBattery (avoid syscall estimation)
        case 0x87440F5E: // scePowerIsPowerOnline  - Assuming it's not super necessary
        case 0x04B7766E: //	scePowerRegisterCallback - Assuming it's already done by the game
        case 0xD6D016EF: // scePowerLock - Assuming it's not super necessary
        case 0xEFD3C963: //scePowerTick (avoid syscall estimation)
        case 0xCA3D34C1: // scePowerUnlock - Assuming it's not super necessary
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break; 
#endif            
  
//if the game has no access to sceGeEdramGetSize :(
#ifdef FORCE_HARDCODED_VRAM_SIZE
        case 0x1F6752AD: // sceGeEdramGetSize
			hook_call = MAKE_JUMP(_hook_sceGeEdramGetSize);
            break;             
#endif 

// Define if the game has no access to sceUtilityCheckNetParam :(
#ifdef HOOK_ERROR_sceUtilityCheckNetParam
        case 0x5EEE6548: // sceUtilityCheckNetParam
			hook_call = MAKE_JUMP(_hook_generic_error);
            break;             
#endif 
  
// sceAudio Hooks  
  
#ifdef HOOK_AUDIOFUNCTIONS                    
        case 0x38553111: //sceAudioSRCChReserve(avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioSRCChReserve);
            break;
		
        case 0x5C37C0AE: //	sceAudioSRCChRelease (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioSRCChRelease);
            break;
		
        case 0xE0727056: // sceAudioSRCOutputBlocking (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioSRCOutputBlocking);
            break;
#endif
            
#ifdef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
        case 0x136CAF51: // sceAudioOutputBlocking (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioOutputBlocking);
            break; 
#endif   

#ifdef HOOK_sceAudioGetChannelRestLen_WITH_sceAudioGetChannelRestLength
        case 0xE9D97901: // sceAudioGetChannelRestLen (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceAudioGetChannelRestLength); //pure alias to sceAudioGetChannelRestLength
            break;  
#endif

#ifdef HOOK_sceAudioOutput_WITH_sceAudioOutputBlocking
        case 0x8C1009B2: // sceAudioOutput (avoid syscall estimation)
#ifndef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
			hook_call = MAKE_JUMP(sceAudioOutputBlocking);
#else
			hook_call = MAKE_JUMP(_hook_sceAudioOutputBlocking);
#endif
            break; 
#endif
            
            
// Define if the game has access to sceKernelDcacheWritebackAll but not sceKernelDcacheWritebackInvalidateAll
#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackAll
        case 0xB435DEC5: // sceKernelDcacheWritebackInvalidateAll
            hook_call = MAKE_JUMP(sceKernelDcacheWritebackAll);
            break;
#endif

#ifdef HOOK_CHDIR_AND_FRIENDS    
        case 0x55F4717D: //	sceIoChdir (only if it failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoChdir\n");
            hook_call = MAKE_JUMP(_hook_sceIoChdir);
            break;

/*
        case 0x109F50BC: //	sceIoOpen (only ifs sceIoChdir failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoOpen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;
*/

        case 0xB29DDF9C: //	sceIoDopen (only if sceIoChdir failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoDopen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoDopen);
            break;      
#elif defined VITA_DIR_FIX
        case 0xB29DDF9C: //	sceIoDopen
            LOGSTR0("VITA_DIR_FIX sceIoDopen\n");                        
            hook_call = MAKE_JUMP(sceIoDopen_Vita);
            break;      
#endif

#ifdef VITA_DIR_FIX
        case 0xE3EB004C: //	sceIoDread
            LOGSTR0("VITA_DIR_FIX sceIoDread\n");                        
            hook_call = MAKE_JUMP(sceIoDread_Vita);
            break;
        case 0xEB092469: //	sceIoDclose
            LOGSTR0("VITA_DIR_FIX sceIoDclose\n");                        
            hook_call = MAKE_JUMP(sceIoDclose_Vita);
            break;   
#endif


#ifdef HOOK_sceKernelSendMsgPipe_WITH_sceKernelTrySendMsgPipe
        case 0x876DBFAD: //	sceKernelSendMsgPipe (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceKernelTrySendMsgPipe);
            break;
#endif

#ifdef HOOK_sceKernelGetThreadId
        case 0x293B45B8: //	sceKernelGetThreadId (avoid syscall estimation) - Assuming it's not super necessary
			hook_call = MAKE_JUMP(_hook_sceKernelGetThreadId);
            break;      
#endif

#ifdef HOOK_sceAudioOutput2GetRestSample_WITH_sceAudioGetChannelRestLength
        case 0x647CEF33: // sceAudioOutput2GetRestSample (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioOutput2GetRestSample);
            break; 
#endif

	// TODO : implement real mersenne random generation
#ifdef HOOK_mersenne_twister_rdm
        case 0xE860E75E: // sceKernelUtilsMt19937Init (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelUtilsMt19937Init);
            break;
        case 0x06FB8A63: // sceKernelUtilsMt19937UInt (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelUtilsMt19937UInt);
            break;
#endif

	// This will be slow and should not be active for high performance programs...
#ifdef HOOK_sceDisplayWaitVblankStartCB_WITH_sceDisplayWaitVblankStart
        case 0x46F186C3: // sceDisplayWaitVblankStartCB (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceDisplayWaitVblankStart);
            break;
#endif

	// This will be slow and should not be active for high performance programs...
#ifdef HOOK_sceKernelDcacheWritebackRange_WITH_sceKernelDcacheWritebackAll
        case 0x3EE30821: // sceKernelDcacheWritebackRange (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceKernelDcacheWritebackAll);
            break;
#endif

#ifdef HOOK_sceKernelDcacheWritebackInvalidateRange_WITH_sceKernelDcacheWritebackInvalidateAll
        case 0x34B9FA9E: // sceKernelDcacheWritebackInvalidateRange (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceKernelDcacheWritebackInvalidateAll);
            break;
#endif

#ifdef HOOK_sceKernelDcacheWritebackAll_WITH_sceKernelDcacheWritebackRange
        case 0x79D1C3FA: // sceKernelDcacheWritebackAll (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelDcacheWritebackAll);
            break;
#endif

#ifdef HOOK_sceKernelDcacheWritebackInvalidateAll_WITH_sceKernelDcacheWritebackInvalidateRange
        case 0xB435DEC5: // sceKernelDcacheWritebackInvalidateAll (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelDcacheWritebackInvalidateAll);
            break;
#endif


#ifdef HOOK_sceAudioOutputPanned_WITH_sceAudioOutputPannedBlocking
        case 0xE2D56B2D: // sceAudioOutputPanned (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceAudioOutputPannedBlocking);
            break;
#endif


#ifdef HOOK_sceKernelTerminateThread_WITH_sceKernelTerminateDeleteThread
        case 0x616403BA: // sceKernelTerminateThread (avoid syscall estimation)
			hook_call = MAKE_JUMP(sceKernelTerminateDeleteThread);
            break;
#endif

#ifdef HOOK_sceKernelChangeCurrentThreadAttr_WITH_dummy
        case 0xEA748E31: // sceKernelChangeCurrentThreadAttr (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceCtrlSetIdleCancelThreshold_WITH_dummy
        case 0xA7144800: // sceCtrlSetIdleCancelThreshold (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceImposeSetHomePopup_WITH_dummy
        case 0x5595A71A: // sceImposeSetHomePopup (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceKernelReferThreadStatus
        case 0x17C1684E: // sceKernelReferThreadStatus (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceKernelTotalFreeMemSize
        case 0xF919F628: // sceKernelTotalFreeMemSize (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelTotalFreeMemSize);
            break;
#endif

#ifdef HOOK_sceKernelWakeupThread_WITH_dummy
        case 0xD59EAD2F: // sceKernelWakeupThread (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceDisplayGetVcount_WITH_dummy
        case 0x9C6EAAD7: // sceDisplayGetVcount (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif
	}


#endif

	return hook_call;
}


#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/malloc.h>
#include <common/path.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <hbl/mod/modmgr.h>
#include <hbl/stubs/hook.h>
#include <hbl/stubs/resolve.h>
#include <hbl/utils/settings.h>
#include <hbl/utils/md5.h>
#include <hbl/utils/memory.h>
#include <hbl/eloader.h>
#include <exploit_config.h>

//Note: most of the Threads monitoring code largely inspired (a.k.a. copy/pasted) by Noobz-Fanjita-Ditlew. Thanks guys!

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() calling setup_hook()

#define SIZE_THREAD_TRACKING_ARRAY 32
#define MAX_CALLBACKS 32
#define MAX_OPEN_DIR_VITA 10

static char *mod_chdir; //cwd of the currently running module
static int cur_cpufreq = 0; //current cpu frequency
static int cur_busfreq = 0; //current bus frequency
static SceUID running_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID pending_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID exited_th[SIZE_THREAD_TRACKING_ARRAY];
static SceUID openFiles[16];
static unsigned numOpenFiles = 0;
static SceUID osAllocs[512];
static unsigned osAllocNum = 0;
static SceKernelCallbackFunction cbfuncs[MAX_CALLBACKS];
static int cbids[MAX_CALLBACKS];
static int cbcount = 0;
static SceUID memSema;
static SceUID thSema;
static SceUID cbSema;
static SceUID audioSema;
static SceUID ioSema;
static int audio_th[8];
static int cur_ch_id = -1;
#ifdef VITA
static int dirLen;
static int dirFix[MAX_OPEN_DIR_VITA][2];
#endif


void init_hook()
{
#ifdef VITA
	int i, j;

	for (i = 0; i < MAX_OPEN_DIR_VITA; i++)
		for (j = 0; j < 2; j++)
 			dirFix[i][j] = -1;
#endif

	memSema = sceKernelCreateSema("hblmemsema", 0, 1, 1, 0);
	thSema = sceKernelCreateSema("hblthSema", 0, 1, 1, 0);
	cbSema = sceKernelCreateSema("hblcbsema", 0, 1, 1, 0);
	audioSema = sceKernelCreateSema("hblaudiosema", 0, 1, 1, 0);
	ioSema = sceKernelCreateSema("hbliosema", 0, 1, 1, 0);
}

//Checks if the homebrew should return to xmb on exit
// yes if user specified it in the cfg file AND it is not the menu
int force_return_to_xmb()
{
        unsigned int i = mod_table.num_loaded_mod;
    if (!return_to_xmb_on_exit) return 0;
    if (strcmp(mod_table.table[i].path, MENU_PATH) == 0) return 0;
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
int _hook_sceAudioSRCChRelease();
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
	int i;
	int found = 0;
	int thid = sceKernelGetThreadId();

	LOG_PRINTF("Enter hookExitThread : %08X\n", thid);

	WAIT_SEMA(thSema, 1, 0);

	for (i = 0; i < num_run_th; i++) {
		if (running_th[i] == thid) {
			found = 1;
			num_run_th--;
			LOG_PRINTF("Running threads: %d\n", num_run_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			running_th[i] = running_th[i + 1];
	}

	running_th[num_run_th] = 0;


#ifdef DELETE_EXIT_THREADS
	LOG_PRINTF("Num exit thr: %08X\n", num_exit_th);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (num_exit_th < SIZE_THREAD_TRACKING_ARRAY)
	{
		LOG_PRINTF("Set array\n");
		exited_th[num_exit_th] = thid;
		num_exit_th++;

		LOG_PRINTF("Exited threads: %d\n", num_exit_th);
	}
#endif

#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i = 0; i < 8; i++)
	{
		WAIT_SEMA(audioSema, 1, 0);
		if (audio_th[i] == thid)
			_hook_sceAudioChRelease(i);
		sceKernelSignalSema(audioSema, 1);
	}
#endif

	sceKernelSignalSema(thSema, 1);

	LOG_PRINTF("Exit hookExitThread\n");

	return sceKernelExitThread(status);
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
	int i;
	int found = 0;
	int thid = sceKernelGetThreadId();

    
	LOG_PRINTF("Enter hookExitDeleteThread : %08X\n", thid);

	WAIT_SEMA(thSema, 1, 0);

	for (i = 0; i < num_run_th; i++) {
		if (running_th[i] == thid) {
			found = 1;
			num_run_th--;
			LOG_PRINTF("Running threads: %d\n", num_run_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			running_th[i] = running_th[i + 1];
	}

	running_th[num_run_th] = 0;



	LOG_PRINTF("Exit hookExitDeleteThread\n");

#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
	for (i = 0; i < 8; i++) {
		WAIT_SEMA(audioSema, 1, 0);
		if (audio_th[i] == thid)
			_hook_sceAudioChRelease(i);
		sceKernelSignalSema(audioSema, 1);
	}
#endif

	//return (sceKernelExitDeleteThread(status));
	// (Patapon does not import this function)
	// But modules on p5 do.

#ifdef DELETE_EXIT_THREADS
	LOG_PRINTF("Num exit thr: %08X\n", num_exit_th);

	/*************************************************************************/
	/* Add to exited list                                                    */
	/*************************************************************************/
	if (num_exit_th < SIZE_THREAD_TRACKING_ARRAY)
	{
		LOG_PRINTF("Set array\n");
		exited_th[num_exit_th] = thid;
		num_exit_th++;

		LOG_PRINTF("Exited threads: %d\n", num_exit_th);
	}
#endif

	sceKernelSignalSema(thSema, 1);
	return sceKernelExitDeleteThread(status);
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

    
	SceUID lreturn = sceKernelCreateThread(name, entry, initPriority, stackSize, attr, option);

    if (lreturn < 0)
    {
        return lreturn;
    }


    LOG_PRINTF("API returned %08X\n", lreturn);

    /*************************************************************************/
    /* Add to pending list                                                   */
    /*************************************************************************/
    WAIT_SEMA(thSema, 1, 0);
    if (num_pend_th < SIZE_THREAD_TRACKING_ARRAY)
    {
      LOG_PRINTF("Set array\n");
      pending_th[num_pend_th] = lreturn;
      num_pend_th++;

      LOG_PRINTF("Pending threads: %d\n", num_pend_th);
    }

    sceKernelSignalSema(thSema, 1);
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
	int i;
	int found = 0;

	LOG_PRINTF("Enter hookRunThread: %08X\n", thid);

	WAIT_SEMA(thSema, 1, 0);

	LOG_PRINTF("Number of pending threads: %08X\n", num_pend_th);

	/***************************************************************************/
	/* Remove from pending list                                                */
	/***************************************************************************/
	for (i = 0; i < num_pend_th; i++) {
		if (pending_th[i] == thid) {
			found = 1;
			num_pend_th--;
			LOG_PRINTF("Pending threads: %d\n", num_pend_th);
		}

		if (found && i <= SIZE_THREAD_TRACKING_ARRAY - 2)
			pending_th[i] = pending_th[i + 1];
	}

	if (found) {
		pending_th[num_pend_th] = 0;

		/***************************************************************************/
		/* Add to running list                                                     */
		/***************************************************************************/
		LOG_PRINTF("Number of running threads: %08X\n", num_run_th);
		if (num_run_th < SIZE_THREAD_TRACKING_ARRAY) {
			running_th[num_run_th] = thid;
			num_run_th++;
			LOG_PRINTF("Running threads: %d\n", num_run_th);
		}
	}

	sceKernelSignalSema(thSema, 1);


	LOG_PRINTF("Exit hookRunThread: %08X\n", thid);

	return sceKernelStartThread(thid, arglen, argp);

}



//Cleans up Threads and allocated Ram
void threads_cleanup()
{

        int lthisthread = sceKernelGetThreadId();
    u32 i;
    LOG_PRINTF("Threads cleanup\n");
    WAIT_SEMA(thSema, 1, 0);
#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
    LOG_PRINTF("cleaning audio threads\n");
	for (i=0;i<8;i++)
	{
		//WAIT_SEMA(audioSema, 1, 0);
		_hook_sceAudioChRelease(i);
		//sceKernelSignalSema(audioSema, 1);
	}
#endif

    LOG_PRINTF("Running Threads cleanup\n");
    while (num_run_th > 0)
    {
        LOG_PRINTF("%d running threads remain\n", num_run_th);
        if (running_th[num_run_th - 1] != lthisthread)
        {
            LOG_PRINTF("Kill thread ID %08X\n", running_th[num_run_th - 1]);
            kill_thread(running_th[num_run_th - 1]);
        }
        else
        {
        LOG_PRINTF("Not killing myself - yet\n");
        }
        num_run_th--;
    }


    LOG_PRINTF("Pending Threads cleanup\n");
    while (num_pend_th > 0)
    {
        LOG_PRINTF("%d pending threads remain\n", num_pend_th);
        /*************************************************************************/
        /* This test shouldn't really apply to pending threads, but do it        */
        /* anyway                                                                */
        /*************************************************************************/
        if (pending_th[num_pend_th - 1] != lthisthread)
        {
            LOG_PRINTF("Kill thread ID %08X\n", pending_th[num_pend_th - 1]);
            sceKernelDeleteThread(pending_th[num_pend_th - 1]);
        }
        else
        {
            LOG_PRINTF("Not killing myself - yet\n");
        }
        num_pend_th--;
    }

#ifdef DELETE_EXIT_THREADS
    LOG_PRINTF("Sleeping Threads cleanup\n");
    /***************************************************************************/
    /* Delete the threads that exitted but haven't been deleted yet            */
    /***************************************************************************/
    while (num_exit_th > 0)
    {
        LOG_PRINTF("%d exited threads remain\n", num_exit_th);
        if (exited_th[num_exit_th - 1] != lthisthread)
        {
            LOG_PRINTF("Delete thread ID %08X\n", exited_th[num_exit_th - 1]);
            sceKernelDeleteThread(exited_th[num_exit_th - 1]);
        }
        else
        {
            LOG_PRINTF("Not killing myself - yet\n");
        }
        num_exit_th--;
    }
#endif

    sceKernelSignalSema(thSema, 1);
    LOG_PRINTF("Threads cleanup Done\n");
}

//
// File I/O manager Hooks
//
// returns 1 if a string is an absolute file path, 0 otherwise
int path_is_absolute(const char * file)
{
    unsigned i;
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
        if (!mod_chdir)
    {
        char * buf = malloc_hbl(strlen(file) + 1);
        strcpy(buf, file);
        return buf;
    }
#ifdef VITA
    else if (!strcmp(".", file))
    {
        char * buf = malloc_hbl(strlen(mod_chdir) + 1);
        strcpy(buf, mod_chdir);
        return buf;
    }
    else if (!strcmp("..", file))
    {
		char *ret;

		if ((ret = strrchr(mod_chdir, '/')) != NULL && ret[-1] != ':')
		{
		    ret[0] = 0;
		}

        char * buf = malloc_hbl(strlen(mod_chdir) + 1);
        strcpy(buf, mod_chdir);
        return buf;
    }
#endif

    int len = strlen(mod_chdir);

    char * buf = malloc_hbl( len +  1 +  strlen(file) + 1);

    strcpy(buf, mod_chdir);
    if(buf[len-1] !='/')
        strcat(buf, "/");

    strcat(buf,file);

    return buf;
}


//useful ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoOpenForChDirFailure(const char *file, int flags, SceMode mode)
{
        if (chdir_ok || path_is_absolute(file))
        return sceIoOpen(file, flags, mode);

    char * buf = relative_to_absolute(file);
    LOG_PRINTF("sceIoOpen override: %s become %s\n", (u32)file, (u32)buf);
    SceUID ret = sceIoOpen(buf, flags, mode);
    _free(buf);
    return ret;
}


SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode)
{
	SceUID ret;
	unsigned i;

	WAIT_SEMA(ioSema, 1, 0);
	ret = _hook_sceIoOpenForChDirFailure(file, flags, mode);

	// Add to tracked files array if files was opened successfully
	if (ret > -1)
	{
		if (numOpenFiles < sizeof(openFiles) / sizeof(SceUID) - 1)
		{
			for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++)
			{
				if (openFiles[i] == 0)
				{
					openFiles[i] = ret;
					numOpenFiles++;
					break;
				}
			}
		}
		else
		{
			LOG_PRINTF("WARNING: file list full, cannot add newly opened file\n");
		}
	}

	sceKernelSignalSema(ioSema, 1);
	//LOG_PRINTF("_hook_sceIoOpen(%s, %d, %d) = 0x%08X\n", (u32)file, (u32)flags, (u32)mode, (u32)result);
	return ret;
}


int _hook_sceIoClose(SceUID fd)
{
	SceUID ret;
	unsigned i;
    
	WAIT_SEMA(ioSema, 1, 0);
	ret = sceIoClose(fd);

	// Remove from tracked files array if closing was successfull
	if (ret > -1)
	{
		for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++)
		{
			if (openFiles[i] == fd)
			{
				openFiles[i] = 0;
				numOpenFiles--;
				break;
			}
		}
	}

	sceKernelSignalSema(ioSema, 1);
	//LOG_PRINTF("_hook_sceIoClose(0x%08X) = 0x%08X\n", (u32)fd, (u32)result);
	return ret;
}


// Close all files that remained open after the homebrew quits
void files_cleanup()
{
	unsigned i;

	LOG_PRINTF("Files Cleanup\n");
    
	WAIT_SEMA(ioSema, 1, 0);
	for (i = 0; i < sizeof(openFiles) / sizeof(SceUID); i++)
	{
		if (openFiles[i] != 0)
		{
			sceIoClose(openFiles[i]);
			LOG_PRINTF("closing file UID 0x%08X\n", openFiles[i]);
			openFiles[i] = 0;
		}
	}

	numOpenFiles = 0;
	sceKernelSignalSema(ioSema, 1);
	LOG_PRINTF("Files Cleanup Done\n");
}




//Resolve the call for a function and runs it without parameters
// This is quite ugly, so if you find a better way of doing this I'm all ears
int run_nid (u32 nid){
        // Is it known by HBL?
    int ret = get_nid_index(nid);
    LOG_PRINTF("NID: 0x%08X\n", nid);
    if (ret <= 0)
    {
        LOG_PRINTF("Unknown NID: 0x%08X\n", nid);
        return 0;
    }
    u32 syscall = globals->nid_table.table[ret].call;

    if (!syscall)
    {
        LOG_PRINTF("No syscall for NID: 0x%08X\n", nid);
        return 0;
    }

    u32 cur_stub[2];
    resolve_call(cur_stub, syscall);
    CLEAR_CACHE;

	// This debug line must also be present in the release build!
	// Otherwise the PSP will freeze and turn off when jumping to function().
    log_printf("call is: 0x%08X\n", cur_stub[0]);

    void (*function)() = (void *)(&cur_stub);
    function();
    return 1;
}

//Force close Net modules
void net_term()
{
    //Call the closing functions only if the matching library is loaded
	if( is_utility_loaded(PSP_MODULE_NET_INET) != 0 )
	{
	    if (get_lib_index("sceNetApctl") >= 0)
	    {
	        run_nid(0xB3EDD0EC); //sceNetApctlTerm();
	    }

	    if (get_lib_index("sceNetResolver") >= 0)
	    {
	        run_nid(0x6138194A); //sceNetResolverTerm();
	    }
	    if (get_lib_index("sceNetInet") >= 0)
	    {
	        run_nid(0xA9ED66B9); //sceNetInetTerm();
	    }
	}

	if( is_utility_loaded(PSP_MODULE_NET_COMMON) != 0)
	{
	    if (get_lib_index("sceNet") >= 0)
	    {
	        run_nid(0x281928A9); //sceNetTerm();
	    }
	}
}
#ifndef VITA
// Release the kernel audio channel
void audio_term()
{
	
	if (globals->syscalls_known)
	{
		// sceAudioSRCChRelease
#ifdef HOOK_AUDIOFUNCTIONS
		_hook_sceAudioSRCChRelease();
#else
		if (!run_nid(0x5C37C0AE))
		{
			estimate_syscall("sceAudio", 0x5C37C0AE, FROM_LOWEST);
			run_nid(0x5C37C0AE);
		}
#endif
	}
}
#endif

void ram_cleanup()
{
        unsigned i;

	LOG_PRINTF("Ram Cleanup\n");

	WAIT_SEMA(memSema, 1, 0);
	for (i = 0; i < osAllocNum; i++)
		sceKernelFreePartitionMemory(osAllocs[i]);
	osAllocNum = 0;
	sceKernelSignalSema(memSema, 1);

	LOG_PRINTF("Ram Cleanup Done\n");
}


// Release all subinterrupt handler
void subinterrupthandler_cleanup()
{
#ifdef SUB_INTR_HANDLER_CLEANUP
    LOG_PRINTF("Subinterrupthandler Cleanup\n");
	int i, j;

	for (i = 0; i <= 66; i++) // 66 is the highest value of enum PspInterrupts
	{
		for (j = 0; j <= 30; j++) // 30 is the highest value of enum PspSubInterrupts
		{
			if (sceKernelReleaseSubIntrHandler(i, j) > -1)
			{
				LOG_PRINTF("Subinterrupt handler released for %d, %d\n", i, j);
			}
		}
	}
    LOG_PRINTF("Subinterrupthandler Cleanup Done\n");
#endif
}


void exit_everything_but_me()
{
    net_term();
#ifndef VITA
	audio_term();
#endif
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
	    if (!hook_exit_cb_called && hook_exit_cb)
    {
        LOG_PRINTF("Call exit CB: %08X\n", hook_exit_cb);
        hook_exit_cb_called = 1;
        hook_exit_cb(0,0,NULL);
    }
*/
	LOG_PRINTF("_hook_sceKernelExitGame called\n");
    exit_everything_but_me();
    sceKernelExitDeleteThread(0);
}

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
        LOG_PRINTF("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08X\n", partitionid, (u32)name, type, size, (u32)addr);
    WAIT_SEMA(memSema, 1, 0);

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

	LOG_PRINTF("-> final allocation made for %d of %d requested bytes with result 0x%08X\n", size, original_size, uid);

	if (uid > 0)
	{
        /***********************************************************************/
        /* Succeeded OS alloc.  Record the block ID in the tracking list.      */
        /* (Don't worry if there's no space to record it, we'll just have to   */
        /* leak it).                                                           */
        /***********************************************************************/
        if (osAllocNum < sizeof(osAllocs) / sizeof(SceUID))
        {
            osAllocs[osAllocNum] = uid;
            osAllocNum ++;
            LOG_PRINTF("Num tracked OS blocks now: %08X\n", osAllocNum);
        }
        else
        {
            LOG_PRINTF("!!! EXCEEDED OS ALLOC TRACKING ARRAY\n");
        }
    }
    sceKernelSignalSema(memSema, 1);
    return uid;
}

int _hook_sceKernelFreePartitionMemory(SceUID blockid)
{
	int ret;
	unsigned i;
	int found = 0;

	WAIT_SEMA(memSema, 1, 0);
	ret = sceKernelFreePartitionMemory(blockid);

	/*************************************************************************/
	/* Remove UID from list of alloc'd mem.                                  */
	/*************************************************************************/
	for (i = 0; i < osAllocNum; i++) {
		if (osAllocs[i] == blockid)
			found = 1;

		if (found && i < sizeof(osAllocs) / sizeof(SceUID) - 2)
			osAllocs[i] = osAllocs[i + 1];
	}

	if (found)
		osAllocNum--;

	sceKernelSignalSema(memSema, 1);

	return ret;

}


/*****************************************************************************/
/* Create a callback.  Record the details so that we can refer to it if      */
/* need be.                                                                  */
/*****************************************************************************/
int _hook_sceKernelCreateCallback(const char *name, SceKernelCallbackFunction func, void *arg)
{
        int lrc = sceKernelCreateCallback(name, func, arg);

    LOG_PRINTF("Enter createcallback: %s\n", (u32)name);

    WAIT_SEMA(cbSema, 1, 0);
    if (cbcount < MAX_CALLBACKS)
    {
        cbids[cbcount] = lrc;
        cbfuncs[cbcount] = func;
        cbcount ++;
    }
    sceKernelSignalSema(cbSema, 1);

    LOG_PRINTF("Exit createcallback: %s ID: %08X\n", (u32) name, lrc);

    return(lrc);
}

/*****************************************************************************/
/* Register an ExitGame handler                                              */
/*****************************************************************************/
int _hook_sceKernelRegisterExitCallback(int cbid)
{
        int i;

    LOG_PRINTF("Enter registerexitCB: %08X\n", cbid);

    WAIT_SEMA(cbSema, 1, 0);
    for (i = 0; i < cbcount; i++)
    {
        if (cbids[i] == cbid)
        {
            LOG_PRINTF("Found matching CB, func: %08X\n", cbfuncs[i]);
            hook_exit_cb = cbfuncs[i];
            break;
        }
    }
    sceKernelSignalSema(cbSema, 1);

    LOG_PRINTF("Exit registerexitCB: %08X\n",cbid);

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
    	if (lreturn >= 0)
	{
	    if (lreturn >= 8)
		{
			LOG_PRINTF("AudioChReserve return out of range: %08X\n", lreturn);
			//waitForButtons();
		}
		else
		{
			WAIT_SEMA(audioSema, 1, 0);
			audio_th[lreturn] = sceKernelGetThreadId();
			sceKernelSignalSema(audioSema, 1);
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
                cur_ch_id = result;
    }
    return result;
}

//         sceAudioSRCOutputBlocking(PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
//   //      sceAudioOutputBlocking(audio_channel, PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf)
{
    #ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
    return sceAudioOutputBlocking(cur_ch_id,vol, buf);
#else
    return sceAudioOutputPannedBlocking(cur_ch_id,vol, vol, buf);
#endif
}

#ifdef HOOK_sceAudioOutputBlocking_WITH_sceAudioOutputPannedBlocking
//quite straightforward
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(channel,vol, vol, buf);
}
#endif

#ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
//quite straightforward
int _hook_sceAudioOutputPannedBlocking(int channel, int vol, int UNUSED(vol2), void * buf)
{
    return sceAudioOutputBlocking(channel,vol, buf);
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
        		WAIT_SEMA(audioSema, 1, 0);
		audio_th[channel] = 0;
		sceKernelSignalSema(audioSema, 1);
	}
#endif
	return lreturn;
}

//
int _hook_sceAudioSRCChRelease()
{
        if (cur_ch_id < 0 || cur_ch_id > 7)
    {
        LOG_PRINTF("FATAL: cur_ch_id < 0 in _hook_sceAudioSRCChRelease\n");
        return -1;
    }
    int result = _hook_sceAudioChRelease(cur_ch_id);
    cur_ch_id--;
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
#ifdef VITA
            sceIoDopen_Vita(dirname);
#else
            sceIoDopen(dirname);
#endif

    char * buf = relative_to_absolute(dirname);
#ifdef VITA
    SceUID ret = sceIoDopen_Vita(buf);
#else
    SceUID ret = sceIoDopen(buf);
#endif
    _free(buf);
    return ret;
}


#ifdef VITA
// Adds Vita's missing "." / ".." entries

SceUID sceIoDopen_Vita(const char *dirname)
{
    	LOG_PRINTF("sceIoDopen_Vita start\n");
    SceUID id = sceIoDopen(dirname);

	if (id >= 0)
	{
		int dirname_len = strlen(dirname);

		// If directory isn't "ms0:" or "ms0:/", add "." & ".." entries
		if (dirname[dirname_len - 1] != ':' && dirname[dirname_len - 2] != ':')
		{
			int i = 0;

			while (i < dirLen && dirFix[i][0] >= 0)
				++i;

			if (i < MAX_OPEN_DIR_VITA)
			{
				dirFix[i][0] = id;
				dirFix[i][1] = 2;

				if (i == dirLen)
					dirLen++;
			}
		}

	}


	LOG_PRINTF("sceIoDopen_Vita Done\n");
    return id;
}


int sceIoDread_Vita(SceUID id, SceIoDirent *dir)
{
    	LOG_PRINTF("sceIoDread_Vita start\n");
	int i = 0, errCode = 1;
	while (i < dirLen && id != dirFix[i][0])
		++i;


	if (id == dirFix[i][0])
	{
        memset(dir, 0, sizeof(SceIoDirent));
		if (dirFix[i][1] == 1)
		{
			strcpy(dir->d_name,"..");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		else if (dirFix[i][1] == 2)
		{
			strcpy(dir->d_name,".");
			dir->d_stat.st_attr |= FIO_SO_IFDIR;
			dir->d_stat.st_mode |= FIO_S_IFDIR;
		}
		dirFix[i][1]--;

		if (dirFix[i][1] == 0)
		{
			dirFix[i][0] = -1;
			dirFix[i][1] = -1;
		}
	}
	else
	{
		errCode = sceIoDread(id, dir);
	}

	LOG_PRINTF("sceIoDread_Vita Done\n");
	return errCode;
}

int sceIoDclose_Vita(SceUID id)
{
    	LOG_PRINTF("sceIoDclose_Vita start\n");
	int i = 0;
	while (i < dirLen && id != dirFix[i][0] )
		++i;


	if (i < dirLen && id == dirFix[i][0])
	{
			dirFix[i][0] = -1;
			dirFix[i][1] = -1;
	}

	LOG_PRINTF("sceIoDclose_Vita Done\n");
	return sceIoDclose(id);
}
#endif


//hook this ONLY if test_sceIoChdir() fails!
int _hook_sceIoChdir(const char *dirname)
{
	LOG_PRINTF("_hook_sceIoChdir start\n");
        // save chDir into global variable
    if (path_is_absolute(dirname))
    {
        if (mod_chdir)
        {
            _free(mod_chdir);
            mod_chdir = 0;
        }
        mod_chdir = relative_to_absolute(dirname);
    }
    else
    {
        char * result = relative_to_absolute(dirname);
         if (mod_chdir)
        {
            _free(mod_chdir);
        }
        mod_chdir = result;
    }

    if (!mod_chdir)
    {
        return -1;
    }
	LOG_PRINTF("_hook_sceIoChdir: %s becomes %s\n", (u32) dirname, (u32) mod_chdir);

    return 0;
}


// see http://forums.ps2dev.org/viewtopic.php?p=52329
int _hook_scePowerGetCpuClockFrequencyInt()
{
        if (!cur_cpufreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return cur_cpufreq;
}

int _hook_scePowerGetBusClockFrequency() {
        if (!cur_busfreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return cur_busfreq;
}


//Alias, see http://forums.ps2dev.org/viewtopic.php?t=11294
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq)
{
#if defined(HOOK_scePowerSetClockFrequency_WITH_scePower_EBD177D6)
    int ret = scePower_EBD177D6(pllfreq, cpufreq, busfreq);
#elif defined(HOOK_scePowerSetClockFrequency_WITH_scePower_469989AD)
    int ret = scePower_469989AD(pllfreq, cpufreq, busfreq);
#else
    int ret = 0;
#endif
    if (ret >= 0)
    {
                pllfreq = pllfreq;
        cpufreq = cpufreq;
        busfreq = busfreq;
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
#ifdef VITA
	return 660;
#else
	//There has to be a more efficient way...maybe get_fw_ver should directly do this
	u32 version = get_fw_ver();
	// 0x0w0y0z10 <==> firmware w.yz
	return 0x01000000 * (version / 100)
		+ 0x010000 * ((version % 100) / 10)
		+ 0x0100 * (version % 10)
		+ 0x10;
#endif
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

#ifdef HOOK_sceKernelTryReceiveMsgPipe_WITH_sceKernelReceiveMsgPipe
int _hook_sceKernelTryReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2)
{
    return sceKernelReceiveMsgPipe(uid, message, size, unk1, unk2, 0);
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
	LOG_PRINTF("_hook_sceKernelLoadModule\n");
	LOG_PRINTF("Attempting to load %s\n", (u32)path);

	SceUID elf_file = _hook_sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (elf_file < 0)
	{
		LOG_PRINTF("Error 0x%08X opening requested module %s\n", elf_file, (u32)path);
		return elf_file;
	}

	SceOff offset = 0;
	LOG_PRINTF("_hook_sceKernelLoadModule offset: 0x%08X\n", offset);
	SceUID ret = load_module(elf_file, path, NULL, offset);
    sceIoClose(elf_file);
	LOG_PRINTF("load_module returned 0x%08X\n", ret);

	return ret;
}

int	_hook_sceKernelStartModule(SceUID modid, SceSize UNUSED(argsize), void *UNUSED(argp), int *UNUSED(status), SceKernelSMOption *UNUSED(option))
{
	LOG_PRINTF("_hook_sceKernelStartModule\n");

	SceUID ret = start_module(modid);
	LOG_PRINTF("start_module returned 0x%08X\n", ret);

	return ret;
}
#endif

#ifdef HOOK_UTILITY

int _hook_sceUtilityLoadModule(int id)
{
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
	if (id == PSP_AV_MODULE_MP3
		|| id == PSP_AV_MODULE_AVCODEC
		|| id == PSP_AV_MODULE_ATRAC3PLUS
		|| id == PSP_AV_MODULE_MPEGBASE)
		return 0;
    return SCE_KERNEL_ERROR_ERROR;
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
int _hook_sceKernelGetThreadId()
{
	// Somehow sceKernelGetThreadId isn't imported by the game,
	// and can be called from HBL but not from a homebrew
    // This is because HBL accepts both jumps and syscalls, while the nid table for games only accepts syscalls
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

u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context UNUSED(*ctx))
{
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
int _hook_sceKernelReferThreadStatus(SceUID thid, SceKernelThreadInfo UNUSED(*info))
{
	memset(info+sizeof(SceSize), 0, info->size-sizeof(SceSize));

	return 0;
}
#endif

#ifdef HOOK_sceDisplayGetFrameBuf
static void *frame_topaddr[2];
static int frame_bufferwidth[2], frame_pixelformat[2];
int _hook_sceDisplaySetFrameBuf(void *topaddr,int bufferwidth, int pixelformat,int sync)
{
	frame_topaddr[ sync ] = topaddr;
	frame_bufferwidth[ sync ] = bufferwidth;
	frame_pixelformat[ sync ] = pixelformat;

	return sceDisplaySetFrameBuf( topaddr, bufferwidth, pixelformat, sync);
}

int _hook_sceDisplayGetFrameBuf(void **topaddr, int *bufferwidth, int *pixelformat, int sync)
{
	*topaddr = frame_topaddr[ sync ];
	*bufferwidth = frame_bufferwidth[ sync ];
	*pixelformat = frame_pixelformat[ sync ];
	return 0;
}
#endif

#ifdef HOOK_Osk
int _hook_sceUtilityOskInitStart (SceUtilityOskParams *params)
{
	if ( *((char*)params->data->outtext) == 0 )
	{
		if (params->data->outtextlimit > 3)
		{
			params->data->outtext[0] = 'L';
			params->data->outtext[1] = '_';
			params->data->outtext[2] = 'L';
			params->data->outtext[3] = 0;
		}
		else
		{
			params->data->outtext[0] = 'L';
			params->data->outtext[1] = 0;
		}

		/*	memset(oskData->outtext, 'L', oskData->outtextlimit);
			*((char*) (oskData->outtext+oskData->outtextlimit-1) ) = 0;*/
	}

	return 0;
}

#endif



// Returns a hooked call for the given NID or zero
u32 setup_hook(u32 nid, u32 UNUSED(existing_real_call))
{
	u32 hook_call = 0;
    
    //We have 3 switch blocks
    // The first one is for nids that need to be hooked in all cases
    //The second one is for firmwares that don't have perfect syscall estimation, in all cases
	// The third one is for firmwares that don't have perfect syscall estimation, only if the function is not already imported by the game
    switch (nid)
	{

//utility functions, we need those
        case 0x237DBD4F: // sceKernelAllocPartitionMemory
            LOG_PRINTF(" mem trick ");
            hook_call = MAKE_JUMP(_hook_sceKernelAllocPartitionMemory);
            break;

        case 0xB6D61D02: // sceKernelFreePartitionMemory
            LOG_PRINTF(" mem trick ");
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
			LOG_PRINTF(" loadmodule trick ");
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
			LOG_PRINTF(" loadmodule trick ");
			hook_call = MAKE_JUMP(_hook_sceKernelStartModule);
			break;
#endif


#ifdef HOOK_UTILITY
		case 0xC629AF26: //sceUtilityLoadAvModule
			LOG_PRINTF(" Hook sceUtilityLoadAvModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadAvModule);
			break;

		case 0x0D5BC6D2: //sceUtilityLoadUsbModule
			LOG_PRINTF(" Hook sceUtilityLoadUsbModule\n");
			hook_call = MAKE_JUMP(_hook_generic_error);
			break;

		case 0x1579a159: //sceUtilityLoadNetModule
			LOG_PRINTF(" Hook sceUtilityLoadNetModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadNetModule);
			break;

		case 0x2A2B3DE0: // sceUtilityLoadModule
			LOG_PRINTF(" Hook sceUtilityLoadModule\n");
			hook_call = MAKE_JUMP(_hook_sceUtilityLoadModule);
			break;

		case 0xF7D8D092: //sceUtilityUnloadAvModule
		case 0xF64910F0: //sceUtilityUnloadUsbModule
		case 0x64d50c56: //sceUtilityUnloadNetModule
		case 0xE49BFE92: // sceUtilityUnloadModule
			LOG_PRINTF(" Hook sceUtilityUnloadModule\n");
			hook_call = MAKE_JUMP(_hook_generic_ok);
			break;
#endif

		// Hook these to keep track of open files when the homebrew quits

		case 0x109F50BC: //	sceIoOpen
            LOG_PRINTF("Hook sceIoOpen\n");
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;

		case 0x810C4BC3: //	sceIoClose
            LOG_PRINTF("Hook sceIoClose\n");
            hook_call = MAKE_JUMP(_hook_sceIoClose);
            break;

    }

    if (hook_call)
        return hook_call;
#ifndef VITA
	// Overrides below this point don't need to be done if we have perfect syscall estimation
	if (globals->syscalls_known)
		return 0;
#endif
    switch (nid) {
#ifdef HOOK_CHDIR_AND_FRIENDS
        case 0x55F4717D: //	sceIoChdir (only if it failed)
            if (chdir_ok)
                break;
            LOG_PRINTF(" Chdir trick sceIoChdir\n");
            hook_call = MAKE_JUMP(_hook_sceIoChdir);
            break;

/*
        case 0x109F50BC: //	sceIoOpen (only ifs sceIoChdir failed)
            if (chdir_ok)
                break;
            LOG_PRINTF(" Chdir trick sceIoOpen\n");
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;
*/

        case 0xB29DDF9C: //	sceIoDopen (only if sceIoChdir failed)
            if (chdir_ok)
                break;
            LOG_PRINTF(" Chdir trick sceIoDopen\n");
            hook_call = MAKE_JUMP(_hook_sceIoDopen);
            break;
#elif defined VITA
        case 0xB29DDF9C: //	sceIoDopen
            LOG_PRINTF("VITA sceIoDopen\n");
            hook_call = MAKE_JUMP(sceIoDopen_Vita);
            break;
#endif

#ifdef VITA
        case 0xE3EB004C: //	sceIoDread
            LOG_PRINTF("VITA sceIoDread\n");
            hook_call = MAKE_JUMP(sceIoDread_Vita);
            break;
        case 0xEB092469: //	sceIoDclose
            LOG_PRINTF("VITA sceIoDclose\n");
            hook_call = MAKE_JUMP(sceIoDclose_Vita);
            break;
#endif

#ifdef HOOK_sceDisplayGetFrameBuf
		case 0xEEDA2E54://sceDisplayGetFrameBuf
			hook_call = MAKE_JUMP(_hook_sceDisplayGetFrameBuf);
			break;
		case 0x289D82FE://sceDisplaySetFrameBuf
			hook_call = MAKE_JUMP(_hook_sceDisplaySetFrameBuf);
			break;
#endif

#ifdef HOOK_sceKernelSleepThreadCB_WITH_sceKernelDelayThreadCB
		case 0x82826F70: // sceKernelSleepThreadCB   (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelSleepThreadCB);
			break;
#endif

    }

    if (hook_call)
        return hook_call;


#ifdef DONT_HOOK_IF_FUNCTION_IS_IMPORTED
	// Don't need to do a hook if we already have the call
    if (existing_real_call)
        return 0;
#endif

#ifndef DISABLE_ADDITIONAL_HOOKS

    switch (nid)
	{

// Overrides to avoid syscall estimates, those are not necessary but reduce estimate failures and improve compatibility for now
        case 0x06A70004: //	sceIoMkdir
            if (override_sceIoMkdir == GENERIC_SUCCESS)
            {
                LOG_PRINTF(" sceIoMkdir goes to void because of settings ");
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
            LOG_PRINTF(" mem trick ");
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

        //others
        case 0x68963324: //	sceIoLseek32 (avoid syscall estimation)
            //based on http://forums.ps2dev.org/viewtopic.php?t=12490
			hook_call = MAKE_JUMP(_hook_sceIoLseek32);
            break;

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

#ifdef HOOK_sceKernelTrySendMsgPipe_WITH_sceKernelSendMsgPipe
        case 0x884C9F90: //	sceKernelTrySendMsgPipe (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceKernelTrySendMsgPipe);
            break;
#endif

#ifdef HOOK_sceKernelTryReceiveMsgPipe_WITH_sceKernelReceiveMsgPipe
        case 0xDF52098F: // sceKernelTryReceiveMsgPipe
            hook_call = MAKE_JUMP(_hook_sceKernelTryReceiveMsgPipe);
            break;
#endif

#ifdef HOOK_sceKernelReceiveMsgPipe_WITH_sceKernelTryReceiveMsgPipe
        case 0x74829B76: // sceKernelReceiveMsgPipe (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelReceiveMsgPipe);
            break;
#endif

#ifdef HOOK_sceKernelReferMsgPipeStatusWITH_dummy
		case 0x33BE4024://sceKernelReferMsgPipeStatus
            hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif
/*
		case 0xD97F94D8 ://sceDmacTryMemcpy
            hook_call = MAKE_JUMP(sceDmacMemcpy);
			break;
*/
        case 0x94AA61EE: // sceKernelGetThreadCurrentPriority (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceKernelGetThreadCurrentPriority);
            break;

		case 0xBC6FEBC5://sceKernelReferSemaStatus
		case 0x17C1684E: //sceKernelReferThreadStatus
			hook_call = MAKE_JUMP(_hook_generic_error);
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

#ifdef HOOK_sceAudioOutputPannedBlocking_WITH_sceAudioOutputBlocking
        case 0x13F592BC: // sceAudioOutputPannedBlocking (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceAudioOutputPannedBlocking);
            break;
#endif

#ifdef HOOK_sceAudioGetChannelRestLength_WITH_dummy
        case 0xb011922f: // sceAudioGetChannelRestLength (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif

#ifdef HOOK_sceAudioGetChannelRestLen_WITH_dummy
        case 0xE9D97901: // sceAudioGetChannelRestLen (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
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

#ifdef HOOK_sceMp3InitResource_WITH_dummy
		case 0x35750070: //sceMp3InitResource
		    hook_call = MAKE_JUMP(_hook_generic_ok);
			break;
#endif

#ifdef HOOK_sceMp3Init_WITH_dummy
		case 0x44E07129: //sceMp3Init
		   hook_call = MAKE_JUMP(_hook_generic_ok);
		   break;
#endif

#ifdef HOOK_sceMp3Decode_WITH_dummy
		case 0xD021C0FB: //sceMp3Decode
			hook_call = MAKE_JUMP(_hook_generic_ok);
			break;
#endif

#ifdef HOOK_Osk
        case 0xF6269B82: // sceUtilityOskInitStart (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_sceUtilityOskInitStart);
            break;

        case 0x3DFAEBA9: // sceUtilityOskShutdownStart (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;

        case 0x4B85C861: // sceUtilityOskUpdate (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;

        case 0xF3F76017: // sceUtilityOskGetStatus (avoid syscall estimation)
			hook_call = MAKE_JUMP(_hook_generic_ok);
            break;
#endif
	}


#endif

    LOG_PRINTF("Missing function: 0x%08X Hooked with: 0x%08X\n\n",nid,hook_call);
	return hook_call;
}

/**
*  Setup a Default Nid for Firmware 6.60 where the Kernel Syscall Estimation doesn't work
*  Only ment to give a ok for function that we don't make custom version from
*  By Thecobra
**/
u32 setup_default_nid(u32 nid){
	u32 hook_call = 0;
	switch(nid){
	  case 0x3E0271D3: //sceKernelVolatileMemLock
	  case 0xA14F40B2: //sceKernelVolatileMemTryLock
	  case 0xA569E425: //sceKernelVolatileMemUnlock
	    hook_call = MAKE_JUMP(_hook_generic_error);
		break;
	  default: //Default Ok function
		hook_call = MAKE_JUMP(_hook_generic_ok);
		break;
	}

	LOG_PRINTF("Missing function: 0x%08X Defaulted to generic: 0x%08X\n\n",nid,hook_call);

	return hook_call;
}

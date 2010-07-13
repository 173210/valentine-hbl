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

// Returns a hooked call for the given NID or zero
u32 setup_hook(u32 nid)
{
	u32 hook_call = 0;
    tGlobals * g = get_globals();
	
	// HOOOOOOK THAT!!!
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
			hook_call = MAKE_JUMP(_hook_sceKernelLoadModule);
			break;
		
		case 0x50F0C1EC: // sceKernelStartModule
			LOGSTR0(" loadmodule trick ");
			hook_call = MAKE_JUMP(_hook_sceKernelStartModule);
			break;
#endif  

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
            
		case 0x3F7AD767: //	sceRtcGetCurrentTick (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceRtcGetCurrentTick);
            break;   
            
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
  
        case 0x4CFA57B0: //sceRtcGetCurrentClock	 (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceRtcGetCurrentClock);
            break; 
            
        //others
        case 0x68963324: //	sceIoLseek32 (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceIoLseek32);
            break;
		
        case 0x3A622550: //	sceCtrlPeekBufferPositive (avoid syscall estimation)
            if (g->override_sceCtrlPeekBufferPositive == OVERRIDE)
            {
                hook_call = MAKE_JUMP(_hook_sceCtrlPeekBufferPositive);
            }
            break;
            
        case 0x737486F2: // scePowerSetClockFrequency   - yay, that's a pure alias :)
            hook_call = MAKE_JUMP(_hook_scePowerSetClockFrequency);
            break;  
            
        case 0x383F7BCC: // sceKernelTerminateDeleteThread  (avoid syscall estimation)
            hook_call = MAKE_JUMP(kill_thread); //TODO Take into account with thread monitors ?
            break;    
        
        case 0xD675EBB8: // sceKernelSelfStopUnloadModule (avoid syscall estimation) - not sure about this one
            hook_call = MAKE_JUMP(_hook_sceKernelSelfStopUnloadModule);
            break;               

        case 0x82826F70: // sceKernelSleepThreadCB   (avoid syscall estimation)          
            hook_call = MAKE_JUMP(_hook_sceKernelSleepThreadCB);
            break; 
            
        case 0x884C9F90: //	sceKernelTrySendMsgPipe (avoid syscall estimation)  
            hook_call = MAKE_JUMP(_hook_sceKernelTrySendMsgPipe);
            break;    

        case 0x74829B76: // sceKernelReceiveMsgPipe (avoid syscall estimation)  
            hook_call = MAKE_JUMP(_hook_sceKernelReceiveMsgPipe);
            break;                 
           
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

        case 0x478FE6F5:// scePowerGetBusClockFrequency     (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_scePowerGetBusClockFrequency);
            break;         

        case 0x2085D15D: //scePowerGetBatteryLifePercent  (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_scePowerGetBatteryLifePercent);
            break;   

        case 0x8EFB3FA2: //scePowerGetBatteryLifeTime   (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_scePowerGetBatteryLifeTime);
            break;      
            
// Hooks to a function that does nothing but says "ok"            
        case 0x87440F5E: // scePowerIsPowerOnline  - Assuming it's not super necessary
        case 0x04B7766E: //	scePowerRegisterCallback - Assuming it's already done by the game
        case 0xD6D016EF: // scePowerLock - Assuming it's not super necessary
        case 0xCA3D34C1: // scePowerUnlock - Assuming it's not super necessary
            hook_call = MAKE_JUMP(_hook_generic_ok);
            break; 	   

#endif            
  
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
  
        case 0x136CAF51: // sceAudioOutputBlocking (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceAudioOutputBlocking);
            break;  
              

#endif   

#ifdef HOOK_CHDIR_AND_FRIENDS    
        case 0x55F4717D: //	sceIoChdir (only if it failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoChdir\n");
            hook_call = MAKE_JUMP(_hook_sceIoChdir);
            break;
		
        case 0x109F50BC: //	sceIoOpen (only ifs sceIoChdir failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoOpen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;
		
        case 0xB29DDF9C: //	sceIoDopen (only if sceIoChdir failed)
            if (g->chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoDopen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoDopen);
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
    }

	return hook_call;
}

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

	sceKernelWaitSema(g->thrSema, 1, 0);

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
		sceKernelWaitSema(g->audioSema, 1, 0);
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

	sceKernelWaitSema(g->thrSema, 1, 0);

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
		sceKernelWaitSema(g->audioSema, 1, 0);
		if (g->audio_threads[i] == lthreadid)
		{
			_hook_sceAudioChRelease(i);
		}
		sceKernelSignalSema(g->audioSema, 1);
	}
#endif   
    
	//return (sceKernelExitDeleteThread(status)); //Patapon does not import this function
    
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
    return (sceKernelExitThread(status));

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
    sceKernelWaitSema(g->thrSema, 1, 0);
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

  sceKernelWaitSema(g->thrSema, 1, 0);

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
    sceKernelWaitSema(g->thrSema, 1, 0);
#ifdef MONITOR_AUDIO_THREADS
	// Ditlew
    LOGSTR0("cleaning audio threads\n");
	for (i=0;i<8;i++)
	{
		//sceKernelWaitSema(g->audioSema, 1, 0);
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
    sceKernelDcacheWritebackInvalidateAll();
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

void ram_cleanup() 
{
    LOGSTR0("Ram Cleanup\n");
    tGlobals * g = get_globals();
    u32 ii;
    sceKernelWaitSema(g->memSema, 1, 0);
    for (ii=0; ii < g->osAllocNum; ii++)
    {
        sceKernelFreePartitionMemory(g->osAllocs[ii]);
    }
    g->osAllocNum = 0;
    sceKernelSignalSema(g->memSema, 1);
    LOGSTR0("Ram Cleanup Done\n");  
}

//To return to the menu instead of exiting the game
void  _hook_sceKernelExitGame() 
{

    tGlobals * g = get_globals();
    /***************************************************************************/
    /* Call any exit callback first.                                           */
    /***************************************************************************/
    if (!g->calledexitcb && g->exitcallback)
    {
        LOGSTR1("Call exit CB: %08lX\n", (u32) g->exitcallback);
        g->calledexitcb = 1;
        g->exitcallback(0,0,NULL);
    }

    net_term();
    threads_cleanup();
    ram_cleanup();
    sceKernelExitDeleteThread(0);
}

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
    tGlobals * g = get_globals();
    LOGSTR5("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08lX\n", partitionid, (u32)name, type, size, (u32)addr);
    sceKernelWaitSema(g->memSema, 1, 0);
    SceUID uid = sceKernelAllocPartitionMemory(partitionid,name, type, size, addr);
    if (uid <=0)
    {
        LOGSTR1("failed with result: 0x%08lX\n", uid);
    }    
    else
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
    sceKernelWaitSema(g->memSema, 1, 0);
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

    sceKernelWaitSema(g->cbSema, 1, 0);
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
    int lrc = 0;
    int i;

    LOGSTR1("Enter registerexitCB: %08lX\n", cbid);

    sceKernelWaitSema(g->cbSema, 1, 0);
    for (i = 0; i < g->callbackcount; i++)
    {
        if (g->callbackids[i] == cbid)
        {
            LOGSTR1("Found matching CB, func: %08lX\n", (u32) g->callbackfuncs[i]);
            g->exitcallback = g->callbackfuncs[i];
            break;
        }
    }
    lrc = sceKernelRegisterExitCallback(cbid);
    sceKernelSignalSema(g->cbSema, 1);

    LOGSTR2("Exit registerexitCB: %08lX ret: %08lX\n", cbid, lrc);

    return(lrc);
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

int _hook_sceRtcGetCurrentClock  (pspTime  *time, int UNUSED(tz))
{
    //todo deal with the timezone...
    return sceRtcGetCurrentClockLocalTime(time);
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

int _hook_sceRtcGetCurrentTick(u64 * tick)
{   	
    pspTime time;
    sceRtcGetCurrentClockLocalTime(&time);
    return _hook_sceRtcGetTick (&time, tick);
}

//based on http://forums.ps2dev.org/viewtopic.php?t=12490
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence) 
{
    return sceIoLseek(fd, offset, whence); 
}

//based on http://forums.ps2dev.org/viewtopic.php?p=27173
//This will be slow and should not be active for high performance programs...
int _hook_sceCtrlPeekBufferPositive(SceCtrlData* pad_data,int count)
{
    return sceCtrlReadBufferPositive(pad_data,count);
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
			sceKernelWaitSema(g->audioSema, 1, 0);
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
    int result =  _hook_sceAudioChReserve (PSP_AUDIO_NEXT_CHANNEL, samplecount, format);
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

//quite straightforward
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(channel,vol, vol, buf);
}

// Ditlew
int _hook_sceAudioChRelease(int channel)
{
	int lreturn;

	lreturn = sceAudioChRelease( channel );

#ifdef MONITOR_AUDIO_THREADS
	if (lreturn >= 0)
	{
        tGlobals * g = get_globals();
		sceKernelWaitSema(g->audioSema, 1, 0);  
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
    sceIoChdir(HBL_ROOT);
    if (file_exists(HBL_BIN))
        return 1;
    return 0;
}

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
    int len = strlen(g->module_chdir);
    
    char * buf = malloc_hbl( len +  1 +  strlen(file) + 1);

    strcpy(buf, g->module_chdir);
    if(buf[len-1] !='/')
        strcat(buf, "/");
        
    strcat(buf,file);
    return buf;
}

//hook this ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode)
{
    if (path_is_absolute(file))
        return sceIoOpen(file, flags, mode);
        
    char * buf = relative_to_absolute(file);
    SceUID ret = sceIoOpen(buf, flags, mode);
    free(buf);
    return ret;
}

//hook this ONLY if test_sceIoChdir() fails!
SceUID _hook_sceIoDopen(const char *dirname)   
{
    if (path_is_absolute(dirname))
        return sceIoDopen(dirname);
        
    char * buf = relative_to_absolute(dirname);
    SceUID ret = sceIoDopen(buf);
    free(buf);
    return ret;
}

//hook this ONLY if test_sceIoChdir() fails!
int _hook_sceIoChdir(const char *dirname)   
{
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

// see http://powermgrprx.googlecode.com/svn-history/r2/trunk/include/pspmodulemgr.h
int _hook_sceKernelSelfStopUnloadModule  (int exitcode, SceSize  argsize, void *argp)
{
    int status;
    return ModuleMgrForUser_8F2DF740(exitcode, argsize, argp, &status, NULL);

}

int _hook_sceKernelGetThreadCurrentPriority()
{
    return 0x18;
    //TODO : real management of threads --> keep track of their priorities ?
}

// Sleep is not delay
int _hook_sceKernelSleepThreadCB() 
{
    while (1)
        sceKernelDelayThreadCB(1000000);
    return 1;
}

int _hook_sceKernelTrySendMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2)
{
    return sceKernelSendMsgPipe(uid, message, size, unk1, unk2, 0);
}

int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int UNUSED(timeout))
{
    return sceKernelTryReceiveMsgPipe(uid, message, size, unk1, unk2);
}



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
	
	SceUID elf_file = sceIoOpen(path, PSP_O_RDONLY, 0777);

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


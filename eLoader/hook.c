#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "malloc.h"
#include "hook.h"
#include "memory.h"

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() calling setup_hook()

char* g_module_chdir = NULL;

int chdir_ok = 0;

// Returns a hooked call for the given NID or zero
u32 setup_hook(u32 nid)
{
	u32 hook_call = 0;
	
	// HOOOOOOK THAT!!!
    switch (nid) 
	{

//utility functions, we need those
        case 0x237DBD4F: // sceKernelAllocPartitionMemory
            LOGSTR0(" mem trick ");
            hook_call = MAKE_JUMP(_hook_sceKernelAllocPartitionMemory);
            break;                     
            
#ifdef HOOK_THREADS
        case 0x446D8DE6: // sceKernelCreateThread
            hook_call = MAKE_JUMP(_hook_sceKernelCreateThread);
            break;

		case 0xF475845D: // sceKernelStartThread
			hook_call = MAKE_JUMP(_hook_sceKernelStartThread);
			break;
#endif

#ifdef RETURN_TO_MENU_ON_EXIT                
        case 0x05572A5F: // sceKernelExitGame
            if (g_menu_enabled)
                hook_call = MAKE_JUMP(_hook_sceKernelExitGame);
            break;
#endif 

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
	    case 0x3FC9AE6A:  //sceKernelDevkitVersion   (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceKernelDevkitVersion);
            break;
            
        case 0xA291F107: // sceKernelMaxFreeMemSize (avoid syscall estimation)
            LOGSTR0(" mem trick ");
            hook_call = MAKE_JUMP(sceKernelMaxFreeMemSize);
            break;
		
        case 0xC41C2853: //	sceRtcGetTickResolution (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceRtcGetTickResolution);
            break;
		
        case 0x68963324: //	sceIoLseek32 (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceIoLseek32);
            break;
		
        case 0x3A622550: //	sceCtrlPeekBufferPositive (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceCtrlPeekBufferPositive);
            break;
		
        case 0x3F7AD767: //	sceRtcGetCurrentTick (avoid syscall estimation)
            hook_call = MAKE_JUMP(_hook_sceRtcGetCurrentTick);
            break;   
            
        case 0x737486F2: // scePowerSetClockFrequency   - yay, that's a pure alias :)
            hook_call = MAKE_JUMP(_hook_scePowerSetClockFrequency);
            break;  
            
        case 0x383F7BCC: // sceKernelTerminateDeleteThread  (avoid syscall estimation)
            hook_call = MAKE_JUMP(kill_thread);
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

        case 0x809CE29B : // sceKernelExitDeleteThread (avoid syscall estimation)  
            hook_call = MAKE_JUMP(_hook_sceKernelExitDeleteThread);
            break;               
           
        case 0x94AA61EE: // sceKernelGetThreadCurrentPriority (avoid syscall estimation)  
            hook_call = MAKE_JUMP(_hook_sceKernelGetThreadCurrentPriority);
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
            if (chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoChdir\n");
            hook_call = MAKE_JUMP(_hook_sceIoChdir);
            break;
		
        case 0x109F50BC: //	sceIoOpen (only ifs sceIoChdir failed)
            if (chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoOpen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoOpen);
            break;
		
        case 0xB29DDF9C: //	sceIoDopen (only if sceIoChdir failed)
            if (chdir_ok)
                break;
            LOGSTR0(" Chdir trick sceIoDopen\n");                        
            hook_call = MAKE_JUMP(_hook_sceIoDopen);
            break;                      
#endif	
    }

	return hook_call;
}

#ifdef HOOK_THREADS

SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority,
                             	   int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{ 
	LOGSTR5("\n->Attempting to create thread named %s with entry point 0x%08lX, priority 0x%08lX, stack size 0x%08lX, and attributes 0x%08lX\n", 
	        name, entry, currentPriority, stackSize, attr);
	
    //u32 gp_bak = 0;
	SceUID res;


	/*
    if (gp) 
	{
        GET_GP(gp_bak);
        SET_GP(gp);
    }
	
    entry_point = entry;
	*/
	
    res = sceKernelCreateThread(name, entry, currentPriority, stackSize, attr, option);

	if (res < 0)
	{
		LOGSTR1("Thread creation failed with error 0x%08lX\n", res);
	}
	else
	{
		LOGSTR1("Thread successfully created with ID 0x%08lX\n", res);		
	}

	/*
    if (gp) 
	{
        SET_GP(gp_bak);
    }
	*/
	
    return res;
}

int	_hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
	SceUID res;

	LOGSTR1("\n->Attempting to start thread ID 0x%08lX\n", thid);

	res = sceKernelStartThread(thid, arglen, argp);

	if (res < 0)
	{
		LOGSTR1("Thread starting failed with error 0x%08lX\n", res);
	}
	else
	{
		LOGSTR1("Thread successfully started!\n", res);	
	}

	return res;
}
#endif

#ifdef RETURN_TO_MENU_ON_EXIT
void  _hook_sceKernelExitGame() 
{
	SceUID thid = sceKernelCreateThread("HBL", main_loop, 0x18, 0x10000, 0, NULL);
	
	if (thid >= 0)
	{
		thid = sceKernelStartThread(thid, 0, NULL);
    }
    sceKernelExitDeleteThread(0);
}
#endif

SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr)
{
    LOGSTR5("call to sceKernelAllocPartitionMemory partitionId: %d, name: %s, type:%d, size:%d, addr:0x%08lX\n", partitionid,name, type, size, addr);
    SceUID uid = sceKernelAllocPartitionMemory(partitionid,name, type, size, addr);
    if (uid <=0)
        LOGSTR1("failed with result: 0x%08lX\n", uid);
    return uid;
}

// always returns 1000000
// based on http://forums.ps2dev.org/viewtopic.php?t=4821
u32 _hook_sceRtcGetTickResolution() 
{
    return 1000000;
}

int _hook_sceRtcGetCurrentTick(u64 * tick)
{   	
    pspTime time;
    int ret =  sceRtcGetCurrentClockLocalTime(&time);
    u64 seconds = 
        (u64) time.day * 24 * 3600
        + (u64) time.hour * 3600 
        + (u64) time.minutes * 60
        + (u64) time.seconds; 
    *tick = seconds * 1000000   + (u64) time.microseconds;
    return 0;
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

int audio_channels[8] = {0,0,0,0,0,0,0,0};
int cur_channel_id = -1;
//audio hooks
// see commented code in http://forums.ps2dev.org/viewtopic.php?p=81473
// //   int audio_channel = sceAudioChReserve(0, 2048 , PSP_AUDIO_FORMAT_STEREO);
//   sceAudioSRCChReserve(2048, wma_samplerate, 2); 
int _hook_sceAudioSRCChReserve (int samplecount, int freq, int channels)
{
    int format = PSP_AUDIO_FORMAT_STEREO;
    if (channels == 1) 
        format = PSP_AUDIO_FORMAT_MONO;
    int result =  sceAudioChReserve (PSP_AUDIO_NEXT_CHANNEL, samplecount, format);
    if (result >= 0)
    {
        cur_channel_id++;
        if (cur_channel_id > 7)
        {
            LOGSTR0("FATAL: cur_channel_id > 7 in _hook_sceAudioSRCChReserve\n");
            return -1;
        } 
        audio_channels[cur_channel_id] = result;
    }   
    return result;
} 

//         sceAudioSRCOutputBlocking(PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]);
//   //      sceAudioOutputBlocking(audio_channel, PSP_AUDIO_VOLUME_MAX, wma_output_buffer[wma_output_index]); 
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(audio_channels[cur_channel_id],vol, vol, buf);
}

//quite straightforward
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf)
{
    return sceAudioOutputPannedBlocking(channel,vol, vol, buf);
}

//
int _hook_sceAudioSRCChRelease()
{
    if (cur_channel_id < 0)
    {
        LOGSTR0("FATAL: cur_channel_id < 0 in _hook_sceAudioSRCChRelease\n");
        return -1;
    } 
    int result = sceAudioChRelease(audio_channels[cur_channel_id]);
    cur_channel_id--;
    return result;
}  

int test_sceIoChdir()
{
    sceIoChdir(HBL_ROOT);
    if (file_exists(HBL_BIN))
        return 1;
    return 0;
}

// returns 1 if a string is anabsolute file path, 0 otherwise
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
    if (!g_module_chdir)
    {
        char * buf = malloc(strlen(file) + 1);
        strcpy(buf, file);
        return buf;
    }
    int len = strlen(g_module_chdir);
    
    char * buf = malloc( len +  1 +  strlen(file) + 1);

    strcpy(buf, g_module_chdir);
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
    // save chDir into global variable
    if (path_is_absolute(dirname))
    {
        if (g_module_chdir)
        {
            free(g_module_chdir);
            g_module_chdir = 0;
        }  
        g_module_chdir = relative_to_absolute(dirname);
    }
    else
    {
        char * result = relative_to_absolute(dirname);
         if (g_module_chdir)
        {
            free(g_module_chdir);
        }
        g_module_chdir = result;
    }
    return g_module_chdir;
}

int g_pllfreq = 0;
int g_cpufreq = 0;
int g_busfreq = 0;

// see http://forums.ps2dev.org/viewtopic.php?p=52329
int _hook_scePowerGetCpuClockFrequencyInt()
{
    if (!g_cpufreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return g_cpufreq;
}

int _hook_scePowerGetBusClockFrequency() {
    if (!g_busfreq)
    {
        //that's a bit dirty :(
        _hook_scePowerSetClockFrequency(333, 333, 166);
    }
    return g_busfreq;
}


//Alias, see http://forums.ps2dev.org/viewtopic.php?t=11294
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq)
{
    int ret = scePower_EBD177D6(pllfreq, cpufreq, busfreq);
    if (ret >= 0)
    {
        g_pllfreq = pllfreq;
        g_cpufreq = cpufreq;
        g_busfreq = busfreq;
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
    int result = 0x01000000 * version / 100;
    result += 0x010000 * (version % 100) / 10;
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

int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int timeout)
{
    return sceKernelTryReceiveMsgPipe(uid, message, size, unk1, unk2);
}

int _hook_sceKernelExitDeleteThread(int status)
{
    int ret = sceKernelExitThread(status);
    //TODO add self to a list of threads to delete then let HBL handle the deletion
    return ret;
}   	

#ifdef LOAD_MODULE
SceUID _hook_sceKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{	
	LOGSTR0("_hook_sceKernelLoadModule\n");
	LOGSTR1("Attempting to load %s\n", path);
	
	SceUID elf_file = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (elf_file < 0)
	{
		LOGSTR2("Error 0x%08lX opening requested module %s\n", elf_file, path);
		return elf_file;
	}

	SceOff offset = 0;
	LOGSTR1("_hook_sceKernelLoadModule offset: 0x%08lX\n", offset);
	SceUID ret = load_module(elf_file, path, NULL, offset);

	LOGSTR1("load_module returned 0x%08lX\n", ret);

	return ret;
}

int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	LOGSTR0("_hook_sceKernelStartModule\n");
	
	SceUID ret = start_module(modid);
	LOGSTR1("start_module returned 0x%08lX\n", ret);
	
	return ret;
}
#endif


// A function that just returns "ok" but does nothing
// Note: in Sony's world, "0" means ok
int _hook_generic_ok()
{
    return 0;
}


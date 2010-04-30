#include "sdk.h"
#include "eloader.h"
#include "debug.h"
#include "malloc.h"

// Hooks for some functions used by Homebrews
// Hooks are put in place by resolve_imports() in resolve.c

typedef struct {
    u16 year;
    u16 month;
    u16 day;
    u16 hour;
    u16 minutes;
    u16 seconds;
    u32 microseconds;
} pspTime;

g_module_chdir = 0;

#ifdef FAKE_THREADS

SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority,
                             	   int stackSize, SceUInt attr, SceKernelThreadOptParam *option)
{ 
    u32 gp_bak = 0;
	SceUID res;
	
    if (gp) 
	{
        GET_GP(gp_bak);
        SET_GP(gp);
    }
	
    entry_point = entry;
    res = sceKernelCreateThread(name, entry, runThread, stackSize, attr, option);
	
    if (gp) 
	{
        SET_GP(gp_bak);
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

// will this work ?
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

int path_is_absolute(const char * file)
{
    int i;
    for (i = 0; i < strlen(file) && i < 9; ++i)
        if(file[i] == ':')
            return 1;

    return 0;
}

char * relative_to_absolute(const char * file)
{
    int i;
    int absolute = 0;
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

/* WIP
// A function that just returns "ok" but does nothing
int _hook_ok()
{
    return 1;
}
*/

#ifndef ELOADER_HOOK
#define ELOADER_HOOK

typedef struct 
{
    u16 year;
    u16 month;
    u16 day;
    u16 hour;
    u16 minutes;
    u16 seconds;
    u32 microseconds;
} pspTime;

extern int chdir_ok;

int test_sceIoChdir();

/* HOOKS */

// Thread manager
SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);

// Memory manager
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);

// File I/O manager 
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence);
SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode);
SceUID _hook_sceIoDopen(const char *dirname);
int _hook_sceIoChdir(const char *dirname) ;

// Control manager
int _hook_sceCtrlPeekBufferPositive(SceCtrlData* pad_data,int count);

// Audio manager
int _hook_sceAudioSRCChReserve (int samplecount, int freq, int channels);
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf);
int _hook_sceAudioSRCChRelease();

// Clock manager
u32 _hook_sceRtcGetTickResolution();
int _hook_sceRtcGetCurrentTick(u64 * tick);

// Module manager
SceUID _hook_sceKernelLoadModule (const char *path, int flags, SceKernelLMOption *option);
int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
void  _hook_sceKernelExitGame();


#endif
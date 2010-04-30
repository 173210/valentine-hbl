#ifndef ELOADER_HOOK
#define ELOADER_HOOK

SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
void  _hook_sceKernelExitGame();
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);

u32 _hook_sceRtcGetTickResolution(); 
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence) ;
int _hook_sceCtrlPeekBufferPositive(SceCtrlData* pad_data,int count);
int _hook_sceAudioSRCChReserve (int samplecount, int freq, int channels);
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf);
int _hook_sceAudioSRCChRelease();
int _hook_sceRtcGetCurrentTick(u64 * tick);

SceUID _hook_sceKernelLoadModule (const char *path, int flags, SceKernelLMOption *option);
int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);


int test_sceIoChdir();
SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode);
SceUID _hook_sceIoDopen(const char *dirname);
int _hook_sceIoChdir(const char *dirname) ;

char * g_module_chdir;

#endif
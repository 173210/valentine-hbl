#ifndef ELOADER_HOOK
#define ELOADER_HOOK

#include <psprtc.h>

extern int chdir_ok;

int test_sceIoChdir();

/* Declarations */
//files imported by Patapon but can't find proper .h file
int scePower_EBD177D6(int pllfreq, int cpufreq, int busfreq); 
int	ModuleMgrForUser_8F2DF740(int exitcode, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);


u32 setup_hook(u32 nid);

/* HOOKS */

// Thread manager
SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
int	_hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp);
int _hook_sceKernelTerminateDeleteThread(SceUID thid);
int _hook_sceKernelSleepThreadCB();
int _hook_sceKernelTrySendMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2);
int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int timeout);
int _hook_sceKernelExitDeleteThread(int status);
int _hook_sceKernelGetThreadCurrentPriority();

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
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf);

// RTC
u32 _hook_sceRtcGetTickResolution();
int _hook_sceRtcGetCurrentTick(u64 * tick);
int _hook_sceRtcGetCurrentClock  (pspTime  *time, int tz);
int _hook_sceRtcSetTick (pspTime  *time, const u64  *t);
int _hook_sceRtcConvertUtcToLocalTime  (const u64  *tickUTC, u64  *tickLocal);
int _hook_sceRtcGetTick  (const pspTime  *time, u64  *tick);

// Module manager
SceUID _hook_sceKernelLoadModule (const char *path, int flags, SceKernelLMOption *option);
int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
void  _hook_sceKernelExitGame();
int _hook_sceKernelSelfStopUnloadModule  (int exitcode, SceSize  argsize, void *argp);

// Power
int _hook_scePowerSetClockFrequency(int pllfreq, int cpufreq, int busfreq);
int _hook_scePowerGetCpuClockFrequencyInt();
int _hook_scePowerGetBatteryLifeTime();
int _hook_scePowerGetBatteryLifePercent();
int _hook_scePowerGetBusClockFrequency();

//SysMemUser
int _hook_sceKernelDevkitVersion();

int _hook_sceKernelUtilsMd5Digest  (u8  *data, u32  size, u8  *digest);

//Generic success
int _hook_generic_ok();


#endif
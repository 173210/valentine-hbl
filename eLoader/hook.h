#ifndef ELOADER_HOOK
#define ELOADER_HOOK

#include <psprtc.h>
#include <pspmath.h>

//Comment the following line if you don't want to hook thread creation
#define HOOK_THREADS

//Comment the following line if you don't want to monitor sleeping threads
#define DELETE_EXIT_THREADS

//Comment the following line if you don't want to monitor audio thread creation
#define MONITOR_AUDIO_THREADS

/*
 * Avoiding syscall estimations
 * The following override some major functions to avoid syscall estimations
 * Currently (until we can get 100% syscall estimation working ?) this increases
 * compatibility a LOT, but this alsow slows down some specific homebrews
 * and increases the size of HBL.
 * In the future if we can't achieve perfect syscall estimates, 
 * we will want to move these settings in a settings file, on a per homebrew basis
 * (see Noobz eLoader cfg file for reference on this)
 */

//Comment the following line to avoid overriding sceAudio*
#define HOOK_AUDIOFUNCTIONS

//Comment the following line to avoid overriding scePower*
#define HOOK_POWERFUNCTIONS

//Comment the following line to avoid overriding sceIo* (happens only if sceIoChdir fails)
#define HOOK_CHDIR_AND_FRIENDS

// Comment to avoid overriding sceUtility functions
#define HOOK_UTILITY

extern int chdir_ok;


//Own functions
int test_sceIoChdir();
void threads_cleanup();
void ram_cleanup();
void files_cleanup();
void subinterrupthandler_cleanup();

/* Declarations */
//files imported by Patapon but can't find proper .h file
int scePower_EBD177D6(int pllfreq, int cpufreq, int busfreq); 
int	ModuleMgrForUser_8F2DF740(int exitcode, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);


u32 setup_hook(u32 nid);
void exit_everything_but_me();

/* HOOKS */

//LoadExecForUser
int _hook_sceKernelRegisterExitCallback(int cbid);
void  _hook_sceKernelExitGame();

// Thread manager
SceUID _hook_sceKernelCreateThread(const char *name, void * entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
int	_hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp);
int _hook_sceKernelExitThread(int status);
int _hook_sceKernelExitDeleteThread(int status);
int _hook_sceKernelSleepThreadCB();
int _hook_sceKernelTrySendMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2);
int _hook_sceKernelReceiveMsgPipe(SceUID uid, void * message, unsigned int size, int unk1, void * unk2, int timeout);
int _hook_sceKernelGetThreadCurrentPriority();
int _hook_sceKernelCreateCallback(const char *name, SceKernelCallbackFunction func, void *arg);
int _hook_sceKernelGetThreadId();

// Memory manager
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);
int _hook_sceKernelFreePartitionMemory(SceUID uid);

// File I/O manager 
int _hook_sceIoLseek32 (SceUID  fd, int offset, int whence);
SceUID _hook_sceIoOpen(const char *file, int flags, SceMode mode);
SceUID _hook_sceIoDopen(const char *dirname);
int _hook_sceIoChdir(const char *dirname) ;
int _hook_sceIoClose(SceUID fd);

// Audio manager
int _hook_sceAudioSRCChReserve (int samplecount, int freq, int channels);
int _hook_sceAudioSRCOutputBlocking (int vol,void * buf);
int _hook_sceAudioSRCChRelease();
int _hook_sceAudioOutputBlocking(int channel,int vol,void * buf);
int _hook_sceAudioOutput2GetRestSample();
int _hook_sceAudioChReserve(int channel, int samplecount, int format);
int _hook_sceAudioChRelease(int channel);

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
int _hook_sceKernelSelfStopUnloadModule  (int exitcode, SceSize  argsize, void *argp);
int _hook_sceUtilityLoadModule(int id);
int _hook_sceUtilityLoadNetModule(int id);
int _hook_sceUtilityLoadAvModule(int id);
int _hook_sceUtilityLoadUsbModule(int id);
int _hook_sceUtilityUnloadModule(int id);

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
int _hook_generic_error();

// Mersene randomisation
int _hook_sceKernelUtilsMt19937Init(SceKernelUtilsMt19937Context *ctx, u32 seed);
u32 _hook_sceKernelUtilsMt19937UInt(SceKernelUtilsMt19937Context* ctx);

#endif

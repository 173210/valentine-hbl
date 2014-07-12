#ifndef ELOADER_HOOK
#define ELOADER_HOOK

#include <psprtc.h>

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
int scePower_469989AD(int pllfreq, int cpufreq, int busfreq);
int	ModuleMgrForUser_8F2DF740(int exitcode, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);


u32 setup_hook(u32 nid, u32 existing_real_call);
u32 setup_default_nid(u32 nid);
void exit_everything_but_me();

/* HOOKS */

// Thread manager
int _hook_sceKernelSleepThreadCB();
int	_hook_sceKernelStartThread(SceUID thid, SceSize arglen, void *argp);
SceUID _hook_sceKernelCreateThread(const char *name, void * entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
int _hook_sceKernelExitThread(int status);

// Memory manager
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);

#endif

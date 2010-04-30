#ifndef ELOADER_HOOK
#define ELOADER_HOOK

SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
void  _hook_sceKernelExitGame();
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);
SceUID _hook_sceKernelLoadModule (const char *path, int flags, SceKernelLMOption *option);
int	_hook_sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);

#endif
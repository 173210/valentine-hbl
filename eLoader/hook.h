#ifndef ELOADER_HOOK
#define ELOADER_HOOK

SceUID _hook_sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int currentPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
void  _hook_sceKernelExitGame();
int  _hook_sceKernelMaxFreeMemSize();
void *_hook_sceKernelGetBlockHeadAddr(SceUID mid);
SceUID _hook_sceKernelAllocPartitionMemory(SceUID partitionid, const char *name, int type, SceSize size, void *addr);

#endif
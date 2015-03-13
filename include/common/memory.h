#ifndef ELOADER_MEM
#define ELOADER_MEM

#include <common/sdk.h>
#include <config.h>

/* Overrides of sce functions to avoid syscall estimates */
SceSize hblKernelMaxFreeMemSize();
SceSize hblKernelTotalFreeMemSize();
int kill_thread(SceUID thid);
void subinterrupthandler_cleanup();
void UnloadModules();
#endif


#ifndef ELOADER_MEM
#define ELOADER_MEM

#include <common/sdk.h>
#include <exploit_config.h>

/* Overrides of sce functions to avoid syscall estimates */
SceSize sceKernelMaxFreeMemSize();
SceSize sceKernelTotalFreeMemSize();
int kill_thread(SceUID thid);
#ifdef SUB_INTR_HANDLER_CLEANUP
void subinterrupthandler_cleanup();
#endif
void UnloadModules();
#endif


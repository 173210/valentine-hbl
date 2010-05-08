#ifndef ELOADER_MEM
#define ELOADER_MEM

#include "sdk.h"

/* Free memory allocated by The game */
/* Warning: MUST NOT be called from the game's main thread */
void free_game_memory();

/* Overrides of sce functions to avoid syscall estimates */
SceSize sceKernelMaxFreeMemSize();
SceSize sceKernelTotalFreeMemSize();
int kill_thread(SceUID thid);

#endif


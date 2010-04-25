#ifndef ELOADER_MEM
#define ELOADER_MEM

#include "sdk.h"

/* Free memory allocated by The game */
/* Warning: MUST NOT be called from The game's main thread */
void free_game_memory();

/* Overrides of sce functions to avoid syscall estimates */
SceSize sceKernelMaxFreeMemSize();
SceSize sceKernelTotalFreeMemSize();

#endif


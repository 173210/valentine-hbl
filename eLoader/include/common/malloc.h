#ifndef ELOADER_MALLOC
#define ELOADER_MALLOC

#include <common/sdk.h>

// Max number of allocated memory blocks
#define MAX_ALLOCS 20

// Allocate memory
void *malloc_hbl(SceSize size);
// Free memory
void _free(void *ptr);

//Free all allocated blocks (for cleanup);
void free_allmalloc_hbls();

#endif


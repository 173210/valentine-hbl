#ifndef ELOADER_MALLOC
#define ELOADER_MALLOC

#include "sdk.h"

/* Max number of allocated memory blocks */
#define MAX_ALLOCS 1

/* Allocate memory */
void* malloc(u32 size);

/* Free memory */
void free(void* ptr);

#endif


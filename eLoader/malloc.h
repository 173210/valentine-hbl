#ifndef ELOADER_MALLOC
#define ELOADER_MALLOC

#include "sdk.h"

// Max number of allocated memory blocks
#define MAX_ALLOCS 20

/* Blocks structure */
typedef struct 
{
	SceUID uid;    // UID of block
	void* address; // Head address of block (0 if the block isn't allocated)
} HBLMemBlock;

// Intializes memory allocation system
void init_malloc(void);

// Allocate memory
void* malloc_p2(SceSize size);
void* malloc_hbl(SceSize size);

// Free memory
void free(void* ptr);

// Allocates memory from a given address
void * allocate_memory(u32 size, void* addr);

#endif


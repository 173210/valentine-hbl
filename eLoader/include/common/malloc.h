#ifndef ELOADER_MALLOC
#define ELOADER_MALLOC

#include <common/sdk.h>

// Max number of allocated memory blocks
#define MAX_ALLOCS 20

/* Blocks structure */
typedef struct 
{
	SceUID uid;    // UID of block
	void* address; // Head address of block (0 if the block isn't allocated)
} HBLMemBlock;


// Allocate memory
void* malloc_p2(SceSize size);
void* malloc_hbl(SceSize size);

// Free memory
void free(void* ptr);
//Free all allocated blocks (for cleanup);
void free_all_mallocs();

#endif


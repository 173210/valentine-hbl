#include <common/utils/string.h>
#include <common/sdk.h>
#include <hbl/eloader.h>
#include <common/malloc.h>
#include <common/debug.h>
#include <common/globals.h>

int find_free_block()
{
	int i;

	for(i = 0; globals->blockids[i]; i++)
		if(i >= MAX_ALLOCS)
			return -1;

	return i;
}

// Allocate memory
void *malloc_hbl(SceSize size)
{
	SceUID blockid;
    	LOGSTR("-->ALLOCATING MEMORY from partition 2, size 0x%08X... ", size);

	int i = find_free_block();
	if(i == MAX_ALLOCS) {
		LOGSTR("WARNING: no free blocks remaining\n");
		return NULL;
	}

	LOGSTR("Found free block %d\n", i);

	// Allocate block
	blockid = sceKernelAllocPartitionMemory(2, "ValentineMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
	if(blockid < 0) {
		LOGSTR("WARNING: malloc failed with error 0x%08X\n", blockid);
		return NULL;
	}

	LOGSTR("Got block from kernel with UID 0x%08X\n", blockid);

	// Fill block info
	globals->blockids[i] = blockid;

	return sceKernelGetBlockHeadAddr(blockid);
}

// Free memory
void _free(void *ptr)
{
	int i;

	for(i = 0; i < MAX_ALLOCS; i++)
		if(ptr == sceKernelGetBlockHeadAddr(globals->blockids[i])) {
			// Block found

			// Free block
			sceKernelFreePartitionMemory(globals->blockids[i]);

			// Re-initialize block
			globals->blockids[i] = 0;

			return;
		}
}

void free_allmalloc_hbls()
{
	int i;

	for(i = 0; i < MAX_ALLOCS; i++) {
		sceKernelFreePartitionMemory(globals->blockids[i]);

		// Re-initialize block
		globals->blockids[i] = 0;
	}
}


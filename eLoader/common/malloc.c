#include <common/sdk.h>
#include <hbl/eloader.h>
#include <common/malloc.h>
#include <common/debug.h>
#include <common/globals.h>

int find_free_block()
{
        int i;
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(!globals->blocks[i].address) // Found a free block
			break;
	}
    return i;
}

/* Allocate memory */
void* _malloc(SceSize size, int pnum)
{
	SceUID uid;
    	LOGSTR("-->ALLOCATING MEMORY from partition %d, size 0x%08X... ", (u32)pnum, (u32)size);

	int i = find_free_block();
	if(i == MAX_ALLOCS) // No free block found
	{
		LOGSTR("WARNING: no free blocks remaining\n");
		return NULL;
	}

	LOGSTR("Found free block %d\n", i);

	/* Allocate block */
	uid = sceKernelAllocPartitionMemory(pnum, "ValentineMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
	if(uid < 0) // Memory allocation failed
	{
		uid = sceKernelAllocPartitionMemory(pnum, "ValentineMalloc", PSP_SMEM_High, size, NULL); // Try to allocate from highest available address
		if(uid < 0) // Memory allocation failed
		{
			LOGSTR("WARNING: malloc failed with error 0x%08X\n", uid);
			return NULL;
		}
	}

	LOGSTR("Got block from kernel with UID 0x%08X\n", uid);

	/* Fill block info */
	globals->blocks[i].uid = uid;
	globals->blocks[i].address = sceKernelGetBlockHeadAddr(uid);

	return globals->blocks[i].address;
}

void free_all_mallocs() {
	int i;
    	for(i=0;i<MAX_ALLOCS;i++)
	{
        if(globals->blocks[i].address) { // Block found
            /* Free block */
            sceKernelFreePartitionMemory(globals->blocks[i].uid);

            /* Re-initialize block */
            memset(&(globals->blocks[i]), 0, sizeof(globals->blocks[i]));
        }
	}

	return;
}

/* Free memory */
void free(void* ptr)
{
	int i;
    	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(globals->blocks[i].address == ptr) // Block found
			break;
	}

	if(i == MAX_ALLOCS) // Block not found
		return;

	/* Free block */
	sceKernelFreePartitionMemory(globals->blocks[i].uid);

	/* Re-initialize block */
	memset(&(globals->blocks[i]), 0, sizeof(globals->blocks[i]));

	return;
}

void* malloc_p2(SceSize size)
{
	return _malloc(size, 2);
}

void* malloc_hbl(SceSize size)
{
    return _malloc(size, 2);
}
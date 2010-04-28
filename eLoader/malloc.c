#include "eloader.h"
#include "malloc.h"
#include "debug.h"

/* Number of allocated blocks */
u32 nblocks = 0;

/* Is the linked list initialized? */
u8 init = 0;

/* Blocks */
HBLMemBlock block[MAX_ALLOCS];

/* Allocate memory */
void* malloc(SceSize size)
{
	int i;
	SceUID uid;
	
	/* Initialize the linked list */
	if(!init)
	{
		memset(block, 0, sizeof(block));
		init = 1;
	}
	
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(!block[i].address) // Found a free block
			break;
	}
	
	if(i == MAX_ALLOCS) // No free block found
		return NULL;
	
	/* Allocate block */
	uid = sceKernelAllocPartitionMemory(2, "ValentineMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
	if(uid < 0) // Memory allocation failed
	{
		uid = sceKernelAllocPartitionMemory(2, "ValentineMalloc", PSP_SMEM_High, size, NULL); // Try to allocate from highest available address
		if(uid < 0) // Memory allocation failed
			return NULL;
	}
	
	/* Fill block info */
	block[i].uid = uid;
	block[i].address = sceKernelGetBlockHeadAddr(uid);
	
	return block[i].address;
}

/* Free memory */
void free(void* ptr)
{
	int i;
	
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(block[i].address == ptr) // Block found
			break;
	}
	
	if(i == MAX_ALLOCS) // Block not found
		return;
	
	/* Free block */
	sceKernelFreePartitionMemory(block[i].uid);
	
	/* Re-initialize block */
	memset(&block[i], 0, sizeof(block[i]));
	
	return;
}

// Allocates memory for homebrew so it doesn't overwrite itself
void allocate_memory(u32 size, void* addr)
{
	SceUID mem;
	
	LOGSTR1("-->ALLOCATING EXECUTABLE MEMORY @ 0x%08lX\n", addr);
	mem = sceKernelAllocPartitionMemory(2, "ELFMemory", PSP_SMEM_Addr, size, addr);
	if(mem < 0)
		LOGSTR1(" allocate_memory FAILED: 0x%08lX", mem);
}

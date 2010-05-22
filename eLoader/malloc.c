#include "sdk.h"
#include "eloader.h"
#include "malloc.h"
#include "debug.h"

/* Number of allocated blocks */
u32 nblocks;

/* Blocks */
HBLMemBlock block[MAX_ALLOCS];

void init_malloc(void)
{
	nblocks = 0;
	memset(block, 0, sizeof(block));
}

int find_free_block()
{
    int i;
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(!block[i].address) // Found a free block
			break;
	}
    return i;
}

/* Allocate memory */
void* _malloc(SceSize size, int pnum)
{
	SceUID uid;

	LOGSTR2("-->ALLOCATING MEMORY from partition %d, size 0x%08lX... ", (u32)pnum, (u32)size);
    
	int i = find_free_block();
	if(i == MAX_ALLOCS) // No free block found
	{
		LOGSTR0("WARNING: no free blocks remaining\n");
		return NULL;
	}

	LOGSTR1("Found free block %d\n", i);	

	/* Allocate block */
	//uid = sceKernelAllocPartitionMemory(2, "ValentineMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
	uid = sceKernelAllocPartitionMemory(pnum, "ValentineMalloc", PSP_SMEM_Low, size, NULL); // Try to allocate from the lowest available address
	if(uid < 0) // Memory allocation failed
	{
		//uid = sceKernelAllocPartitionMemory(2, "ValentineMalloc", PSP_SMEM_High, size, NULL); // Try to allocate from highest available address
		uid = sceKernelAllocPartitionMemory(pnum, "ValentineMalloc", PSP_SMEM_High, size, NULL); // Try to allocate from highest available address
		if(uid < 0) // Memory allocation failed
		{
			LOGSTR1("WARNING: malloc failed with error 0x%08lX\n", uid);
			return NULL;
		}
	}

	LOGSTR1("Got block from kernel with UID 0x%08lX\n", uid);
	
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

void* malloc_p2(SceSize size)
{
	return _malloc(size, 2);
}

void* malloc_p5(SceSize size)
{
	return _malloc(size, 5);
}

// Allocates memory for homebrew so it doesn't overwrite itself
void * allocate_memory(u32 size, void* addr)
{
	SceUID mem = 0;
	
    int i = find_free_block();
	if(i == MAX_ALLOCS) // No free block found
	{
		LOGSTR0("WARNING: no free blocks remaining\n");
		return NULL;
	}
    
    if (addr)
    {
        LOGSTR2("-->ALLOCATING MEMORY @ 0x%08lX size 0x%08lX... ", (u32)addr, size);
        mem = sceKernelAllocPartitionMemory(2, "ELFMemory", PSP_SMEM_Addr, size, addr);
    }
    else
    {
        addr = malloc_p2((SceSize)size);
    }
    
	if ((mem < 0) || (addr == NULL))
	{
		LOGSTR1("FAILED: 0x%08lX\n", mem);
        return NULL;
	}

    LOGSTR0("OK\n");
    
    /* Fill block info */
	block[i].uid = mem;
	block[i].address = sceKernelGetBlockHeadAddr(mem);
	
	return block[i].address;
}

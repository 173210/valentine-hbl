#include "eloader.h"
#include "malloc.h"
#include "debug.h"
#include "lib.h"

/* Number of allocated blocks */
u32 nblocks;

/* Blocks */
HBLMemBlock block[MAX_ALLOCS];

void init_malloc(void)
{
	nblocks = 0;
	memset(block, 0, sizeof(block));
}

SceUID _malloc(SceSize size)
{

    int types[] = {PSP_SMEM_Low, PSP_SMEM_High};
    int mem_partitions[] = {5, 2};
    u32 i, j;
    SceUID uid;
    
    for (i = 0; i < sizeof(mem_partitions)/sizeof(mem_partitions[0]); ++i)
    {
        for (j = 0; j < sizeof(types)/sizeof(types[0]); ++j)
        {
            /* Allocate block */
            uid = sceKernelAllocPartitionMemory(mem_partitions[i], "ValentineMalloc", types[j], size, NULL); // Try to allocate from the lowest available address
            if (uid >= 0)
            {
                return uid;
            }
        }
    }
    
    return uid;

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
void* malloc(SceSize size)
{

	SceUID uid;
	
    
	int i = find_free_block();
	if(i == MAX_ALLOCS) // No free block found
	{
		LOGSTR0("WARNING: no free blocks remaining\n");
		return NULL;
	}

	LOGSTR1("Found free block %d\n", i);
	
    uid = _malloc(size);
    if(uid < 0) // Memory allocation failed
    {
        LOGSTR1("WARNING: malloc failed with error 0x%08lX\n", uid);
        return NULL;
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

// Allocates memory for homebrew so it doesn't overwrite itself
void * allocate_memory(u32 size, void* addr)
{
	SceUID mem;
	
    int i = find_free_block();
	if(i == MAX_ALLOCS) // No free block found
	{
		LOGSTR0("WARNING: no free blocks remaining\n");
		return NULL;
	}
    
    if (addr)
    {
        LOGSTR1("-->ALLOCATING MEMORY @ 0x%08lX... ", (ULONG)addr);
        mem = sceKernelAllocPartitionMemory(2, "ELFMemory", PSP_SMEM_Addr, size, addr);
    }
    else
    {
        LOGSTR0("-->ALLOCATING MEMORY @ Low");
        mem = sceKernelAllocPartitionMemory(2, "ELFMemory", PSP_SMEM_Low, size, NULL);
    }
    
	if(mem < 0)
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

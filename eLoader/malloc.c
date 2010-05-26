#include "sdk.h"
#include "eloader.h"
#include "malloc.h"
#include "debug.h"
#include "globals.h"


void init_malloc(void)
{
    //now done in init globals
}

int find_free_block()
{
    tGlobals * g = get_globals();
    int i;
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(!g->block[i].address) // Found a free block
			break;
	}
    return i;
}

/* Allocate memory */
void* _malloc(SceSize size, int pnum)
{
	SceUID uid;
    tGlobals * g = get_globals();
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
	g->block[i].uid = uid;
	g->block[i].address = sceKernelGetBlockHeadAddr(uid);
	
	return g->block[i].address;
}

/* Free memory */
void free(void* ptr)
{
	int i;
    tGlobals * g = get_globals();	
	for(i=0;i<MAX_ALLOCS;i++)
	{
		if(g->block[i].address == ptr) // Block found
			break;
	}
	
	if(i == MAX_ALLOCS) // Block not found
		return;
	
	/* Free block */
	sceKernelFreePartitionMemory(g->block[i].uid);
	
	/* Re-initialize block */
	memset(&(g->block[i]), 0, sizeof(g->block[i]));
	
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

// Allocates memory for homebrew so it doesn't overwrite itself
void * allocate_memory(u32 size, void* addr)
{
    tGlobals * g = get_globals();
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
	g->block[i].uid = mem;
	g->block[i].address = sceKernelGetBlockHeadAddr(mem);
	
	return g->block[i].address;
}

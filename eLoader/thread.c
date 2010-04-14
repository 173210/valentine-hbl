#include "eloader.h"
#include "debug.h"

// Find a thread by name
SceUID find_thread(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelThreadInfo info;
	unsigned int attempt = 0;

	do
	{
		attempt++;
		ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
		if (ret < 0)
			reestimate_syscall((u32*) (ADDR_HBL_STUBS_BLOCK_ADDR + 0x00a0)); // sceKernelGetThreadmanIdList
	} while ((ret < 0) && (attempt <= MAX_REESTIMATE_ATTEMPTS));

	if (ret < 0)
		return ret;

	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		attempt = 0;
		do
		{
			attempt++;
			ret = sceKernelReferThreadStatus(readbuf[idcount-1], &info);
			if (ret < 0)
				reestimate_syscall((u32*) (ADDR_HBL_STUBS_BLOCK_ADDR + 0x0090)); // sceKernelReferThreadStatus
		} while ((ret < 0) && (attempt <= MAX_REESTIMATE_ATTEMPTS));
			
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferThreadStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
	
		if (strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

// Find a semaphore by name
SceUID find_sema(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelSemaInfo info;
	
	ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Semaphore, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);

	if (ret < 0)
		return ret;
	
	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		ret = sceKernelReferSemaStatus(readbuf[idcount-1], &info);
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferSemaStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
		
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

// Find an event flag by name
SceUID find_evflag(const char *name) 
{
	SceUID readbuf[256];
	int idcount, ret;
	SceKernelEventFlagInfo info;
	
	ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_EventFlag, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);

	if (ret < 0)
		return ret;

	for(info.size=sizeof(info); idcount>0; idcount--)
	{
		ret = sceKernelReferEventFlagStatus(readbuf[idcount-1], &info);
		if(ret < 0)
		{
			DEBUG_PRINT(" sceKernelReferEventFlagStatus FAILED ", &ret, sizeof(ret));
			return ret;
		}
		
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

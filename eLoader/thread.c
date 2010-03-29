#include "eloader.h"
#include "syscall.h"
#include "debug.h"

/* Find a thread by name */
SceUID find_thread(const char *name) {
	SceUID readbuf[256];
	int idcount;
	SceKernelThreadInfo info;
	
	
	#ifdef DEBUG
		SceUID fd;
	#endif
	
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GET ID LIST ", strlen(" GET ID LIST "));
			sceIoClose(fd);
		}
	#endif
	
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
	
	
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GOT ID LIST, COUNT ", strlen(" GOT ID LIST, COUNT "));
			sceIoWrite(fd, &idcount, sizeof(idcount));
			sceIoClose(fd);
		}
	#endif
	

	for(info.size=sizeof(info);idcount>0;idcount--)
	{
		
		if(sceKernelReferThreadStatus(readbuf[idcount-1], &info) < 0)
			return -1;
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

/* Find a semaphore by name */
SceUID find_sema(const char *name) {
	SceUID readbuf[256];
	int idcount;
	SceKernelSemaInfo info;
	
	/*
	#ifdef DEBUG
		SceUID fd;
	#endif
	
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GET ID LIST ", strlen(" GET ID LIST "));
			sceIoClose(fd);
		}
	#endif
	*/
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Semaphore, &readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
	
	/*
	#ifdef DEBUG
		fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_APPEND, 0777);
		if (fd >= 0)
		{
			sceIoWrite(fd, " GOT ID LIST, COUNT ", strlen(" GOT ID LIST, COUNT "));
			sceIoWrite(fd, &idcount, sizeof(idcount));
			sceIoClose(fd);
		}
	#endif
	*/

	for(info.size=sizeof(info);idcount>0;idcount--)
	{
		
		if(sceKernelReferSemaStatus(readbuf[idcount-1], &info) < 0)
			return -1;
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

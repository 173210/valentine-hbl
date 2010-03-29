#include "eloader.h"
#include "elf.h"
#include "syscall.h"
#include "thread.h"
#include "debug.h"

/* Find a module by name */
SceUID find_module(const char *name) {
	SceUID readbuf[256];
	int idcount;
	SceKernelModuleInfo info;
	
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
	
	sceKernelGetModuleIdList(readbuf, sizeof(readbuf)/sizeof(SceUID), &idcount);
	
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
		
		if(sceKernelQueryModuleInfo(readbuf[idcount-1], &info) < 0)
			return -1;
        DEBUG_PRINT(info.name, NULL, 0);
		if(strcmp(info.name, name) == 0)
			return readbuf[idcount-1];
	}
	
	return -1;
}

/*
// Get complete module info from an UID
int get_module_info(SceUID modid, tModuleInfo *modinfo)
{
	SceKernelModuleInfo info;
	void *ptr;
	
	// Get module info
	if(sceKernelQueryModuleInfo(modid, &info) < 0)
		return -1;
	
	// Create complete module info part
	modinfo->attribute = info.attribute;
	*((u16 *) &modinfo->version) = *((u16 *) &info.version);
	memcpy(&modinfo->name, &info.name, sizeof(modinfo->name));
	modinfo->gp_value = info.gp_value;
	
	// Get complete module info
	while(memcmp(ptr, modinfo, sizeof(tModuleInfo) - sizeof(u32)*4))
		ptr++;
	
	// Copy complete module info
	memcpy(modinfo, ptr, sizeof(tModuleInfo));
	
	return 0;
}
*/

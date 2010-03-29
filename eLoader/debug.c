#include "debug.h"
#include "sdk.h"

// Debugging log function
void write_debug(const char* description, void* value, unsigned int size)
{
	SceUID fd;
	
	if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777)) >= 0)
	{
	if (description != NULL)
		{
			sceIoWrite(PSPLINK_OUT, description, strlen(description));		
			sceIoWrite(fd, description, strlen(description));
		}
		if (value != NULL) {
			//sceIoWrite(PSPLINK_OUT, value, size);		
			sceIoWrite(fd, value, size);
		}
		sceIoClose(fd);
	}
}

void exit_with_log(const char* description, void* value, unsigned int size)
{
	write_debug(description, value, size);
	sceKernelExitGame();
}
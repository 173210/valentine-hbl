#include "debug.h"
#include "sdk.h"
#include "utils.h"

void init_debug(){
 SceUID fd;
 if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777)) >= 0)
 {
    sceIoClose(fd);
  }
}

void write_debug_newline (const char* description){
  write_debug(description, "\n", 1);
}
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
			sceIoWrite(PSPLINK_OUT, value, size);		
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

void logstr0(const char* A)
{
    write_debug(A, NULL, 0);
}

void logstr1(const char* A, unsigned long B)			
{
    char buff[512];
    mysprintf1(buff, A, (unsigned long)B);
    write_debug(buff, NULL, 0);
}

void logstr2(const char* A, unsigned long B, unsigned long C)		
{
    char buff[512];
    mysprintf2(buff, A, (unsigned long)B, (unsigned long)C);
    write_debug(buff, NULL, 0);
}

void logstr3(const char* A, unsigned long B, unsigned long C, unsigned long D)		
{
    char buff[512];
    mysprintf3(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D);
    write_debug(buff, NULL, 0);
}

void logstr4(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E)
{
    char buff[512];
    mysprintf4(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E);
    write_debug(buff, NULL, 0);
}

void logstr8(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I)
{
    char buff[512];
    mysprintf8(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D, (unsigned long)E, (unsigned long)F, (unsigned long)G, (unsigned long)H, (unsigned long)I);
    write_debug(buff, NULL, 0);
}
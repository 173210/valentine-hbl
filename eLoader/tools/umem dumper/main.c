/* User memory dumper by m0skit0 */
/* Dumps user memory */

#include "sdk.h"

#define START_ADDRESS 0x08800000
#define END_ADDRESS 0x09FFFFFF

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{
	SceUID dump_fd;

	dump_fd = sceIoOpen("ms0:/memdump", PSP_O_CREAT | PSP_O_WRONLY, 0777);

	if (dump_fd >= 0)
	{
		sceIoWrite(dump_fd, (void*) START_ADDRESS, END_ADDRESS - START_ADDRESS + 1);
		sceIoClose(dump_fd);
	}

	sceKernelDelayThread(100000);

	sceKernelExitGame();
}

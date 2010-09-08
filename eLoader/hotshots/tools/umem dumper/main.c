/* User memory dumper by m0skit0 */
/* Dumps user memory */

#include "sdk.h"

#define USER_START_ADDRESS 0x08800000
#define USER_END_ADDRESS 0x09FFFFFF

#define KERNEL_START_ADDRESS 0x08000000
#define KERNEL_END_ADDRESS 0x083FFFFF

// Entry point
void _start(unsigned long, unsigned long *) __attribute__ ((section (".text.start")));
void _start(unsigned long arglen, unsigned long *argp)
{
	SceUID dump_fd;

	dump_fd = sceIoOpen("ms0:/umem.dump", PSP_O_CREAT | PSP_O_WRONLY, 0777);

	if (dump_fd >= 0)
	{
		sceIoWrite(dump_fd, (void*) USER_START_ADDRESS, USER_END_ADDRESS - USER_START_ADDRESS + 1);
		sceIoClose(dump_fd);
	}

	dump_fd = sceIoOpen("ms0:/kmem.dump", PSP_O_CREAT | PSP_O_WRONLY, 0777);

	if (dump_fd >= 0)
	{
		sceIoWrite(dump_fd, (void*) KERNEL_START_ADDRESS, KERNEL_END_ADDRESS - KERNEL_START_ADDRESS + 1);
		sceIoClose(dump_fd);
	}

	sceKernelDelayThread(100000);

	sceKernelExitGame();
}

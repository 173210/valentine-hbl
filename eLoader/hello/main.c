#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("HelloWorld", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(256);

int main(int argc, char *argv[])
{
	SceCtrlData pad;
	
	pspDebugScreenInit();
	pspDebugScreenClear();

	printf("Hello World!\nPress X to exit\n\n");

	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS));
	
	printf("Exiting...\n");
	sceKernelExitGame();

	return 0;
}

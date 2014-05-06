#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psppower.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("HBL Tester", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(256);

int nbErrors = 0;

int ok()
{
    printf("ok\n");
    return 1;
}

int error()
{
    printf("ERROR\n");
    nbErrors++;
    return 0;
}

// Expect at least 20MB Ram Free
int testFreeRam()
{
    printf("sceKernelMaxFreeMemSize > 20MB...");
    int ram = sceKernelMaxFreeMemSize();
    if (ram < 20 * 1024 * 1024)
        return error();

    return ok();
}

int testFrequencies()
{
    int result = 1;
    scePowerSetClockFrequency(333, 333, 166);

    printf("scePowerGetCpuClockFrequency = 333...");
    int cpu = scePowerGetCpuClockFrequency();
    if (cpu < 332 ||cpu > 334) {
        result = error();
    } else {
        ok();
    }

    return result;

}

int main(int argc, char *argv[])
{
	SceCtrlData pad;

	pspDebugScreenInit();
	pspDebugScreenClear();

    printf("Testing HBL\n\n");

    testFreeRam();
    testFrequencies();


	printf("\nPress X to exit\n\n");

	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS));

	printf("Exiting...\n");
	sceKernelExitGame();

	return 0;
}

#include <pspsdk.h>
#include <pspkernel.h>

PSP_MODULE_INFO("hbl_launcher", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(100);
PSP_MAIN_THREAD_STACK_SIZE_KB(100);

int main(int argc, char *argv[])
{
	SceUID file = sceIoOpen("ms0:/H.BIN", PSP_O_RDONLY, 0777);
	sceIoRead(file, (void*)0x09000000, 1024 * 1024);
	sceIoClose(file);

	void (*entry)(void) = (void*)0x09000000;
	entry();
	
	return 0;
}

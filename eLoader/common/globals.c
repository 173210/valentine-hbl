#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/globals.h>
#include <common/utils.h>
#include <common/debug.h>
#include <exploit_config.h>

#ifndef VITA
// "cache" for the firmware version
// 0 means unknown

// cache for the psp model
// 0 means unknown
// 1 is PSPGO


// New method by neur0n to get the firmware version from the
// module_sdk_version export of sceKernelLibrary
// http://wololo.net/talk/viewtopic.php?f=4&t=128
static void detect_fw_ver()
{
	int i, cnt;

	SceLibraryEntryTable *tbl = (SceLibraryEntryTable *)
		(memfindsz("sceKernelLibrary", (void *)0x08800300, 0x00001000) + 8);

	cnt = tbl->vstubcount + tbl->stubcount;

	// LOG_PRINTF("Entry is 0x%08X \n", (int)Entry);
	// LOG_PRINTF("cnt is 0x%08X \n", cnt);

	for (i = 0; ((int *)tbl->entrytable)[i] != 0x11B97506; i--)
		if (i >= cnt) {
			LOG_PRINTF("Warning: Cannot find module_sdk_version function\n");
			return;
		}

	globals->fw_ver = *((int **)tbl->entrytable)[i + cnt];

	LOG_PRINTF("Detected firmware version is 0x%08X\n", globals->fw_ver);
}

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
static void detect_psp_go()
{
	// This call will always fail, but with a different error code depending on the model
	// Check for "No such device" error
	globals->psp_go = sceIoOpen("ef0:/", 1, 0777) == SCE_KERNEL_ERROR_NODEV;
}

static void detect_syscall()
{
	int i;
	short syscalls_known_fw[] = SYSCALLS_KNOWN_FW;
	short syscalls_known_go_fw[] = SYSCALLS_KNOWN_GO_FW;

	short *fw_array;
	int fw_array_size;

	if (globals->psp_go) {
		fw_array = syscalls_known_go_fw;
		fw_array_size = sizeof(syscalls_known_go_fw) / sizeof(short);
	} else {
		fw_array = syscalls_known_fw;
		fw_array_size = sizeof(syscalls_known_fw) / sizeof(short);
	}

	for (i = 0; i < fw_array_size; i++) {
		if (fw_array[i] == globals->fw_ver) {
			globals->syscalls_known = 1;
			break;
		}
	}
}
#endif
#endif

void init_globals()
{
        memset(globals, 0, sizeof(tGlobals));
#ifndef VITA
	// Intialize firmware and model
	detect_fw_ver();
#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	detect_psp_go();

	// Select syscall estimation method
	detect_syscall();
#endif
#endif
}


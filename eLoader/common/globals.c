#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/globals.h>
#include <common/debug.h>
#include <common/utils.h>
#include <exploit_config.h>

void init_globals()
{
        memset(globals, 0, sizeof(tGlobals));
#ifndef VITA
	// Intialize firmware and model
	get_fw_ver();
	getPSPModel();

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	// Select syscall estimation method
	int i;
	short syscalls_known_fw[] = SYSCALLS_KNOWN_FW;
	short syscalls_known_go_fw[] = SYSCALLS_KNOWN_GO_FW;

	short *fw_array;
	int fw_array_size;
	if (getPSPModel() == PSP_GO) {
		fw_array = syscalls_known_go_fw;
		fw_array_size = sizeof(syscalls_known_go_fw) / sizeof(short);
	} else {
		fw_array = syscalls_known_fw;
		fw_array_size = sizeof(syscalls_known_fw) / sizeof(short);
	}

	for (i = 0; i < fw_array_size; i++) {
		if (fw_array[i] == get_fw_ver()) {
			globals->syscalls_known = 1;
			break;
		}
	}
#endif
#endif
}


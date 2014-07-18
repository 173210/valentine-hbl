#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/globals.h>
#include <common/debug.h>
#include <common/utils.h>
#include <exploit_config.h>

void init_globals()
{
        memset(globals, 0, sizeof(tGlobals));
#ifdef VITA
	strcpy(globals->menupath, HBL_ROOT"EBOOT.PBP");
#else
	strcpy(globals->menupath, HBL_ROOT"menu/EBOOT.PBP");

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
	globals->cur_ch_id = -1;
}

#ifdef VITA
void reset_vita_dirs()
{
	int i, j;

	for (i = 0; i < MAX_OPEN_DIR_VITA; i++)
		for (j = 0; j < 2; j++)
 			globals->dirFix[i][j] = -1;
}
#endif



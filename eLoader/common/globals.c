#include <common/stubs/syscall.h>
#include <common/globals.h>
#include <common/debug.h>
#include <common/utils.h>
#include <exploit_config.h>

void init_globals()
{
    if ((u32)GLOBALS_ADDR < 0x14000)
    {
        if (HBL_STUBS_START + NUM_HBL_IMPORTS*2*sizeof(u32) > (u32)GLOBALS_ADDR)
        {
            exit_with_log("FATAL: Increase GLOBALS_ADDR", NULL, 0);
        }

        u32 size = sizeof(tGlobals);
        if (size + (u32)GLOBALS_ADDR >= 0x14000)
        {
            logstr1("Globals structure doesn't fit in Scratchpad RAM: 0x%08lX\n", size);
            exit_with_log("FATAL: Globals too big", NULL, 0);
        }
    }
        memset(globals, 0, sizeof(tGlobals));
#ifdef VITA
	strcpy(globals->menupath, HBL_ROOT"EBOOT.PBP");
#else
    strcpy(globals->menupath, HBL_ROOT"menu/EBOOT.PBP");
#endif
#ifndef VITA
	// Intialize firmware and model
	get_fw_ver();
	getPSPModel();

#ifndef DEACTIVATE_SYSCALL_ESTIMATION
	// Select syscall estimation method
	globals->syscalls_known = 0;

	int i;
	unsigned short syscalls_known_for_firmwares[] = SYSCALLS_KNOWN_FOR_FIRMWARES;
	unsigned short syscalls_known_for_go_firmwares[] = SYSCALLS_KNOWN_FOR_GO_FIRMWARES;

	unsigned short* firmware_array;
	int firmware_array_size;
	if (getPSPModel() == PSP_GO)
	{
		firmware_array = syscalls_known_for_go_firmwares;
		firmware_array_size = sizeof(syscalls_known_for_go_firmwares) / sizeof(unsigned short);
	}
	else
	{
		firmware_array = syscalls_known_for_firmwares;
		firmware_array_size = sizeof(syscalls_known_for_firmwares) / sizeof(unsigned short);
	}

	for (i = 0; i < firmware_array_size; i++)
	{
		if (firmware_array[i] == get_fw_ver())
		{
			globals->syscalls_known = 1;
			break;
		}
	}
#endif
#endif
	globals->syscalls_from_p5 = 1; // Always available

	globals->exit_cb_called = 0;

    globals->cur_ch_id = -1;

#ifdef VITA
    reset_vita_dirs();
#endif
}

#ifdef VITA
void reset_vita_dirs()
{
        int i,j;

    for (i=0;i<MAX_OPEN_DIR_VITA;++i)
        for (j=0;j<2;++j)
            globals->dirFix[i][j] = -1;
}
#endif
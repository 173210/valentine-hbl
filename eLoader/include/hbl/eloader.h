#ifndef ELOADER
#define ELOADER

#include <common/sdk.h>

// Maximum attempts to reestimate a syscall
#define MAX_REESTIMATE_ATTEMPTS 5

// Fixed loading address for relocatables
#define PRX_LOAD_ADDRESS 0x08900000

extern int chdir_ok;

extern int hook_exit_cb_called;
extern SceKernelCallbackFunction hook_exit_cb;

extern int num_pend_th;
extern int num_run_th;
extern int num_exit_th;

extern u32 gp;
extern u32 *entry_point;

#endif

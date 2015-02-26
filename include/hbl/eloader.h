#ifndef ELOADER
#define ELOADER

#include <common/sdk.h>

#define P2_PTR ((void*)0x08800000)
#define P2_SIZE (25165824)

#define EDRAM_SIZE (2097152)

// Fixed loading address for relocatables
#define PRX_LOAD_ADDRESS 0x08900000

extern int hook_exit_cb_called;
extern SceKernelCallbackFunction hook_exit_cb;

extern int num_pend_th;
extern int num_run_th;
extern int num_exit_th;

extern u32 gp;
extern u32 *entry_point;

#endif

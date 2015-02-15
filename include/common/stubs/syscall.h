#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

#include <common/sdk.h>
#include <exploit_config.h>

// Path for NID libraries
#define LIB_PATH HBL_ROOT "libs"
#define LIB_EXT ".nid"

SceUID open_nids_file(const char *lib);

/* Estimate a syscall */
/* Pass library name and NID */
/* Return syscall number */
int estimate_syscall(const char *lib, int nid);

#endif


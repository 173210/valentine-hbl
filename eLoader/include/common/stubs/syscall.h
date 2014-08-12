#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

#include <common/sdk.h>
#include <exploit_config.h>

#if defined(VITA) || !defined(SYSCALL_REF_LIB) \
	|| !(defined(SYSCALL_OFFSETS_500) || defined(SYSCALL_OFFSETS_500_CFW) \
	|| defined(SYSCALL_OFFSETS_550) || defined(SYSCALL_OFFSETS_550_CFW) \
	|| defined(SYSCALL_OFFSETS_570) || defined(SYSCALL_OFFSETS_570_GO) \
	|| defined(SYSCALL_OFFSETS_600) || defined(SYSCALL_OFFSETS_600_GO) \
	|| defined(SYSCALL_KERNEL_OFFSETS_620) \
	|| defined(SYSCALL_KERNEL_OFFSETS_630) \
	|| defined(SYSCALL_KERNEL_OFFSETS_635))
#define DEACTIVATE_SYSCALL_ESTIMATION
#endif

// Path for NID libraries
#define LIB_PATH HBL_ROOT "libs"
#define LIB_EXT ".nid"

typedef enum
{
	FROM_LOWER,
	FROM_HIGHER,
	SUBSTRACT,	// Only for reestimation
	ADD_TWICE,	// Only for reestimation
	FROM_LOWEST,
	FROM_CLOSEST
} HBLEstimateMethod;

SceUID open_nids_file(const char *lib);

/* Estimate a syscall */
/* Pass library name and NID */
/* Return syscall number */
int estimate_syscall(const char *lib, int nid, HBLEstimateMethod method);

#endif


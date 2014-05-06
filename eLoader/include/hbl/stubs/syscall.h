#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

#if defined(VITA) || !defined(SYSCALL_REFERENCE_LIBRARY) \
	|| !(defined(SYSCALL_OFFSETS_500) || defined(SYSCALL_OFFSETS_500_CFW) \
	|| defined(SYSCALL_OFFSETS_550) || defined(SYSCALL_OFFSETS_550_CFW) \
	|| defined(SYSCALL_OFFSETS_570) || defined(SYSCALL_OFFSETS_570_GO) \
	|| defined(SYSCALL_OFFSETS_600) || defined(SYSCALL_OFFSETS_600_GO) \
	|| defined(SYSCALL_KERNEL_OFFSETS_620) \
	|| defined(SYSCALL_KERNEL_OFFSETS_630) \
	|| defined(SYSCALL_KERNEL_OFFSETS_635))
#define DEACTIVATE_SYSCALL_ESTIMATION
#else
//Comment the following line to avoid reestimation
#define REESTIMATE_SYSCALL
#endif

typedef enum
{
	FROM_LOWER = 0,
	FROM_HIGHER = 1,
	SUBSTRACT = 2,		// Only for reestimation
	ADD_TWICE = 3,		// Only for reestimation
	FROM_LOWEST = 4,
	FROM_CLOSEST = 5
} HBLEstimateMethod;

/* Estimate a syscall */
/* Pass library name and NID */
/* Return syscall number */
u32 estimate_syscall(const char *lib, u32 nid, HBLEstimateMethod method);

u32 estimate_syscall_higher(int lib_index, u32 nid, SceUID nid_file);
u32 estimate_syscall_lower(int lib_index, u32 nid, SceUID nid_file);

/*
 * Reestimate a syscall if it's suspected of being incorrect
*/
u32 reestimate_syscall(const char * lib, u32 nid, u32* stub, HBLEstimateMethod type);

SceUID open_nids_file(const char* lib);

#endif


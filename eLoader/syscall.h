#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

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

#endif


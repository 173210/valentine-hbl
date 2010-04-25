#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

/* Estimate a syscall */
/* Pass library name and NID */
/* Return syscall number */
u32 estimate_syscall(const char *lib, u32 nid);

/*
 * Reestimate a syscall if it's suspected of being incorrect
*/
u32 reestimate_syscall(const char * lib, u32* stub);

#endif


#ifndef ELOADER_SYSCALL
#define ELOADER_SYSCALL

/* Estimate a syscall */
/* Pass library name and NID */
/* Return syscall number */
u32 estimate_syscall(const char *lib, u32 nid);

#endif


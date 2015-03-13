#include "pspimport.s"

#ifdef F_SysMemUserForUser_0000
	IMPORT_START	"SysMemUserForUser", 0x40000000
#elif defined(F_SysMemUserForUser_0001)
	IMPORT_FUNC	"SysMemUserForUser", 0x237DBD4F, sceKernelAllocPartitionMemory
#elif defined(F_SysMemUserForUser_0002)
	IMPORT_FUNC	"SysMemUserForUser", 0xB6D61D02, sceKernelFreePartitionMemory
#elif defined(F_SysMemUserForUser_0003)
	IMPORT_FUNC	"SysMemUserForUser", 0x9D9A5BA1, sceKernelGetBlockHeadAddr
#endif

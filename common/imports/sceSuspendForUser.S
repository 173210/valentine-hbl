#include "pspimport.s"

#ifdef F_sceSuspendForUser_0000
	IMPORT_START	"sceSuspendForUser", 0x40000000
#elif defined(F_sceSuspendForUser_0001)
	IMPORT_FUNC	"sceSuspendForUser", 0xA569E425, sceKernelVolatileMemUnlock
#endif

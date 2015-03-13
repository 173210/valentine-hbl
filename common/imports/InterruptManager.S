#include "pspimport.s"

#ifdef F_InterruptManager_0000
	IMPORT_START	"InterruptManager", 0x40000000
#elif defined(F_InterruptManager_0001)
	IMPORT_FUNC	"InterruptManager", 0xD61E6961, sceKernelReleaseSubIntrHandler
#endif

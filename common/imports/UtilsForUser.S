#include "pspimport.s"

#ifdef F_UtilsForUser_0000
	IMPORT_START	"UtilsForUser", 0x40010000
#elif defined(F_UtilsForUser_0001)
	IMPORT_FUNC	"UtilsForUser", 0x79D1C3FA, sceKernelDcacheWritebackAll
#elif defined(F_UtilsForUser_0002)
	IMPORT_FUNC	"UtilsForUser", 0xB435DEC5, sceKernelDcacheWritebackInvalidateAll
#elif defined(F_UtilsForUser_0003)
	IMPORT_FUNC	"UtilsForUser", 0x3EE30821, sceKernelDcacheWritebackRange
#elif defined(F_UtilsForUser_0004)
	IMPORT_FUNC	"UtilsForUser", 0x34B9FA9E, sceKernelDcacheWritebackInvalidateRange
#endif

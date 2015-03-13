#include "pspimport.s"

#ifdef F_ModuleMgrForUser_0000
	IMPORT_START	"ModuleMgrForUser", 0x40010000
#elif defined(F_ModuleMgrForUser_0001)
	IMPORT_FUNC	"ModuleMgrForUser", 0x977DE386, sceKernelLoadModule
#elif defined(F_ModuleMgrForUser_0002)
	IMPORT_FUNC	"ModuleMgrForUser", 0x50F0C1EC, sceKernelStartModule
#elif defined(F_ModuleMgrForUser_0003)
	IMPORT_FUNC	"ModuleMgrForUser", 0xD1FF982A, sceKernelStopModule
#elif defined(F_ModuleMgrForUser_0004)
	IMPORT_FUNC	"ModuleMgrForUser", 0x2E0911AA, sceKernelUnloadModule
#elif defined(F_ModuleMgrForUser_0005)
	IMPORT_FUNC	"ModuleMgrForUser", 0xD8B73127, sceKernelGetModuleIdByAddress
#elif defined(F_ModuleMgrForUser_0006)
	IMPORT_FUNC	"ModuleMgrForUser", 0x8F2DF740, ModuleMgrForUser_8F2DF740
#endif

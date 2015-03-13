#include "pspimport.s"

#ifdef F_ThreadManForUser_0000
	IMPORT_START	"ThreadManForUser", 0x40010000
#elif defined(F_ThreadManForUser_0001)
	IMPORT_FUNC	"ThreadManForUser", 0xE81CAF8F, sceKernelCreateCallback
#elif defined(F_ThreadManForUser_0002)
	IMPORT_FUNC	"ThreadManForUser", 0x82826F70, sceKernelSleepThreadCB
#elif defined(F_ThreadManForUser_0003)
	IMPORT_FUNC	"ThreadManForUser", 0xCEADEB47, sceKernelDelayThread
#elif defined(F_ThreadManForUser_0004)
	IMPORT_FUNC	"ThreadManForUser", 0x68DA9E36, sceKernelDelayThreadCB
#elif defined(F_ThreadManForUser_0005)
	IMPORT_FUNC	"ThreadManForUser", 0xD6DA4BA1, sceKernelCreateSema
#elif defined(F_ThreadManForUser_0006)
	IMPORT_FUNC	"ThreadManForUser", 0x28B6489C, sceKernelDeleteSema
#elif defined(F_ThreadManForUser_0007)
	IMPORT_FUNC	"ThreadManForUser", 0x3F53E640, sceKernelSignalSema
#elif defined(F_ThreadManForUser_0008)
	IMPORT_FUNC	"ThreadManForUser", 0x4E3A1105, sceKernelWaitSema
#elif defined(F_ThreadManForUser_0009)
	IMPORT_FUNC	"ThreadManForUser", 0x6D212BAC, sceKernelWaitSemaCB
#elif defined(F_ThreadManForUser_0010)
	IMPORT_FUNC	"ThreadManForUser", 0xEF9E4C70, sceKernelDeleteEventFlag
#elif defined(F_ThreadManForUser_0011)
	IMPORT_FUNC	"ThreadManForUser", 0x876DBFAD, sceKernelSendMsgPipe
#elif defined(F_ThreadManForUser_0012)
	IMPORT_FUNC	"ThreadManForUser", 0x884C9F90, sceKernelTrySendMsgPipe
#elif defined(F_ThreadManForUser_0013)
	IMPORT_FUNC	"ThreadManForUser", 0x74829B76, sceKernelReceiveMsgPipe
#elif defined(F_ThreadManForUser_0014)
	IMPORT_FUNC	"ThreadManForUser", 0xDF52098F, sceKernelTryReceiveMsgPipe
#elif defined(F_ThreadManForUser_0015)
	IMPORT_FUNC	"ThreadManForUser", 0x89B3D48C, sceKernelDeleteVpl
#elif defined(F_ThreadManForUser_0016)
	IMPORT_FUNC	"ThreadManForUser", 0xED1410E0, sceKernelDeleteFpl
#elif defined(F_ThreadManForUser_0017)
	IMPORT_FUNC	"ThreadManForUser", 0x7E65B999, sceKernelCancelAlarm
#elif defined(F_ThreadManForUser_0018)
	IMPORT_FUNC	"ThreadManForUser", 0x446D8DE6, sceKernelCreateThread
#elif defined(F_ThreadManForUser_0019)
	IMPORT_FUNC	"ThreadManForUser", 0x9FA03CD3, sceKernelDeleteThread
#elif defined(F_ThreadManForUser_0020)
	IMPORT_FUNC	"ThreadManForUser", 0xF475845D, sceKernelStartThread
#elif defined(F_ThreadManForUser_0021)
	IMPORT_FUNC	"ThreadManForUser", 0xAA73C935, sceKernelExitThread
#elif defined(F_ThreadManForUser_0022)
	IMPORT_FUNC	"ThreadManForUser", 0x809CE29B, sceKernelExitDeleteThread
#elif defined(F_ThreadManForUser_0023)
	IMPORT_FUNC	"ThreadManForUser", 0x616403BA, sceKernelTerminateThread
#elif defined(F_ThreadManForUser_0024)
	IMPORT_FUNC	"ThreadManForUser", 0x383F7BCC, sceKernelTerminateDeleteThread
#elif defined(F_ThreadManForUser_0025)
	IMPORT_FUNC	"ThreadManForUser", 0x293B45B8, sceKernelGetThreadId
#elif defined(F_ThreadManForUser_0026)
	IMPORT_FUNC	"ThreadManForUser", 0x60107536, sceKernelDeleteLwMutex
#endif

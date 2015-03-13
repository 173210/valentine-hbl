#include "pspimport.s"

#ifdef F_IoFileMgrForUser_0000
	IMPORT_START	"IoFileMgrForUser", 0x40010000
#elif defined(F_IoFileMgrForUser_0001)
	IMPORT_FUNC	"IoFileMgrForUser", 0x810C4BC3, sceIoClose
#elif defined(F_IoFileMgrForUser_0002)
	IMPORT_FUNC	"IoFileMgrForUser", 0x109F50BC, sceIoOpen
#elif defined(F_IoFileMgrForUser_0003)
	IMPORT_FUNC	"IoFileMgrForUser", 0x6A638D83, sceIoRead
#elif defined(F_IoFileMgrForUser_0004)
	IMPORT_FUNC	"IoFileMgrForUser", 0x42EC03AC, sceIoWrite
#elif defined(F_IoFileMgrForUser_0005)
	IMPORT_FUNC	"IoFileMgrForUser", 0x27EB27B8, sceIoLseek
#elif defined(F_IoFileMgrForUser_0006)
	IMPORT_FUNC	"IoFileMgrForUser", 0xB29DDF9C, sceIoDopen
#elif defined(F_IoFileMgrForUser_0007)
	IMPORT_FUNC	"IoFileMgrForUser", 0xE3EB004C, sceIoDread
#elif defined(F_IoFileMgrForUser_0008)
	IMPORT_FUNC	"IoFileMgrForUser", 0xEB092469, sceIoDclose
#elif defined(F_IoFileMgrForUser_0009)
	IMPORT_FUNC	"IoFileMgrForUser", 0xF27A9C51, sceIoRemove
#elif defined(F_IoFileMgrForUser_0010)
	IMPORT_FUNC	"IoFileMgrForUser", 0x06A70004, sceIoMkdir
#elif defined(F_IoFileMgrForUser_0011)
	IMPORT_FUNC	"IoFileMgrForUser", 0x55F4717D, sceIoChdir
#elif defined(F_IoFileMgrForUser_0012)
	IMPORT_FUNC	"IoFileMgrForUser", 0x779103A0, sceIoRename
#endif

	.set noreorder

#include "pspstub.s"

	STUB_START	"StdioForUser",0x40010011,0x00090005
	STUB_FUNC	0x0CBB0571,sceKernelStdioLseek
	STUB_FUNC	0x172D316E,sceKernelStdin
	STUB_FUNC	0x3054D478,sceKernelStdioRead
	STUB_FUNC	0x924ABA61,sceKernelStdioOpen
	STUB_FUNC	0x9D061C19,sceKernelStdioClose
	STUB_FUNC	0xA3B931DB,sceKernelStdioWrite
	STUB_FUNC	0xA46785C9,sceKernelStdioSendChar
	STUB_FUNC	0xA6BAB2E9,sceKernelStdout
	STUB_FUNC	0xF78BA90A,sceKernelStderr
	STUB_END

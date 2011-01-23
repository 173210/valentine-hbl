	.set noreorder

#include "pspstub.s"

	STUB_START	"sceSuspendForUser",0x40000011,0x00030005
	STUB_FUNC	0x090CCB3F,sceKernelPowerTick
	STUB_FUNC	0x3AEE7261,sceKernelPowerUnlock
	STUB_FUNC	0xEADB1BD7,sceKernelPowerLock
	STUB_END

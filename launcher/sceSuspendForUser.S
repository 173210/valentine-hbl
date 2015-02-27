	.set noreorder

#include "pspstub.s"

	STUB_START	"sceSuspendForUser",0x40000011,0x00060005
	STUB_FUNC	0x090CCB3F,sceKernelPowerTick
	STUB_FUNC	0x3AEE7261,sceKernelPowerUnlock
	STUB_FUNC	0x3E0271D3,sceSuspendForUser_3E0271D3
	STUB_FUNC	0xA14F40B2,sceSuspendForUser_A14F40B2
	STUB_FUNC	0xA569E425,sceSuspendForUser_A569E425
	STUB_FUNC	0xEADB1BD7,sceKernelPowerLock
	STUB_END

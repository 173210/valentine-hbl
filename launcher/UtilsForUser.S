	.set noreorder

#include "pspstub.s"

	STUB_START	"UtilsForUser",0x40010011,0x00180005
	STUB_FUNC	0x06FB8A63,sceKernelUtilsMt19937UInt
	STUB_FUNC	0x16641D70,sceKernelDcacheReadTag
	STUB_FUNC	0x27CC57F0,sceKernelLibcTime
	STUB_FUNC	0x346F6DA8,sceKernelUtilsSha1BlockUpdate
	STUB_FUNC	0x34B9FA9E,sceKernelDcacheWritebackInvalidateRange
	STUB_FUNC	0x37FB5C42,sceKernelGetGPI
	STUB_FUNC	0x3EE30821,sceKernelDcacheWritebackRange
	STUB_FUNC	0x4FD31C9D,sceKernelIcacheProbe
	STUB_FUNC	0x585F1C09,sceKernelUtilsSha1BlockResult
	STUB_FUNC	0x61E1E525,sceKernelUtilsMd5BlockUpdate
	STUB_FUNC	0x6AD345D7,sceKernelSetGPO
	STUB_FUNC	0x71EC4271,sceKernelLibcGettimeofday
	STUB_FUNC	0x79D1C3FA,sceKernelDcacheWritebackAll
	STUB_FUNC	0x80001C4C,sceKernelDcacheProbe
	STUB_FUNC	0x840259F1,sceKernelUtilsSha1Digest
	STUB_FUNC	0x91E4F6A7,sceKernelLibcClock
	STUB_FUNC	0x9E5C5086,sceKernelUtilsMd5BlockInit
	STUB_FUNC	0xB435DEC5,sceKernelDcacheWritebackInvalidateAll
	STUB_FUNC	0xB8D24E78,sceKernelUtilsMd5BlockResult
	STUB_FUNC	0xBFA98062,sceKernelDcacheInvalidateRange
	STUB_FUNC	0xC8186A58,sceKernelUtilsMd5Digest
	STUB_FUNC	0xE860E75E,sceKernelUtilsMt19937Init
	STUB_FUNC	0xF8FCD5BA,sceKernelUtilsSha1BlockInit
	STUB_FUNC	0xFB05FAD0,sceKernelIcacheReadTag
	STUB_END

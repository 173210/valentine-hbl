	.set noreorder

#include "pspstub.s"

	STUB_START	"SysMemUserForUser",0x40000011,0x00060005
	STUB_FUNC	0x237DBD4F,sceKernelAllocPartitionMemory
	STUB_FUNC	0x3FC9AE6A,sceKernelDevkitVersion
	STUB_FUNC	0x9D9A5BA1,sceKernelGetBlockHeadAddr
	STUB_FUNC	0xA291F107,sceKernelMaxFreeMemSize
	STUB_FUNC	0xB6D61D02,sceKernelFreePartitionMemory
	STUB_FUNC	0xF919F628,sceKernelTotalFreeMemSize
	STUB_END

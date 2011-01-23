	.set noreorder

#include "pspstub.s"

	STUB_START	"ModuleMgrForUser",0x40010011,0x000A0005
	STUB_FUNC	0x2E0911AA,sceKernelUnloadModule
	STUB_FUNC	0x50F0C1EC,sceKernelStartModule
	STUB_FUNC	0x710F61B5,sceKernelLoadModuleMs
	STUB_FUNC	0x748CBED9,sceKernelQueryModuleInfo
	STUB_FUNC	0x977DE386,sceKernelLoadModule
	STUB_FUNC	0xB7F46618,sceKernelLoadModuleByID
	STUB_FUNC	0xCC1D3699,sceKernelStopUnloadSelfModule
	STUB_FUNC	0xD1FF982A,sceKernelStopModule
	STUB_FUNC	0xD675EBB8,sceKernelSelfStopUnloadModule
	STUB_FUNC	0xF9275D98,sceKernelLoadModuleBufferUsbWlan
	STUB_END

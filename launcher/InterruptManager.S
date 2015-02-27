	.set noreorder

#include "pspstub.s"

	STUB_START	"InterruptManager",0x40000011,0x00090005
	STUB_FUNC	0x5CB5A78B,sceKernelSuspendSubIntr
	STUB_FUNC	0x7860E0DC,sceKernelResumeSubIntr
	STUB_FUNC	0x8A389411,sceKernelDisableSubIntr
	STUB_FUNC	0xCA04A2B9,sceKernelRegisterSubIntrHandler
	STUB_FUNC	0xD2E8363F,QueryIntrHandlerInfo
	STUB_FUNC	0xD61E6961,sceKernelReleaseSubIntrHandler
	STUB_FUNC	0xEEE43F47,sceKernelRegisterUserSpaceIntrStack
	STUB_FUNC	0xFB8E22EC,sceKernelEnableSubIntr
	STUB_FUNC	0xFC4374B8,sceKernelIsSubInterruptOccurred
	STUB_END

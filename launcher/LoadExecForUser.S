	.set noreorder

#include "pspstub.s"

	STUB_START	"LoadExecForUser",0x40010011,0x00070005
	STUB_FUNC	0x05572A5F,sceKernelExitGame
	STUB_FUNC	0x2AC9954B,sceKernelExitGameWithStatus
	STUB_FUNC	0x362A956B,LoadExecForUser_362A956B
	STUB_FUNC	0x4AC57943,sceKernelRegisterExitCallback
	STUB_FUNC	0x8ADA38D3,LoadExecForUser_8ADA38D3
	STUB_FUNC	0xBD2F1094,sceKernelLoadExec
	STUB_FUNC	0xD1FB50DC,LoadExecForUser_D1FB50DC
	STUB_END

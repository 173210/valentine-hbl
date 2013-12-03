.macro AddNID funcname, nid

	.globl  \funcname
	.ent    \funcname
\funcname = \nid
	.end    \funcname

.endm

	.file	1 "sdk.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	
	
	AddNID module_start, 0x0887d364
	AddNID module_stop, 0x0887d73c
	AddNID sceKernelCpuSuspendIntr, 0x089b5f48
	AddNID sceKernelCpuResumeIntr, 0x089b5f50
	AddNID sceRtcGetCurrentTick, 0x089b5f58
	AddNID sceRtcTickAddSeconds, 0x089b5f60
	AddNID sceUmdCheckMedium, 0x089b5f68
	AddNID sceUmdGetDriveStat, 0x089b5f70
	AddNID sceUmdWaitDriveStat, 0x089b5f78
	AddNID sceUmdRegisterUMDCallBack, 0x089b5f80
	AddNID sceUmdActivate, 0x089b5f88
	AddNID __sceSasSetADSR, 0x089b5f90
	AddNID __sceSasGetAllEnvelopeHeights, 0x089b5f98
	AddNID __sceSasRevParam, 0x089b5fa0
	AddNID __sceSasGetPauseFlag, 0x089b5fa8
	AddNID __sceSasRevType, 0x089b5fb0
	AddNID __sceSasInit, 0x089b5fb8
	AddNID __sceSasSetVolume, 0x089b5fc0
	AddNID __sceSasCoreWithMix, 0x089b5fc8
	AddNID __sceSasSetSL, 0x089b5fd0
	AddNID __sceSasGetEndFlag, 0x089b5fd8
	AddNID __sceSasGetEnvelopeHeight, 0x089b5fe0
	AddNID __sceSasSetKeyOn, 0x089b5fe8
	AddNID __sceSasSetPause, 0x089b5ff0
	AddNID __sceSasSetVoice, 0x089b5ff8
	AddNID __sceSasSetADSRmode, 0x089b6000
	AddNID __sceSasSetKeyOff, 0x089b6008
	AddNID __sceSasCore, 0x089b6010
	AddNID __sceSasSetPitch, 0x089b6018
	AddNID __sceSasSetNoise, 0x089b6020
	AddNID __sceSasGetGrain, 0x089b6028
	AddNID __sceSasSetSimpleADSR, 0x089b6030
	AddNID __sceSasSetGrain, 0x089b6038
	AddNID __sceSasRevEVOL, 0x089b6040
	AddNID __sceSasGetOutputmode, 0x089b6048
	AddNID __sceSasSetVoicePCM, 0x089b6050
	AddNID __sceSasSetOutputmode, 0x089b6058
	AddNID __sceSasRevVON, 0x089b6060
	AddNID sceAtracGetChannel, 0x089b6068
	AddNID sceAtracGetStreamDataInfo, 0x089b6070
	AddNID sceAtracReleaseAtracID, 0x089b6078
	AddNID sceAtracDecodeData, 0x089b6080
	AddNID sceAtracSetDataAndGetID, 0x089b6088
	AddNID sceAtracAddStreamData, 0x089b6090
	AddNID sceAtracSetLoopNum, 0x089b6098
	AddNID sceAtracGetMaxSample, 0x089b60a0
	AddNID sceAudioOutputBlocking, 0x089b60a8
	AddNID sceAudioOutputPannedBlocking, 0x089b60b0
	AddNID sceAudioChReserve, 0x089b60b8
	AddNID sceAudioChRelease, 0x089b60c0
	AddNID sceAudioChangeChannelConfig, 0x089b60c8
	AddNID sceAudioGetChannelRestLength, 0x089b60d0
	AddNID sceAudioChangeChannelVolume, 0x089b60d8
	AddNID sceAudioSetChannelDataLen, 0x089b60e0
	AddNID sceAudioOutputPanned, 0x089b60e8
	AddNID sceKernelLibcGettimeofday, 0x089b60f0
	AddNID sceKernelDcacheWritebackAll, 0x089b60f8
	AddNID sceKernelLibcClock, 0x089b6100
	AddNID sceKernelLibcTime, 0x089b6108
	AddNID sceKernelDelayThread, 0x089b6110
	AddNID sceKernelCreateSema, 0x089b6118
	AddNID sceKernelCreateCallback, 0x089b6120
	AddNID sceKernelChangeCurrentThreadAttr, 0x089b6128
	AddNID sceKernelDeleteCallback, 0x089b6130
	AddNID sceKernelStartThread, 0x089b6138
	AddNID sceKernelDeleteSema, 0x089b6140
	AddNID sceKernelGetThreadId, 0x089b6148
	AddNID sceKernelCheckCallback, 0x089b6150
	AddNID sceKernelGetSystemTimeLow, 0x089b6158
	AddNID sceKernelSignalSema, 0x089b6160
	AddNID sceKernelCreateThread, 0x089b6168
	AddNID sceKernelWaitSema, 0x089b6170
	AddNID sceKernelTerminateThread, 0x089b6178
	AddNID sceKernelDelayThreadCB, 0x089b6180
	AddNID sceKernelDeleteThread, 0x089b6188
	AddNID sceKernelExitThread, 0x089b6190
	AddNID sceKernelSetEventFlag, 0x089b6198
	AddNID sceKernelDeleteEventFlag, 0x089b61a0
	AddNID sceKernelWaitThreadEnd, 0x089b61a8
	AddNID sceKernelWaitEventFlag, 0x089b61b0
	AddNID sceKernelCreateEventFlag, 0x089b61b8
	AddNID sceKernelPrintf, 0x089b61c0
	AddNID sceKernelAllocPartitionMemory, 0x089b61c8
	AddNID SysMemUserForUser_91DE343C, 0x089b61d0
	AddNID sceKernelGetBlockHeadAddr, 0x089b61d8
	AddNID sceKernelFreePartitionMemory, 0x089b61e0
	AddNID sceKernelSetCompilerVersion, 0x089b61e8
	AddNID sceKernelStdin, 0x089b61f0
	AddNID sceKernelStdout, 0x089b61f8
	AddNID sceKernelStderr, 0x089b6200
	AddNID sceKernelUnloadModule, 0x089b6208
	AddNID sceKernelStopModule, 0x089b6210
	AddNID sceKernelGetModuleIdByAddress, 0x089b6218
	AddNID sceKernelGetModuleId, 0x089b6220
	AddNID sceKernelStartModule, 0x089b6228
	AddNID ModuleMgrForUser_8F2DF740, 0x089b6230
	AddNID sceKernelLoadModule, 0x089b6238
	AddNID sceKernelExitGame, 0x089b6240
	AddNID sceKernelRegisterExitCallback, 0x089b6248
	AddNID sceIoWrite, 0x089b6250
	AddNID sceIoRead, 0x089b6258
	AddNID sceIoRename, 0x089b6260
	AddNID sceIoClose, 0x089b6268
	AddNID sceIoReadAsync, 0x089b6270
	AddNID sceIoWaitAsync, 0x089b6278
	AddNID sceIoOpen, 0x089b6280
	AddNID sceIoLseek, 0x089b6288
	AddNID sceIoPollAsync, 0x089b6290
	AddNID scePowerRegisterCallback, 0x089b6298
	AddNID sceGeListSync, 0x089b62a0
	AddNID sceGeUnsetCallback, 0x089b62a8
	AddNID sceGeRestoreContext, 0x089b62b0
	AddNID sceGeListEnQueueHead, 0x089b62b8
	AddNID sceGeSaveContext, 0x089b62c0
	AddNID sceGeContinue, 0x089b62c8
	AddNID sceGeSetCallback, 0x089b62d0
	AddNID sceGeListEnQueue, 0x089b62d8
	AddNID sceGeDrawSync, 0x089b62e0
	AddNID sceGeEdramSetAddrTranslation, 0x089b62e8
	AddNID sceGeGetCmd, 0x089b62f0
	AddNID sceGeListUpdateStallAddr, 0x089b62f8
	AddNID sceGeEdramGetAddr, 0x089b6300
	AddNID sceDisplaySetMode, 0x089b6308
	AddNID sceDisplaySetFrameBuf, 0x089b6310
	AddNID sceDisplayWaitVblankStart, 0x089b6318
	AddNID sceDisplayGetVcount, 0x089b6320
	AddNID sceCtrlSetSamplingMode, 0x089b6328
	AddNID sceCtrlReadBufferPositive, 0x089b6330
	AddNID sceCtrlSetIdleCancelThreshold, 0x089b6338
	AddNID sceUtilityLoadModule, 0x089b6340
	AddNID sceUtilitySavedataInitStart, 0x089b6348
	AddNID sceUtilitySavedataGetStatus, 0x089b6350
	AddNID sceUtilitySavedataShutdownStart, 0x089b6358
	AddNID sceUtilitySavedataUpdate, 0x089b6360
	AddNID sceUtilityUnloadModule, 0x089b6368
	AddNID module_info, 0x089b64f8
	AddNID module_sdk_version, 0x089b68d8
	AddNID module_start_thread_parameter, 0x089b8e70
	AddNID module_stop_thread_parameter, 0x089b8e80



	.ident	"VAL-SDK"

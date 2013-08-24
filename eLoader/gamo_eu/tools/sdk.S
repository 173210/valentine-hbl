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
	
	
	AddNID sceKernelExitGame, 0x0888c374
	AddNID __sceSasSetPause, 0x0888c4d4
	AddNID sceKernelDelayThread, 0x0888c264
	AddNID sceRtcGetCurrentTick, 0x0888c3ec
	AddNID __sceSasCore, 0x0888c4f4
	AddNID __sceSasSetADSRmode, 0x0888c4e4
	AddNID sceGeListUpdateStallAddr, 0x0888c20c
	AddNID sceAudioOutputBlocking, 0x0888c594
	AddNID SysMemUserForUser_1B4217BC, 0x0888c314
	AddNID sceIoDevctl, 0x0888c3b4
	AddNID __sceSasSetADSR, 0x0888c474
	AddNID sceUtilityUnloadModule, 0x0888c444
	AddNID sceAtracAddStreamData, 0x0888c56c
	AddNID sceKernelRegisterExitCallback, 0x0888c37c
	AddNID sceGeDrawSync, 0x0888c1fc
	AddNID sceDisplaySetMode, 0x0888c21c
	AddNID __sceSasSetPitch, 0x0888c4fc
	AddNID __sceSasRevParam, 0x0888c484
	AddNID ModuleMgrForUser_8F2DF740, 0x0888c36c
	AddNID __sceSasGetPauseFlag, 0x0888c48c
	AddNID sceAtracSetLoopNum, 0x0888c574
	AddNID sceUtilityMsgDialogGetStatus, 0x0888c434
	AddNID sceKernelUnloadModule, 0x0888c34c
	AddNID __sceSasGetEndFlag, 0x0888c4bc
	AddNID __sceSasSetVoicePCM, 0x0888c534
	AddNID sceDisplayWaitVblankStart, 0x0888c22c
	AddNID sceDisplaySetFrameBuf, 0x0888c224
	AddNID __sceSasRevVON, 0x0888c544
	AddNID __sceSasSetSimpleADSR, 0x0888c514
	AddNID __sceSasSetKeyOn, 0x0888c4cc
	AddNID sceAudioChRelease, 0x0888c5ac
	AddNID Kernel_Library_293B45B8, 0x0888c39c
	AddNID sceKernelStderr, 0x0888c344
	AddNID __sceSasRevEVOL, 0x0888c524
	AddNID sceAtracSetDataAndGetID, 0x0888c564
	AddNID sceGeSetCallback, 0x0888c1ec
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x0888c254
	AddNID module_sdk_version, 0x0888cb40
	AddNID sceKernelLibcTime, 0x0888c25c
	AddNID sceAtracGetRemainFrame, 0x0888c57c
	AddNID sceKernelCreateSema, 0x0888c26c
	AddNID sceGeContinue, 0x0888c1e4
	AddNID sceGeUnsetCallback, 0x0888c1d4
	AddNID sceCtrlReadBufferPositive, 0x0888c234
	AddNID sceKernelLibcGettimeofday, 0x0888c23c
	AddNID __sceSasSetVoice, 0x0888c4dc
	AddNID __sceSasGetGrain, 0x0888c50c
	AddNID sceIoOpen, 0x0888c3d4
	AddNID sceIoRename, 0x0888c3c4
	AddNID sceAtracGetStreamDataInfo, 0x0888c54c
	AddNID sceUmdGetDriveStat, 0x0888c454
	AddNID sceKernelCpuSuspendIntr, 0x0888c384
	AddNID sceKernelStartThread, 0x0888c28c
	AddNID __sceSasRevType, 0x0888c494
	AddNID sceKernelWaitSema, 0x0888c2cc
	AddNID __sceSasSetVolume, 0x0888c4a4
	AddNID module_start_thread_parameter, 0x08891060
	AddNID module_start, 0x0884e154
	AddNID sceAtracDecodeData, 0x0888c55c
	AddNID sceKernelWaitEventFlag, 0x0888c2bc
	AddNID sceUmdActivate, 0x0888c46c
	AddNID sceUmdCancelWaitDriveStat, 0x0888c44c
	AddNID sceAtracGetInternalErrorInfo, 0x0888c58c
	AddNID sceUtilityMsgDialogInitStart, 0x0888c404
	AddNID sceKernelAllocPartitionMemory, 0x0888c31c
	AddNID sceAudioOutputPannedBlocking, 0x0888c59c
	AddNID sceIoWrite, 0x0888c3ac
	AddNID sceAudioOutputPanned, 0x0888c5d4
	AddNID __sceSasSetNoise, 0x0888c504
	AddNID sceIoRead, 0x0888c3bc
	AddNID sceKernelExitThread, 0x0888c2fc
	AddNID sceAudioChangeChannelVolume, 0x0888c5c4
	AddNID sceUtilitySavedataInitStart, 0x0888c40c
	AddNID sceUtilitySavedataGetStatus, 0x0888c41c
	AddNID sceAudioGetChannelRestLength, 0x0888c5bc
	AddNID sceKernelSetCompilerVersion, 0x0888c30c
	AddNID sceKernelPrintf, 0x0888c304
	AddNID sceAudioSetChannelDataLen, 0x0888c5cc
	AddNID sceKernelGetBlockHeadAddr, 0x0888c324
	AddNID sceIoLseek, 0x0888c3dc
	AddNID sceKernelExitDeleteThread, 0x0888c2dc
	AddNID sceKernelLibcClock, 0x0888c24c
	AddNID sceKernelPollEventFlag, 0x0888c2a4
	AddNID sceKernelWaitThreadEnd, 0x0888c294
	AddNID sceUmdRegisterUMDCallBack, 0x0888c464
	AddNID sceKernelDcacheWritebackAll, 0x0888c244
	AddNID sceKernelCreateCallback, 0x0888c27c
	AddNID sceKernelCreateThread, 0x0888c2c4
	AddNID sceKernelFreePartitionMemory, 0x0888c32c
	AddNID sceAudioChangeChannelConfig, 0x0888c5b4
	AddNID sceUtilityLoadModule, 0x0888c3fc
	AddNID sceKernelClearEventFlag, 0x0888c2e4
	AddNID __sceSasSetOutputmode, 0x0888c53c
	AddNID sceGeListSync, 0x0888c1cc
	AddNID Kernel_Library_1839852A, 0x0888c394
	AddNID sceUtilitySavedataShutdownStart, 0x0888c42c
	AddNID sceGeEdramGetAddr, 0x0888c214
	AddNID __sceSasSetKeyOff, 0x0888c4ec
	AddNID sceKernelStdout, 0x0888c33c
	AddNID sceGeListEnQueue, 0x0888c1f4
	AddNID sceKernelSleepThreadCB, 0x0888c2ec
	AddNID sceAtracReleaseAtracID, 0x0888c554
	AddNID sceKernelSetEventFlag, 0x0888c274
	AddNID __sceSasCoreWithMix, 0x0888c4ac
	AddNID sceKernelStopModule, 0x0888c354
	AddNID sceKernelStdin, 0x0888c334
	AddNID module_info, 0x0888c778
	AddNID sceUtilityMsgDialogShutdownStart, 0x0888c414
	AddNID sceIoClose, 0x0888c3cc
	AddNID Kernel_Library_A089ECA4, 0x0888c38c
	AddNID sceKernelDeleteSema, 0x0888c29c
	AddNID __sceSasInit, 0x0888c49c
	AddNID module_stop, 0x0884e55c
	AddNID sceKernelGetModuleIdByAddress, 0x0888c35c
	AddNID sceGeListEnQueueHead, 0x0888c1dc
	AddNID module_stop_thread_parameter, 0x08891070
	AddNID __sceSasGetOutputmode, 0x0888c52c
	AddNID sceKernelCreateEventFlag, 0x0888c2d4
	AddNID sceUtilitySavedataUpdate, 0x0888c43c
	AddNID __sceSasSetSL, 0x0888c4b4
	AddNID sceGeBreak, 0x0888c204
	AddNID sceKernelDeleteThread, 0x0888c2f4
	AddNID sceUtilityMsgDialogUpdate, 0x0888c424
	AddNID sceKernelCpuResumeIntr, 0x0888c3a4
	AddNID sceKernelGetModuleId, 0x0888c364
	AddNID __sceSasGetAllEnvelopeHeights, 0x0888c47c
	AddNID sceAtracGetSoundSample, 0x0888c584
	AddNID sceAudioChReserve, 0x0888c5a4
	AddNID scePowerRegisterCallback, 0x0888c3f4
	AddNID sceUmdWaitDriveStat, 0x0888c45c
	AddNID __sceSasGetEnvelopeHeight, 0x0888c4c4
	AddNID sceImposeSetLanguageMode, 0x0888c3e4
	AddNID __sceSasSetGrain, 0x0888c51c
	AddNID sceKernelSignalSema, 0x0888c2b4
	AddNID sceKernelDeleteEventFlag, 0x0888c284
	AddNID sceKernelGetSystemTimeLow, 0x0888c2ac
#These functions below are what I added. qwikrazor87
	AddNID sceKernelGetThreadId, 0x0888c39c
	AddNID sceKernelGetModuleGPByAddress, 0x08c80000
	AddNID sceOpenPSIDGetOpenPSID, 0x08c80010
	AddNID sceRtc_508BA64B, 0x08c80018
	AddNID sceRtcParseDateTime, 0x08c80020
	AddNID sceUtilityGetNetParam, 0x08c80028
	AddNID sceUtilityCheckNetParam, 0x08c80030
	AddNID sceUtilityGetSystemParamInt, 0x08c80038
	AddNID sceWlanGetSwitchState, 0x08c80040
	AddNID sceWlanDevAttach, 0x08c80048
	AddNID sceWlanDevDetach, 0x08c80050
	AddNID sceKernelQueryMemoryInfo, 0x08c80058
	AddNID sceKernelGetCompiledSdkVersion, 0x08c80068
	AddNID sceKernelGetSystemTimeWide, 0x08c80080
	AddNID sceKernelGetSystemTime, 0x08c80088
	AddNID sceKernelPollSema, 0x08c800e8
	AddNID sceKernelDeleteLwMutex, 0x08c800f0
	AddNID sceKernelTerminateThread, 0x08c800f8
	AddNID sceKernelSetAlarm, 0x08c80100
	AddNID sceKernelCancelAlarm, 0x08c80108
	AddNID sceKernelCreateLwMutex, 0x08c80128
	AddNID sceKernelDeleteMutex, 0x08c80138
	AddNID sceKernelReferVplStatus, 0x08c80150
	AddNID sceKernelCreateVpl, 0x08c80170
	AddNID sceKernelUnlockMutex, 0x08c80180
	AddNID sceKernelDeleteVpl, 0x08c80188
	AddNID sceKernelTryAllocateVpl, 0x08c801a0
	AddNID sceKernelLockMutex, 0x08c801a8
	AddNID sceKernelFreeVpl, 0x08c801b0
	AddNID sceKernelCreateMutex, 0x08c801b8
	AddNID sceKernelSendMbx, 0x08c801c8
	AddNID sceKernelDeleteMsgPipe, 0x08c801d8
	AddNID sceKernelPollMbx, 0x08c80238
	AddNID sceKernelReceiveMsgPipe, 0x08c80268
	AddNID sceKernelCreateMsgPipe, 0x08c80270
	AddNID sceKernelCreateMbx, 0x08c80278
	AddNID sceKernelDeleteMbx, 0x08c80288
	AddNID sceKernelSendMsgPipe, 0x08c80290
	AddNID sceKernelUtilsMd5BlockUpdate, 0x08c802d0
	AddNID sceKernelUtilsMd5BlockInit, 0x08c802d8
	AddNID sceKernelUtilsMd5BlockResult, 0x08c802e0
	AddNID sceKernelUtilsSha1BlockResult, 0x08c802e8
	AddNID sceKernelUtilsSha1BlockInit, 0x08c80308
	AddNID sceKernelUtilsSha1BlockUpdate, 0x08c80310



	.ident	"VAL-SDK"

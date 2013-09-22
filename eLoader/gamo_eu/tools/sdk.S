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
	
	
	AddNID module_start, 0x0884e154
	AddNID module_stop, 0x0884e55c
	AddNID sceGeListSync, 0x0888c1cc
	AddNID sceGeUnsetCallback, 0x0888c1d4
	AddNID sceGeListEnQueueHead, 0x0888c1dc
	AddNID sceGeContinue, 0x0888c1e4
	AddNID sceGeSetCallback, 0x0888c1ec
	AddNID sceGeListEnQueue, 0x0888c1f4
	AddNID sceGeDrawSync, 0x0888c1fc
	AddNID sceGeBreak, 0x0888c204
	AddNID sceGeListUpdateStallAddr, 0x0888c20c
	AddNID sceGeEdramGetAddr, 0x0888c214
	AddNID sceDisplaySetMode, 0x0888c21c
	AddNID sceDisplaySetFrameBuf, 0x0888c224
	AddNID sceDisplayWaitVblankStart, 0x0888c22c
	AddNID sceCtrlReadBufferPositive, 0x0888c234
	AddNID sceKernelLibcGettimeofday, 0x0888c23c
	AddNID sceKernelDcacheWritebackAll, 0x0888c244
	AddNID sceKernelLibcClock, 0x0888c24c
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x0888c254
	AddNID sceKernelLibcTime, 0x0888c25c
	AddNID sceKernelDelayThread, 0x0888c264
	AddNID sceKernelCreateSema, 0x0888c26c
	AddNID sceKernelSetEventFlag, 0x0888c274
	AddNID sceKernelCreateCallback, 0x0888c27c
	AddNID sceKernelDeleteEventFlag, 0x0888c284
	AddNID sceKernelStartThread, 0x0888c28c
	AddNID sceKernelWaitThreadEnd, 0x0888c294
	AddNID sceKernelDeleteSema, 0x0888c29c
	AddNID sceKernelPollEventFlag, 0x0888c2a4
	AddNID sceKernelGetSystemTimeLow, 0x0888c2ac
	AddNID sceKernelSignalSema, 0x0888c2b4
	AddNID sceKernelWaitEventFlag, 0x0888c2bc
	AddNID sceKernelCreateThread, 0x0888c2c4
	AddNID sceKernelWaitSema, 0x0888c2cc
	AddNID sceKernelCreateEventFlag, 0x0888c2d4
	AddNID sceKernelExitDeleteThread, 0x0888c2dc
	AddNID sceKernelClearEventFlag, 0x0888c2e4
	AddNID sceKernelSleepThreadCB, 0x0888c2ec
	AddNID sceKernelDeleteThread, 0x0888c2f4
	AddNID sceKernelExitThread, 0x0888c2fc
	AddNID sceKernelPrintf, 0x0888c304
	AddNID sceKernelSetCompilerVersion, 0x0888c30c
	AddNID SysMemUserForUser_1B4217BC, 0x0888c314
	AddNID sceKernelAllocPartitionMemory, 0x0888c31c
	AddNID sceKernelGetBlockHeadAddr, 0x0888c324
	AddNID sceKernelFreePartitionMemory, 0x0888c32c
	AddNID sceKernelStdin, 0x0888c334
	AddNID sceKernelStdout, 0x0888c33c
	AddNID sceKernelStderr, 0x0888c344
	AddNID sceKernelUnloadModule, 0x0888c34c
	AddNID sceKernelStopModule, 0x0888c354
	AddNID sceKernelGetModuleIdByAddress, 0x0888c35c
	AddNID sceKernelGetModuleId, 0x0888c364
	AddNID ModuleMgrForUser_8F2DF740, 0x0888c36c
	AddNID sceKernelExitGame, 0x0888c374
	AddNID sceKernelRegisterExitCallback, 0x0888c37c
	AddNID sceKernelCpuSuspendIntr, 0x0888c384
	AddNID Kernel_Library_A089ECA4, 0x0888c38c
	AddNID Kernel_Library_1839852A, 0x0888c394
	AddNID Kernel_Library_293B45B8, 0x0888c39c
	AddNID sceKernelCpuResumeIntr, 0x0888c3a4
	AddNID sceIoWrite, 0x0888c3ac
	AddNID sceIoDevctl, 0x0888c3b4
	AddNID sceIoRead, 0x0888c3bc
	AddNID sceIoRename, 0x0888c3c4
	AddNID sceIoClose, 0x0888c3cc
	AddNID sceIoOpen, 0x0888c3d4
	AddNID sceIoLseek, 0x0888c3dc
	AddNID sceImposeSetLanguageMode, 0x0888c3e4
	AddNID sceRtcGetCurrentTick, 0x0888c3ec
	AddNID scePowerRegisterCallback, 0x0888c3f4
	AddNID sceUtilityLoadModule, 0x0888c3fc
	AddNID sceUtilityMsgDialogInitStart, 0x0888c404
	AddNID sceUtilitySavedataInitStart, 0x0888c40c
	AddNID sceUtilityMsgDialogShutdownStart, 0x0888c414
	AddNID sceUtilitySavedataGetStatus, 0x0888c41c
	AddNID sceUtilityMsgDialogUpdate, 0x0888c424
	AddNID sceUtilitySavedataShutdownStart, 0x0888c42c
	AddNID sceUtilityMsgDialogGetStatus, 0x0888c434
	AddNID sceUtilitySavedataUpdate, 0x0888c43c
	AddNID sceUtilityUnloadModule, 0x0888c444
	AddNID sceUmdCancelWaitDriveStat, 0x0888c44c
	AddNID sceUmdGetDriveStat, 0x0888c454
	AddNID sceUmdWaitDriveStat, 0x0888c45c
	AddNID sceUmdRegisterUMDCallBack, 0x0888c464
	AddNID sceUmdActivate, 0x0888c46c
	AddNID __sceSasSetADSR, 0x0888c474
	AddNID __sceSasGetAllEnvelopeHeights, 0x0888c47c
	AddNID __sceSasRevParam, 0x0888c484
	AddNID __sceSasGetPauseFlag, 0x0888c48c
	AddNID __sceSasRevType, 0x0888c494
	AddNID __sceSasInit, 0x0888c49c
	AddNID __sceSasSetVolume, 0x0888c4a4
	AddNID __sceSasCoreWithMix, 0x0888c4ac
	AddNID __sceSasSetSL, 0x0888c4b4
	AddNID __sceSasGetEndFlag, 0x0888c4bc
	AddNID __sceSasGetEnvelopeHeight, 0x0888c4c4
	AddNID __sceSasSetKeyOn, 0x0888c4cc
	AddNID __sceSasSetPause, 0x0888c4d4
	AddNID __sceSasSetVoice, 0x0888c4dc
	AddNID __sceSasSetADSRmode, 0x0888c4e4
	AddNID __sceSasSetKeyOff, 0x0888c4ec
	AddNID __sceSasCore, 0x0888c4f4
	AddNID __sceSasSetPitch, 0x0888c4fc
	AddNID __sceSasSetNoise, 0x0888c504
	AddNID __sceSasGetGrain, 0x0888c50c
	AddNID __sceSasSetSimpleADSR, 0x0888c514
	AddNID __sceSasSetGrain, 0x0888c51c
	AddNID __sceSasRevEVOL, 0x0888c524
	AddNID __sceSasGetOutputmode, 0x0888c52c
	AddNID __sceSasSetVoicePCM, 0x0888c534
	AddNID __sceSasSetOutputmode, 0x0888c53c
	AddNID __sceSasRevVON, 0x0888c544
	AddNID sceAtracGetStreamDataInfo, 0x0888c54c
	AddNID sceAtracReleaseAtracID, 0x0888c554
	AddNID sceAtracDecodeData, 0x0888c55c
	AddNID sceAtracSetDataAndGetID, 0x0888c564
	AddNID sceAtracAddStreamData, 0x0888c56c
	AddNID sceAtracSetLoopNum, 0x0888c574
	AddNID sceAtracGetRemainFrame, 0x0888c57c
	AddNID sceAtracGetSoundSample, 0x0888c584
	AddNID sceAtracGetInternalErrorInfo, 0x0888c58c
	AddNID sceAudioOutputBlocking, 0x0888c594
	AddNID sceAudioOutputPannedBlocking, 0x0888c59c
	AddNID sceAudioChReserve, 0x0888c5a4
	AddNID sceAudioChRelease, 0x0888c5ac
	AddNID sceAudioChangeChannelConfig, 0x0888c5b4
	AddNID sceAudioGetChannelRestLength, 0x0888c5bc
	AddNID sceAudioChangeChannelVolume, 0x0888c5c4
	AddNID sceAudioSetChannelDataLen, 0x0888c5cc
	AddNID sceAudioOutputPanned, 0x0888c5d4
	AddNID module_info, 0x0888c778
	AddNID module_sdk_version, 0x0888cb40
	AddNID module_start_thread_parameter, 0x08891060
	AddNID module_stop_thread_parameter, 0x08891070



	.ident	"VAL-SDK"

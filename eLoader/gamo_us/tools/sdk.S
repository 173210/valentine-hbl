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
	
	
	AddNID sceGeBreak, 0x0888c1f8
	AddNID module_stop_thread_parameter, 0x08891058
	AddNID sceAtracGetRemainFrame, 0x0888c570
	AddNID sceUtilityMsgDialogUpdate, 0x0888c418
	AddNID sceKernelFreePartitionMemory, 0x0888c320
	AddNID __sceSasSetKeyOff, 0x0888c4e0
	AddNID __sceSasGetAllEnvelopeHeights, 0x0888c470
	AddNID sceIoClose, 0x0888c3c0
	AddNID sceKernelUnloadModule, 0x0888c340
	AddNID sceKernelGetModuleId, 0x0888c358
	AddNID sceKernelSetCompilerVersion, 0x0888c300
	AddNID sceUtilitySavedataGetStatus, 0x0888c410
	AddNID sceIoOpen, 0x0888c3c8
	AddNID __sceSasSetGrain, 0x0888c510
	AddNID sceKernelGetBlockHeadAddr, 0x0888c318
	AddNID sceGeListEnQueue, 0x0888c1e8
	AddNID sceCtrlReadBufferPositive, 0x0888c228
	AddNID sceKernelLibcTime, 0x0888c250
	AddNID sceImposeSetLanguageMode, 0x0888c3d8
	AddNID sceKernelStdout, 0x0888c330
	AddNID sceDisplayWaitVblankStart, 0x0888c220
	AddNID sceKernelExitThread, 0x0888c2f0
	AddNID __sceSasRevParam, 0x0888c478
	AddNID sceAudioGetChannelRestLength, 0x0888c5b0
	AddNID sceKernelSignalSema, 0x0888c2a8
	AddNID sceKernelCreateThread, 0x0888c2b8
	AddNID sceKernelCreateCallback, 0x0888c270
	AddNID __sceSasSetPause, 0x0888c4c8
	AddNID sceKernelCpuResumeIntr, 0x0888c398
	AddNID sceIoWrite, 0x0888c3a0
	AddNID sceKernelDeleteEventFlag, 0x0888c278
	AddNID sceKernelGetSystemTimeLow, 0x0888c2a0
	AddNID sceUtilityMsgDialogInitStart, 0x0888c3f8
	AddNID sceUtilityMsgDialogShutdownStart, 0x0888c408
	AddNID module_info, 0x0888c76c
	AddNID __sceSasSetVoice, 0x0888c4d0
	AddNID sceKernelLibcGettimeofday, 0x0888c230
	AddNID sceUmdGetDriveStat, 0x0888c448
	AddNID __sceSasRevVON, 0x0888c538
	AddNID sceUtilitySavedataShutdownStart, 0x0888c420
	AddNID sceIoRead, 0x0888c3b0
	AddNID sceUtilitySavedataInitStart, 0x0888c400
	AddNID sceGeContinue, 0x0888c1d8
	AddNID sceKernelSleepThreadCB, 0x0888c2e0
	AddNID module_start, 0x0884e148
	AddNID __sceSasSetVoicePCM, 0x0888c528
	AddNID sceUtilitySavedataUpdate, 0x0888c430
	AddNID sceIoDevctl, 0x0888c3a8
	AddNID sceKernelExitDeleteThread, 0x0888c2d0
	AddNID sceGeListEnQueueHead, 0x0888c1d0
	AddNID sceGeListSync, 0x0888c1c0
	AddNID sceKernelRegisterExitCallback, 0x0888c370
	AddNID sceAtracGetInternalErrorInfo, 0x0888c580
	AddNID __sceSasSetPitch, 0x0888c4f0
	AddNID sceDisplaySetMode, 0x0888c210
	AddNID sceKernelSetEventFlag, 0x0888c268
	AddNID sceKernelStopModule, 0x0888c348
	AddNID sceAtracDecodeData, 0x0888c550
	AddNID sceUmdRegisterUMDCallBack, 0x0888c458
	AddNID sceKernelGetModuleIdByAddress, 0x0888c350
	AddNID sceKernelWaitSema, 0x0888c2c0
	AddNID sceAtracSetDataAndGetID, 0x0888c558
	AddNID sceAudioOutputPannedBlocking, 0x0888c590
	AddNID sceKernelStdin, 0x0888c328
	AddNID sceGeEdramGetAddr, 0x0888c208
	AddNID sceGeListUpdateStallAddr, 0x0888c200
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x0888c248
	AddNID sceKernelDcacheWritebackAll, 0x0888c238
	AddNID SysMemUserForUser_1B4217BC, 0x0888c308
	AddNID sceDisplaySetFrameBuf, 0x0888c218
	AddNID __sceSasSetVolume, 0x0888c498
	AddNID sceKernelWaitEventFlag, 0x0888c2b0
	AddNID sceKernelStderr, 0x0888c338
	AddNID __sceSasSetOutputmode, 0x0888c530
	AddNID __sceSasGetGrain, 0x0888c500
	AddNID __sceSasSetADSR, 0x0888c468
	AddNID sceKernelWaitThreadEnd, 0x0888c288
	AddNID sceAudioChangeChannelVolume, 0x0888c5b8
	AddNID sceAudioChReserve, 0x0888c598
	AddNID sceAtracGetSoundSample, 0x0888c578
	AddNID module_stop, 0x0884e550
	AddNID sceUtilityLoadModule, 0x0888c3f0
	AddNID sceKernelCpuSuspendIntr, 0x0888c378
	AddNID sceKernelClearEventFlag, 0x0888c2d8
	AddNID sceKernelDelayThread, 0x0888c258
	AddNID __sceSasCore, 0x0888c4e8
	AddNID sceAtracAddStreamData, 0x0888c560
	AddNID __sceSasGetEndFlag, 0x0888c4b0
	AddNID sceAudioChRelease, 0x0888c5a0
	AddNID __sceSasCoreWithMix, 0x0888c4a0
	AddNID __sceSasRevEVOL, 0x0888c518
	AddNID sceAudioSetChannelDataLen, 0x0888c5c0
	AddNID __sceSasGetPauseFlag, 0x0888c480
	AddNID sceIoLseek, 0x0888c3d0
	AddNID sceKernelAllocPartitionMemory, 0x0888c310
	AddNID sceUmdActivate, 0x0888c460
	AddNID sceUtilityMsgDialogGetStatus, 0x0888c428
	AddNID ModuleMgrForUser_8F2DF740, 0x0888c360
	AddNID module_start_thread_parameter, 0x08891048
	AddNID sceAtracSetLoopNum, 0x0888c568
	AddNID sceAudioOutputBlocking, 0x0888c588
	AddNID sceGeDrawSync, 0x0888c1f0
	AddNID module_sdk_version, 0x0888cb40
	AddNID __sceSasSetADSRmode, 0x0888c4d8
	AddNID sceKernelStartThread, 0x0888c280
	AddNID __sceSasGetEnvelopeHeight, 0x0888c4b8
	AddNID sceUmdWaitDriveStat, 0x0888c450
	AddNID __sceSasSetNoise, 0x0888c4f8
	AddNID __sceSasSetKeyOn, 0x0888c4c0
	AddNID sceUmdCancelWaitDriveStat, 0x0888c440
	AddNID sceKernelCreateEventFlag, 0x0888c2c8
	AddNID sceRtcGetCurrentTick, 0x0888c3e0
	AddNID sceGeSetCallback, 0x0888c1e0
	AddNID sceKernelLibcClock, 0x0888c240
	AddNID __sceSasSetSimpleADSR, 0x0888c508
	AddNID sceKernelDeleteSema, 0x0888c290
	AddNID Kernel_Library_293B45B8, 0x0888c390
	AddNID Kernel_Library_A089ECA4, 0x0888c380
	AddNID sceAtracGetStreamDataInfo, 0x0888c540
	AddNID sceAtracReleaseAtracID, 0x0888c548
	AddNID sceKernelCreateSema, 0x0888c260
	AddNID sceKernelPollEventFlag, 0x0888c298
	AddNID sceUtilityUnloadModule, 0x0888c438
	AddNID sceAudioOutputPanned, 0x0888c5c8
	AddNID sceGeUnsetCallback, 0x0888c1c8
	AddNID __sceSasInit, 0x0888c490
	AddNID Kernel_Library_1839852A, 0x0888c388
	AddNID scePowerRegisterCallback, 0x0888c3e8
	AddNID __sceSasSetSL, 0x0888c4a8
	AddNID sceKernelDeleteThread, 0x0888c2e8
	AddNID sceKernelPrintf, 0x0888c2f8
	AddNID sceIoRename, 0x0888c3b8
	AddNID sceKernelExitGame, 0x0888c368
	AddNID __sceSasRevType, 0x0888c488
	AddNID sceAudioChangeChannelConfig, 0x0888c5a8
	AddNID __sceSasGetOutputmode, 0x0888c520
	AddNID sceKernelGetThreadId, 0x08800c18
	AddNID sceKernelTerminateThread, 0x08800f00
	AddNID sceKernelSendMsgPipe, 0x08801150
	AddNID sceUtilityCheckNetParam, 0x08800df0



	.ident	"VAL-SDK"

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
	
	
	AddNID module_start, 0x08896d70
	AddNID module_stop, 0x08897178
	AddNID sceImposeSetLanguageMode, 0x08926208
	AddNID sceRtcGetCurrentTick, 0x08926210
	AddNID sceRtcTickAddSeconds, 0x08926218
	AddNID sceUmdCheckMedium, 0x08926220
	AddNID sceUmdGetDriveStat, 0x08926228
	AddNID sceUmdWaitDriveStat, 0x08926230
	AddNID sceUmdRegisterUMDCallBack, 0x08926238
	AddNID sceUmdActivate, 0x08926240
	AddNID __sceSasSetADSR, 0x08926248
	AddNID __sceSasGetAllEnvelopeHeights, 0x08926250
	AddNID __sceSasRevParam, 0x08926258
	AddNID __sceSasGetPauseFlag, 0x08926260
	AddNID __sceSasRevType, 0x08926268
	AddNID __sceSasInit, 0x08926270
	AddNID __sceSasSetVolume, 0x08926278
	AddNID __sceSasCoreWithMix, 0x08926280
	AddNID __sceSasSetSL, 0x08926288
	AddNID __sceSasGetEndFlag, 0x08926290
	AddNID __sceSasGetEnvelopeHeight, 0x08926298
	AddNID __sceSasSetKeyOn, 0x089262a0
	AddNID __sceSasSetPause, 0x089262a8
	AddNID __sceSasSetVoice, 0x089262b0
	AddNID __sceSasSetADSRmode, 0x089262b8
	AddNID __sceSasSetKeyOff, 0x089262c0
	AddNID __sceSasCore, 0x089262c8
	AddNID __sceSasSetPitch, 0x089262d0
	AddNID __sceSasSetNoise, 0x089262d8
	AddNID __sceSasGetGrain, 0x089262e0
	AddNID __sceSasSetSimpleADSR, 0x089262e8
	AddNID __sceSasSetGrain, 0x089262f0
	AddNID __sceSasRevEVOL, 0x089262f8
	AddNID __sceSasGetOutputmode, 0x08926300
	AddNID __sceSasSetVoicePCM, 0x08926308
	AddNID __sceSasSetOutputmode, 0x08926310
	AddNID __sceSasRevVON, 0x08926318
	AddNID sceAtracGetChannel, 0x08926320
	AddNID sceAtracGetStreamDataInfo, 0x08926328
	AddNID sceAtracReleaseAtracID, 0x08926330
	AddNID sceAtracDecodeData, 0x08926338
	AddNID sceAtracSetDataAndGetID, 0x08926340
	AddNID sceAtracAddStreamData, 0x08926348
	AddNID sceAtracSetLoopNum, 0x08926350
	AddNID sceAtracGetMaxSample, 0x08926358
	AddNID sceAudioOutputBlocking, 0x08926360
	AddNID sceAudioOutputPannedBlocking, 0x08926368
	AddNID sceAudioChReserve, 0x08926370
	AddNID sceAudioChRelease, 0x08926378
	AddNID sceAudioChangeChannelConfig, 0x08926380
	AddNID sceAudioGetChannelRestLength, 0x08926388
	AddNID sceAudioChangeChannelVolume, 0x08926390
	AddNID sceAudioSetChannelDataLen, 0x08926398
	AddNID sceAudioOutputPanned, 0x089263a0
	AddNID sceKernelLibcGettimeofday, 0x089263a8
	AddNID sceKernelDcacheWritebackAll, 0x089263b0
	AddNID sceKernelLibcClock, 0x089263b8
	AddNID sceKernelLibcTime, 0x089263c0
	AddNID sceKernelDelayThread, 0x089263c8
	AddNID sceKernelCreateSema, 0x089263d0
	AddNID sceKernelCreateCallback, 0x089263d8
	AddNID sceKernelChangeCurrentThreadAttr, 0x089263e0
	AddNID sceKernelDeleteCallback, 0x089263e8
	AddNID sceKernelStartThread, 0x089263f0
	AddNID sceKernelDeleteSema, 0x089263f8
	AddNID sceKernelCheckCallback, 0x08926400
	AddNID sceKernelGetSystemTimeLow, 0x08926408
	AddNID sceKernelSignalSema, 0x08926410
	AddNID sceKernelCreateThread, 0x08926418
	AddNID sceKernelWaitSema, 0x08926420
	AddNID sceKernelTerminateThread, 0x08926428
	AddNID sceKernelDelayThreadCB, 0x08926430
	AddNID sceKernelExitDeleteThread, 0x08926438
	AddNID sceKernelDeleteThread, 0x08926440
	AddNID sceKernelExitThread, 0x08926448
	AddNID sceKernelSetEventFlag, 0x08926450
	AddNID sceKernelDeleteEventFlag, 0x08926458
	AddNID sceKernelWaitThreadEnd, 0x08926460
	AddNID sceKernelWaitEventFlag, 0x08926468
	AddNID sceKernelCreateEventFlag, 0x08926470
	AddNID sceKernelPrintf, 0x08926478
	AddNID sceKernelFreePartitionMemory, 0x08926480
	AddNID sceKernelSetCompilerVersion, 0x08926488
	AddNID sceKernelAllocPartitionMemory, 0x08926490
	AddNID SysMemUserForUser_358CA1BB, 0x08926498
	AddNID sceKernelGetBlockHeadAddr, 0x089264a0
	AddNID sceKernelStdin, 0x089264a8
	AddNID sceKernelStdout, 0x089264b0
	AddNID sceKernelStderr, 0x089264b8
	AddNID sceKernelUnloadModule, 0x089264c0
	AddNID sceKernelStopModule, 0x089264c8
	AddNID sceKernelGetModuleIdByAddress, 0x089264d0
	AddNID sceKernelGetModuleId, 0x089264d8
	AddNID sceKernelStartModule, 0x089264e0
	AddNID ModuleMgrForUser_8F2DF740, 0x089264e8
	AddNID sceKernelLoadModule, 0x089264f0
	AddNID sceKernelExitGame, 0x089264f8
	AddNID sceKernelRegisterExitCallback, 0x08926500
	AddNID sceKernelCpuSuspendIntr, 0x08926508
	AddNID Kernel_Library_293B45B8, 0x08926510
	AddNID sceKernelCpuResumeIntr, 0x08926518
	AddNID Kernel_Library_A089ECA4, 0x08926520
	AddNID Kernel_Library_1839852A, 0x08926528
	AddNID sceIoWrite, 0x08926530
	AddNID sceIoRead, 0x08926538
	AddNID sceIoRename, 0x08926540
	AddNID sceIoClose, 0x08926548
	AddNID sceIoReadAsync, 0x08926550
	AddNID sceIoWaitAsync, 0x08926558
	AddNID sceIoOpen, 0x08926560
	AddNID sceIoLseek, 0x08926568
	AddNID sceIoPollAsync, 0x08926570
	AddNID scePowerRegisterCallback, 0x08926578
	AddNID sceGeListSync, 0x08926580
	AddNID sceGeUnsetCallback, 0x08926588
	AddNID sceGeRestoreContext, 0x08926590
	AddNID sceGeListEnQueueHead, 0x08926598
	AddNID sceGeSaveContext, 0x089265a0
	AddNID sceGeContinue, 0x089265a8
	AddNID sceGeSetCallback, 0x089265b0
	AddNID sceGeListEnQueue, 0x089265b8
	AddNID sceGeDrawSync, 0x089265c0
	AddNID sceGeEdramSetAddrTranslation, 0x089265c8
	AddNID sceGeGetCmd, 0x089265d0
	AddNID sceGeListUpdateStallAddr, 0x089265d8
	AddNID sceGeEdramGetAddr, 0x089265e0
	AddNID sceDisplaySetMode, 0x089265e8
	AddNID sceDisplaySetFrameBuf, 0x089265f0
	AddNID sceDisplayWaitVblankStart, 0x089265f8
	AddNID sceDisplayGetVcount, 0x08926600
	AddNID sceCtrlSetSamplingMode, 0x08926608
	AddNID sceCtrlReadBufferPositive, 0x08926610
	AddNID sceCtrlSetSamplingCycle, 0x08926618
	AddNID sceCtrlSetIdleCancelThreshold, 0x08926620
	AddNID sceUtilityLoadModule, 0x08926628
	AddNID sceUtilitySavedataInitStart, 0x08926630
	AddNID sceUtilitySavedataGetStatus, 0x08926638
	AddNID sceUtilitySavedataShutdownStart, 0x08926640
	AddNID sceUtilityGetSystemParamInt, 0x08926648
	AddNID sceUtilitySavedataUpdate, 0x08926650
	AddNID sceUtilityUnloadModule, 0x08926658
	AddNID module_info, 0x089267fc
	AddNID syslib_11B97506, 0x08926bf0
	AddNID module_start_thread_parameter, 0x0892ee00
	AddNID module_stop_thread_parameter, 0x0892ee10



	.ident	"VAL-SDK"

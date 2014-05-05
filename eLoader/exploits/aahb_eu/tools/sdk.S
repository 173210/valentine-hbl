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
	
	
	AddNID module_start, 0x088b0a44
	AddNID module_stop, 0x088b0e1c
	AddNID sceImposeSetLanguageMode, 0x089dd344
	AddNID sceRtcGetCurrentTick, 0x089dd34c
	AddNID sceRtcTickAddSeconds, 0x089dd354
	AddNID sceUmdCheckMedium, 0x089dd35c
	AddNID sceUmdGetDriveStat, 0x089dd364
	AddNID sceUmdWaitDriveStat, 0x089dd36c
	AddNID sceUmdRegisterUMDCallBack, 0x089dd374
	AddNID sceUmdActivate, 0x089dd37c
	AddNID __sceSasSetADSR, 0x089dd384
	AddNID __sceSasGetAllEnvelopeHeights, 0x089dd38c
	AddNID __sceSasRevParam, 0x089dd394
	AddNID __sceSasGetPauseFlag, 0x089dd39c
	AddNID __sceSasRevType, 0x089dd3a4
	AddNID __sceSasInit, 0x089dd3ac
	AddNID __sceSasSetVolume, 0x089dd3b4
	AddNID __sceSasCoreWithMix, 0x089dd3bc
	AddNID __sceSasSetSL, 0x089dd3c4
	AddNID __sceSasGetEndFlag, 0x089dd3cc
	AddNID __sceSasGetEnvelopeHeight, 0x089dd3d4
	AddNID __sceSasSetKeyOn, 0x089dd3dc
	AddNID __sceSasSetPause, 0x089dd3e4
	AddNID __sceSasSetVoice, 0x089dd3ec
	AddNID __sceSasSetADSRmode, 0x089dd3f4
	AddNID __sceSasSetKeyOff, 0x089dd3fc
	AddNID __sceSasCore, 0x089dd404
	AddNID __sceSasSetPitch, 0x089dd40c
	AddNID __sceSasSetNoise, 0x089dd414
	AddNID __sceSasGetGrain, 0x089dd41c
	AddNID __sceSasSetSimpleADSR, 0x089dd424
	AddNID __sceSasSetGrain, 0x089dd42c
	AddNID __sceSasRevEVOL, 0x089dd434
	AddNID __sceSasGetOutputmode, 0x089dd43c
	AddNID __sceSasSetVoicePCM, 0x089dd444
	AddNID __sceSasSetOutputmode, 0x089dd44c
	AddNID __sceSasRevVON, 0x089dd454
	AddNID sceAtracGetChannel, 0x089dd45c
	AddNID sceAtracGetStreamDataInfo, 0x089dd464
	AddNID sceAtracReleaseAtracID, 0x089dd46c
	AddNID sceAtracDecodeData, 0x089dd474
	AddNID sceAtracSetDataAndGetID, 0x089dd47c
	AddNID sceAtracAddStreamData, 0x089dd484
	AddNID sceAtracSetLoopNum, 0x089dd48c
	AddNID sceAtracGetMaxSample, 0x089dd494
	AddNID sceAudioOutputBlocking, 0x089dd49c
	AddNID sceAudioOutputPannedBlocking, 0x089dd4a4
	AddNID sceAudioChReserve, 0x089dd4ac
	AddNID sceAudioChRelease, 0x089dd4b4
	AddNID sceAudioChangeChannelConfig, 0x089dd4bc
	AddNID sceAudioGetChannelRestLength, 0x089dd4c4
	AddNID sceAudioChangeChannelVolume, 0x089dd4cc
	AddNID sceAudioSetChannelDataLen, 0x089dd4d4
	AddNID sceAudioOutputPanned, 0x089dd4dc
	AddNID sceKernelLibcGettimeofday, 0x089dd4e4
	AddNID sceKernelDcacheWritebackAll, 0x089dd4ec
	AddNID sceKernelLibcClock, 0x089dd4f4
	AddNID sceKernelLibcTime, 0x089dd4fc
	AddNID sceKernelDelayThread, 0x089dd504
	AddNID sceKernelCreateSema, 0x089dd50c
	AddNID sceKernelCreateCallback, 0x089dd514
	AddNID sceKernelChangeCurrentThreadAttr, 0x089dd51c
	AddNID sceKernelDeleteCallback, 0x089dd524
	AddNID sceKernelStartThread, 0x089dd52c
	AddNID sceKernelDeleteSema, 0x089dd534
	AddNID sceKernelCheckCallback, 0x089dd53c
	AddNID sceKernelGetSystemTimeLow, 0x089dd544
	AddNID sceKernelSignalSema, 0x089dd54c
	AddNID sceKernelCreateThread, 0x089dd554
	AddNID sceKernelWaitSema, 0x089dd55c
	AddNID sceKernelTerminateThread, 0x089dd564
	AddNID sceKernelDelayThreadCB, 0x089dd56c
	AddNID sceKernelDeleteThread, 0x089dd574
	AddNID sceKernelExitThread, 0x089dd57c
	AddNID sceKernelSetEventFlag, 0x089dd584
	AddNID sceKernelDeleteEventFlag, 0x089dd58c
	AddNID sceKernelWaitThreadEnd, 0x089dd594
	AddNID sceKernelWaitEventFlag, 0x089dd59c
	AddNID sceKernelCreateEventFlag, 0x089dd5a4
	AddNID sceKernelPrintf, 0x089dd5ac
	AddNID sceKernelSetCompilerVersion, 0x089dd5b4
	AddNID sceKernelAllocPartitionMemory, 0x089dd5bc
	AddNID SysMemUserForUser_35669D4C, 0x089dd5c4
	AddNID sceKernelGetBlockHeadAddr, 0x089dd5cc
	AddNID sceKernelFreePartitionMemory, 0x089dd5d4
	AddNID sceKernelStdin, 0x089dd5dc
	AddNID sceKernelStdout, 0x089dd5e4
	AddNID sceKernelStderr, 0x089dd5ec
	AddNID sceKernelUnloadModule, 0x089dd5f4
	AddNID sceKernelStopModule, 0x089dd5fc
	AddNID sceKernelGetModuleIdByAddress, 0x089dd604
	AddNID sceKernelGetModuleId, 0x089dd60c
	AddNID sceKernelStartModule, 0x089dd614
	AddNID ModuleMgrForUser_8F2DF740, 0x089dd61c
	AddNID sceKernelLoadModule, 0x089dd624
	AddNID sceKernelExitGame, 0x089dd62c
	AddNID sceKernelRegisterExitCallback, 0x089dd634
	AddNID sceKernelCpuSuspendIntr, 0x089dd63c
	AddNID Kernel_Library_293B45B8, 0x089dd644
	AddNID sceKernelCpuResumeIntr, 0x089dd64c
	AddNID Kernel_Library_A089ECA4, 0x089dd654
	AddNID Kernel_Library_1839852A, 0x089dd65c
	AddNID sceIoWrite, 0x089dd664
	AddNID sceIoRead, 0x089dd66c
	AddNID sceIoRename, 0x089dd674
	AddNID sceIoClose, 0x089dd67c
	AddNID sceIoReadAsync, 0x089dd684
	AddNID sceIoWaitAsync, 0x089dd68c
	AddNID sceIoOpen, 0x089dd694
	AddNID sceIoLseek, 0x089dd69c
	AddNID sceIoPollAsync, 0x089dd6a4
	AddNID scePowerRegisterCallback, 0x089dd6ac
	AddNID sceGeListSync, 0x089dd6b4
	AddNID sceGeUnsetCallback, 0x089dd6bc
	AddNID sceGeRestoreContext, 0x089dd6c4
	AddNID sceGeListEnQueueHead, 0x089dd6cc
	AddNID sceGeSaveContext, 0x089dd6d4
	AddNID sceGeContinue, 0x089dd6dc
	AddNID sceGeSetCallback, 0x089dd6e4
	AddNID sceGeListEnQueue, 0x089dd6ec
	AddNID sceGeDrawSync, 0x089dd6f4
	AddNID sceGeEdramSetAddrTranslation, 0x089dd6fc
	AddNID sceGeGetCmd, 0x089dd704
	AddNID sceGeListUpdateStallAddr, 0x089dd70c
	AddNID sceGeEdramGetAddr, 0x089dd714
	AddNID sceDisplaySetMode, 0x089dd71c
	AddNID sceDisplaySetFrameBuf, 0x089dd724
	AddNID sceDisplayWaitVblankStart, 0x089dd72c
	AddNID sceDisplayGetVcount, 0x089dd734
	AddNID sceCtrlSetSamplingMode, 0x089dd73c
	AddNID sceCtrlReadBufferPositive, 0x089dd744
	AddNID sceCtrlSetSamplingCycle, 0x089dd74c
	AddNID sceCtrlSetIdleCancelThreshold, 0x089dd754
	AddNID sceUtilityLoadModule, 0x089dd75c
	AddNID sceUtilitySavedataInitStart, 0x089dd764
	AddNID sceUtilitySavedataGetStatus, 0x089dd76c
	AddNID sceUtilitySavedataShutdownStart, 0x089dd774
	AddNID sceUtilityGetSystemParamInt, 0x089dd77c
	AddNID sceUtilitySavedataUpdate, 0x089dd784
	AddNID sceUtilityUnloadModule, 0x089dd78c
	AddNID module_info, 0x089dd930
	AddNID syslib_11B97506, 0x089ddd20
	AddNID module_start_thread_parameter, 0x089e2d98
	AddNID module_stop_thread_parameter, 0x089e2da8



	.ident	"VAL-SDK"

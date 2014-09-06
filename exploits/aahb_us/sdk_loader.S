.macro AddNID funcname, nid

	.globl  \funcname
	.ent    \funcname
	.type	\funcname, @function
\funcname = \nid
	.end    \funcname
	.size	\funcname, 8

.endm

	.text
	.align	2


	AddNID module_start, 0x088b0608
	AddNID module_stop, 0x088b09e0
	AddNID sceRtcGetCurrentTick, 0x089dcd0c
	AddNID sceRtcTickAddSeconds, 0x089dcd14
	AddNID sceUmdCheckMedium, 0x089dcd1c
	AddNID sceUmdGetDriveStat, 0x089dcd24
	AddNID sceUmdWaitDriveStat, 0x089dcd2c
	AddNID sceUmdRegisterUMDCallBack, 0x089dcd34
	AddNID sceUmdActivate, 0x089dcd3c
	AddNID __sceSasSetADSR, 0x089dcd44
	AddNID __sceSasGetAllEnvelopeHeights, 0x089dcd4c
	AddNID __sceSasRevParam, 0x089dcd54
	AddNID __sceSasGetPauseFlag, 0x089dcd5c
	AddNID __sceSasRevType, 0x089dcd64
	AddNID __sceSasInit, 0x089dcd6c
	AddNID __sceSasSetVolume, 0x089dcd74
	AddNID __sceSasCoreWithMix, 0x089dcd7c
	AddNID __sceSasSetSL, 0x089dcd84
	AddNID __sceSasGetEndFlag, 0x089dcd8c
	AddNID __sceSasGetEnvelopeHeight, 0x089dcd94
	AddNID __sceSasSetKeyOn, 0x089dcd9c
	AddNID __sceSasSetPause, 0x089dcda4
	AddNID __sceSasSetVoice, 0x089dcdac
	AddNID __sceSasSetADSRmode, 0x089dcdb4
	AddNID __sceSasSetKeyOff, 0x089dcdbc
	AddNID __sceSasCore, 0x089dcdc4
	AddNID __sceSasSetPitch, 0x089dcdcc
	AddNID __sceSasSetNoise, 0x089dcdd4
	AddNID __sceSasGetGrain, 0x089dcddc
	AddNID __sceSasSetSimpleADSR, 0x089dcde4
	AddNID __sceSasSetGrain, 0x089dcdec
	AddNID __sceSasRevEVOL, 0x089dcdf4
	AddNID __sceSasGetOutputmode, 0x089dcdfc
	AddNID __sceSasSetVoicePCM, 0x089dce04
	AddNID __sceSasSetOutputmode, 0x089dce0c
	AddNID __sceSasRevVON, 0x089dce14
	AddNID sceAtracGetChannel, 0x089dce1c
	AddNID sceAtracGetStreamDataInfo, 0x089dce24
	AddNID sceAtracReleaseAtracID, 0x089dce2c
	AddNID sceAtracDecodeData, 0x089dce34
	AddNID sceAtracSetDataAndGetID, 0x089dce3c
	AddNID sceAtracAddStreamData, 0x089dce44
	AddNID sceAtracSetLoopNum, 0x089dce4c
	AddNID sceAtracGetMaxSample, 0x089dce54
	AddNID sceAudioOutputBlocking, 0x089dce5c
	AddNID sceAudioOutputPannedBlocking, 0x089dce64
	AddNID sceAudioChReserve, 0x089dce6c
	AddNID sceAudioChRelease, 0x089dce74
	AddNID sceAudioChangeChannelConfig, 0x089dce7c
	AddNID sceAudioGetChannelRestLength, 0x089dce84
	AddNID sceAudioChangeChannelVolume, 0x089dce8c
	AddNID sceAudioSetChannelDataLen, 0x089dce94
	AddNID sceAudioOutputPanned, 0x089dce9c
	AddNID sceKernelLibcGettimeofday, 0x089dcea4
	AddNID sceKernelDcacheWritebackAll, 0x089dceac
	AddNID sceKernelLibcClock, 0x089dceb4
	AddNID sceKernelLibcTime, 0x089dcebc
	AddNID sceKernelDelayThread, 0x089dcec4
	AddNID sceKernelCreateSema, 0x089dcecc
	AddNID sceKernelCreateCallback, 0x089dced4
	AddNID sceKernelChangeCurrentThreadAttr, 0x089dcedc
	AddNID sceKernelDeleteCallback, 0x089dcee4
	AddNID sceKernelStartThread, 0x089dceec
	AddNID sceKernelDeleteSema, 0x089dcef4
	AddNID sceKernelCheckCallback, 0x089dcefc
	AddNID sceKernelGetSystemTimeLow, 0x089dcf04
	AddNID sceKernelSignalSema, 0x089dcf0c
	AddNID sceKernelCreateThread, 0x089dcf14
	AddNID sceKernelWaitSema, 0x089dcf1c
	AddNID sceKernelTerminateThread, 0x089dcf24
	AddNID sceKernelDelayThreadCB, 0x089dcf2c
	AddNID sceKernelDeleteThread, 0x089dcf34
	AddNID sceKernelExitThread, 0x089dcf3c
	AddNID sceKernelSetEventFlag, 0x089dcf44
	AddNID sceKernelDeleteEventFlag, 0x089dcf4c
	AddNID sceKernelWaitThreadEnd, 0x089dcf54
	AddNID sceKernelWaitEventFlag, 0x089dcf5c
	AddNID sceKernelCreateEventFlag, 0x089dcf64
	AddNID sceKernelPrintf, 0x089dcf6c
	AddNID sceKernelSetCompilerVersion, 0x089dcf74
	AddNID sceKernelAllocPartitionMemory, 0x089dcf7c
	AddNID SysMemUserForUser_35669D4C, 0x089dcf84
	AddNID sceKernelGetBlockHeadAddr, 0x089dcf8c
	AddNID sceKernelFreePartitionMemory, 0x089dcf94
	AddNID sceKernelStdin, 0x089dcf9c
	AddNID sceKernelStdout, 0x089dcfa4
	AddNID sceKernelStderr, 0x089dcfac
	AddNID sceKernelUnloadModule, 0x089dcfb4
	AddNID sceKernelStopModule, 0x089dcfbc
	AddNID sceKernelGetModuleIdByAddress, 0x089dcfc4
	AddNID sceKernelGetModuleId, 0x089dcfcc
	AddNID sceKernelStartModule, 0x089dcfd4
	AddNID ModuleMgrForUser_8F2DF740, 0x089dcfdc
	AddNID sceKernelLoadModule, 0x089dcfe4
	AddNID sceKernelExitGame, 0x089dcfec
	AddNID sceKernelRegisterExitCallback, 0x089dcff4
	AddNID sceKernelCpuSuspendIntr, 0x089dcffc
	AddNID Kernel_Library_293B45B8, 0x089dd004
	AddNID sceKernelCpuResumeIntr, 0x089dd00c
	AddNID Kernel_Library_A089ECA4, 0x089dd014
	AddNID Kernel_Library_1839852A, 0x089dd01c
	AddNID sceIoWrite, 0x089dd024
	AddNID sceIoRead, 0x089dd02c
	AddNID sceIoRename, 0x089dd034
	AddNID sceIoClose, 0x089dd03c
	AddNID sceIoReadAsync, 0x089dd044
	AddNID sceIoWaitAsync, 0x089dd04c
	AddNID sceIoOpen, 0x089dd054
	AddNID sceIoLseek, 0x089dd05c
	AddNID sceIoPollAsync, 0x089dd064
	AddNID scePowerRegisterCallback, 0x089dd06c
	AddNID sceGeListSync, 0x089dd074
	AddNID sceGeUnsetCallback, 0x089dd07c
	AddNID sceGeRestoreContext, 0x089dd084
	AddNID sceGeListEnQueueHead, 0x089dd08c
	AddNID sceGeSaveContext, 0x089dd094
	AddNID sceGeContinue, 0x089dd09c
	AddNID sceGeSetCallback, 0x089dd0a4
	AddNID sceGeListEnQueue, 0x089dd0ac
	AddNID sceGeDrawSync, 0x089dd0b4
	AddNID sceGeEdramSetAddrTranslation, 0x089dd0bc
	AddNID sceGeGetCmd, 0x089dd0c4
	AddNID sceGeListUpdateStallAddr, 0x089dd0cc
	AddNID sceGeEdramGetAddr, 0x089dd0d4
	AddNID sceDisplaySetMode, 0x089dd0dc
	AddNID sceDisplaySetFrameBuf, 0x089dd0e4
	AddNID sceDisplayWaitVblankStart, 0x089dd0ec
	AddNID sceDisplayGetVcount, 0x089dd0f4
	AddNID sceCtrlSetSamplingMode, 0x089dd0fc
	AddNID sceCtrlReadBufferPositive, 0x089dd104
	AddNID sceCtrlSetSamplingCycle, 0x089dd10c
	AddNID sceCtrlSetIdleCancelThreshold, 0x089dd114
	AddNID sceUtilityLoadModule, 0x089dd11c
	AddNID sceUtilitySavedataInitStart, 0x089dd124
	AddNID sceUtilitySavedataGetStatus, 0x089dd12c
	AddNID sceUtilitySavedataShutdownStart, 0x089dd134
	AddNID sceUtilitySavedataUpdate, 0x089dd13c
	AddNID sceUtilityUnloadModule, 0x089dd144
	AddNID module_info, 0x089dd2d4
	AddNID syslib_11B97506, 0x089dd6b0
	AddNID module_start_thread_parameter, 0x089e2728
	AddNID module_stop_thread_parameter, 0x089e2738



	.ident	"VAL-SDK"

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
	
	
	AddNID module_start, 0x0887d348
	AddNID module_stop, 0x0887d720
	AddNID sceKernelCpuSuspendIntr, 0x089b5ecc
	AddNID sceKernelCpuResumeIntr, 0x089b5ed4
	AddNID sceRtcGetCurrentTick, 0x089b5edc
	AddNID sceRtcTickAddSeconds, 0x089b5ee4
	AddNID sceUmdCheckMedium, 0x089b5eec
	AddNID sceUmdGetDriveStat, 0x089b5ef4
	AddNID sceUmdWaitDriveStat, 0x089b5efc
	AddNID sceUmdRegisterUMDCallBack, 0x089b5f04
	AddNID sceUmdActivate, 0x089b5f0c
	AddNID __sceSasSetADSR, 0x089b5f14
	AddNID __sceSasGetAllEnvelopeHeights, 0x089b5f1c
	AddNID __sceSasRevParam, 0x089b5f24
	AddNID __sceSasGetPauseFlag, 0x089b5f2c
	AddNID __sceSasRevType, 0x089b5f34
	AddNID __sceSasInit, 0x089b5f3c
	AddNID __sceSasSetVolume, 0x089b5f44
	AddNID __sceSasCoreWithMix, 0x089b5f4c
	AddNID __sceSasSetSL, 0x089b5f54
	AddNID __sceSasGetEndFlag, 0x089b5f5c
	AddNID __sceSasGetEnvelopeHeight, 0x089b5f64
	AddNID __sceSasSetKeyOn, 0x089b5f6c
	AddNID __sceSasSetPause, 0x089b5f74
	AddNID __sceSasSetVoice, 0x089b5f7c
	AddNID __sceSasSetADSRmode, 0x089b5f84
	AddNID __sceSasSetKeyOff, 0x089b5f8c
	AddNID __sceSasCore, 0x089b5f94
	AddNID __sceSasSetPitch, 0x089b5f9c
	AddNID __sceSasSetNoise, 0x089b5fa4
	AddNID __sceSasGetGrain, 0x089b5fac
	AddNID __sceSasSetSimpleADSR, 0x089b5fb4
	AddNID __sceSasSetGrain, 0x089b5fbc
	AddNID __sceSasRevEVOL, 0x089b5fc4
	AddNID __sceSasGetOutputmode, 0x089b5fcc
	AddNID __sceSasSetVoicePCM, 0x089b5fd4
	AddNID __sceSasSetOutputmode, 0x089b5fdc
	AddNID __sceSasRevVON, 0x089b5fe4
	AddNID sceAtracGetChannel, 0x089b5fec
	AddNID sceAtracGetStreamDataInfo, 0x089b5ff4
	AddNID sceAtracReleaseAtracID, 0x089b5ffc
	AddNID sceAtracDecodeData, 0x089b6004
	AddNID sceAtracSetDataAndGetID, 0x089b600c
	AddNID sceAtracAddStreamData, 0x089b6014
	AddNID sceAtracSetLoopNum, 0x089b601c
	AddNID sceAtracGetMaxSample, 0x089b6024
	AddNID sceAudioOutputBlocking, 0x089b602c
	AddNID sceAudioOutputPannedBlocking, 0x089b6034
	AddNID sceAudioChReserve, 0x089b603c
	AddNID sceAudioChRelease, 0x089b6044
	AddNID sceAudioChangeChannelConfig, 0x089b604c
	AddNID sceAudioGetChannelRestLength, 0x089b6054
	AddNID sceAudioChangeChannelVolume, 0x089b605c
	AddNID sceAudioSetChannelDataLen, 0x089b6064
	AddNID sceAudioOutputPanned, 0x089b606c
	AddNID sceKernelLibcGettimeofday, 0x089b6074
	AddNID sceKernelDcacheWritebackAll, 0x089b607c
	AddNID sceKernelLibcClock, 0x089b6084
	AddNID sceKernelLibcTime, 0x089b608c
	AddNID sceKernelDelayThread, 0x089b6094
	AddNID sceKernelCreateSema, 0x089b609c
	AddNID sceKernelCreateCallback, 0x089b60a4
	AddNID sceKernelChangeCurrentThreadAttr, 0x089b60ac
	AddNID sceKernelDeleteCallback, 0x089b60b4
	AddNID sceKernelStartThread, 0x089b60bc
	AddNID sceKernelDeleteSema, 0x089b60c4
	AddNID sceKernelGetThreadId, 0x089b60cc
	AddNID sceKernelCheckCallback, 0x089b60d4
	AddNID sceKernelGetSystemTimeLow, 0x089b60dc
	AddNID sceKernelSignalSema, 0x089b60e4
	AddNID sceKernelCreateThread, 0x089b60ec
	AddNID sceKernelWaitSema, 0x089b60f4
	AddNID sceKernelTerminateThread, 0x089b60fc
	AddNID sceKernelDelayThreadCB, 0x089b6104
	AddNID sceKernelDeleteThread, 0x089b610c
	AddNID sceKernelExitThread, 0x089b6114
	AddNID sceKernelSetEventFlag, 0x089b611c
	AddNID sceKernelDeleteEventFlag, 0x089b6124
	AddNID sceKernelWaitThreadEnd, 0x089b612c
	AddNID sceKernelWaitEventFlag, 0x089b6134
	AddNID sceKernelCreateEventFlag, 0x089b613c
	AddNID sceKernelPrintf, 0x089b6144
	AddNID sceKernelAllocPartitionMemory, 0x089b614c
	AddNID SysMemUserForUser_91DE343C, 0x089b6154
	AddNID sceKernelGetBlockHeadAddr, 0x089b615c
	AddNID sceKernelFreePartitionMemory, 0x089b6164
	AddNID sceKernelSetCompilerVersion, 0x089b616c
	AddNID sceKernelStdin, 0x089b6174
	AddNID sceKernelStdout, 0x089b617c
	AddNID sceKernelStderr, 0x089b6184
	AddNID sceKernelUnloadModule, 0x089b618c
	AddNID sceKernelStopModule, 0x089b6194
	AddNID sceKernelGetModuleIdByAddress, 0x089b619c
	AddNID sceKernelGetModuleId, 0x089b61a4
	AddNID sceKernelStartModule, 0x089b61ac
	AddNID ModuleMgrForUser_8F2DF740, 0x089b61b4
	AddNID sceKernelLoadModule, 0x089b61bc
	AddNID sceKernelExitGame, 0x089b61c4
	AddNID sceKernelRegisterExitCallback, 0x089b61cc
	AddNID sceIoWrite, 0x089b61d4
	AddNID sceIoRead, 0x089b61dc
	AddNID sceIoRename, 0x089b61e4
	AddNID sceIoClose, 0x089b61ec
	AddNID sceIoReadAsync, 0x089b61f4
	AddNID sceIoWaitAsync, 0x089b61fc
	AddNID sceIoOpen, 0x089b6204
	AddNID sceIoLseek, 0x089b620c
	AddNID sceIoPollAsync, 0x089b6214
	AddNID scePowerRegisterCallback, 0x089b621c
	AddNID sceGeListSync, 0x089b6224
	AddNID sceGeUnsetCallback, 0x089b622c
	AddNID sceGeRestoreContext, 0x089b6234
	AddNID sceGeListEnQueueHead, 0x089b623c
	AddNID sceGeSaveContext, 0x089b6244
	AddNID sceGeContinue, 0x089b624c
	AddNID sceGeSetCallback, 0x089b6254
	AddNID sceGeListEnQueue, 0x089b625c
	AddNID sceGeDrawSync, 0x089b6264
	AddNID sceGeEdramSetAddrTranslation, 0x089b626c
	AddNID sceGeGetCmd, 0x089b6274
	AddNID sceGeListUpdateStallAddr, 0x089b627c
	AddNID sceGeEdramGetAddr, 0x089b6284
	AddNID sceDisplaySetMode, 0x089b628c
	AddNID sceDisplaySetFrameBuf, 0x089b6294
	AddNID sceDisplayWaitVblankStart, 0x089b629c
	AddNID sceDisplayGetVcount, 0x089b62a4
	AddNID sceCtrlSetSamplingMode, 0x089b62ac
	AddNID sceCtrlReadBufferPositive, 0x089b62b4
	AddNID sceCtrlSetIdleCancelThreshold, 0x089b62bc
	AddNID sceUtilityLoadModule, 0x089b62c4
	AddNID sceUtilitySavedataInitStart, 0x089b62cc
	AddNID sceUtilitySavedataGetStatus, 0x089b62d4
	AddNID sceUtilitySavedataShutdownStart, 0x089b62dc
	AddNID sceUtilitySavedataUpdate, 0x089b62e4
	AddNID sceUtilityUnloadModule, 0x089b62ec
	AddNID module_info, 0x089b647c
	AddNID syslib_11B97506, 0x089b6858
	AddNID module_start_thread_parameter, 0x089b8df0
	AddNID module_stop_thread_parameter, 0x089b8e00



	.ident	"VAL-SDK"

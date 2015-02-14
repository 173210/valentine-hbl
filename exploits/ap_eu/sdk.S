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


	AddNID module_start, 0x088943cc
	AddNID module_stop, 0x088947a4
	AddNID sceImposeSetLanguageMode, 0x0891ca64
	AddNID sceRtcGetCurrentTick, 0x0891ca6c
	AddNID sceRtcTickAddSeconds, 0x0891ca74
	AddNID sceUmdCheckMedium, 0x0891ca7c
	AddNID sceUmdGetDriveStat, 0x0891ca84
	AddNID sceUmdWaitDriveStat, 0x0891ca8c
	AddNID sceUmdRegisterUMDCallBack, 0x0891ca94
	AddNID sceUmdActivate, 0x0891ca9c
	AddNID __sceSasSetADSR, 0x0891caa4
	AddNID __sceSasGetAllEnvelopeHeights, 0x0891caac
	AddNID __sceSasRevParam, 0x0891cab4
	AddNID __sceSasGetPauseFlag, 0x0891cabc
	AddNID __sceSasRevType, 0x0891cac4
	AddNID __sceSasInit, 0x0891cacc
	AddNID __sceSasSetVolume, 0x0891cad4
	AddNID __sceSasCoreWithMix, 0x0891cadc
	AddNID __sceSasSetSL, 0x0891cae4
	AddNID __sceSasGetEndFlag, 0x0891caec
	AddNID __sceSasGetEnvelopeHeight, 0x0891caf4
	AddNID __sceSasSetKeyOn, 0x0891cafc
	AddNID __sceSasSetPause, 0x0891cb04
	AddNID __sceSasSetVoice, 0x0891cb0c
	AddNID __sceSasSetADSRmode, 0x0891cb14
	AddNID __sceSasSetKeyOff, 0x0891cb1c
	AddNID __sceSasCore, 0x0891cb24
	AddNID __sceSasSetPitch, 0x0891cb2c
	AddNID __sceSasSetNoise, 0x0891cb34
	AddNID __sceSasGetGrain, 0x0891cb3c
	AddNID __sceSasSetSimpleADSR, 0x0891cb44
	AddNID __sceSasSetGrain, 0x0891cb4c
	AddNID __sceSasRevEVOL, 0x0891cb54
	AddNID __sceSasGetOutputmode, 0x0891cb5c
	AddNID __sceSasSetVoicePCM, 0x0891cb64
	AddNID __sceSasSetOutputmode, 0x0891cb6c
	AddNID __sceSasRevVON, 0x0891cb74
	AddNID sceAtracGetChannel, 0x0891cb7c
	AddNID sceAtracGetStreamDataInfo, 0x0891cb84
	AddNID sceAtracReleaseAtracID, 0x0891cb8c
	AddNID sceAtracDecodeData, 0x0891cb94
	AddNID sceAtracSetDataAndGetID, 0x0891cb9c
	AddNID sceAtracAddStreamData, 0x0891cba4
	AddNID sceAtracSetLoopNum, 0x0891cbac
	AddNID sceAtracGetMaxSample, 0x0891cbb4
	AddNID sceAudioOutputBlocking, 0x0891cbbc
	AddNID sceAudioOutputPannedBlocking, 0x0891cbc4
	AddNID sceAudioChReserve, 0x0891cbcc
	AddNID sceAudioChRelease, 0x0891cbd4
	AddNID sceAudioChangeChannelConfig, 0x0891cbdc
	AddNID sceAudioGetChannelRestLength, 0x0891cbe4
	AddNID sceAudioChangeChannelVolume, 0x0891cbec
	AddNID sceAudioSetChannelDataLen, 0x0891cbf4
	AddNID sceAudioOutputPanned, 0x0891cbfc
	AddNID sceKernelLibcGettimeofday, 0x0891cc04
	AddNID sceKernelDcacheWritebackAll, 0x0891cc0c
	AddNID sceKernelLibcClock, 0x0891cc14
	AddNID sceKernelLibcTime, 0x0891cc1c
	AddNID sceKernelDelayThread, 0x0891cc24
	AddNID sceKernelCreateSema, 0x0891cc2c
	AddNID sceKernelCreateCallback, 0x0891cc34
	AddNID sceKernelChangeCurrentThreadAttr, 0x0891cc3c
	AddNID sceKernelDeleteCallback, 0x0891cc44
	AddNID sceKernelStartThread, 0x0891cc4c
	AddNID sceKernelDeleteSema, 0x0891cc54
	AddNID sceKernelCheckCallback, 0x0891cc5c
	AddNID sceKernelGetSystemTimeLow, 0x0891cc64
	AddNID sceKernelSignalSema, 0x0891cc6c
	AddNID sceKernelCreateThread, 0x0891cc74
	AddNID sceKernelWaitSema, 0x0891cc7c
	AddNID sceKernelTerminateThread, 0x0891cc84
	AddNID sceKernelDelayThreadCB, 0x0891cc8c
	AddNID sceKernelDeleteThread, 0x0891cc94
	AddNID sceKernelExitThread, 0x0891cc9c
	AddNID sceKernelSetEventFlag, 0x0891cca4
	AddNID sceKernelDeleteEventFlag, 0x0891ccac
	AddNID sceKernelWaitThreadEnd, 0x0891ccb4
	AddNID sceKernelWaitEventFlag, 0x0891ccbc
	AddNID sceKernelCreateEventFlag, 0x0891ccc4
	AddNID sceKernelPrintf, 0x0891cccc
	AddNID sceKernelSetCompilerVersion, 0x0891ccd4
	AddNID sceKernelAllocPartitionMemory, 0x0891ccdc
	AddNID SysMemUserForUser_35669D4C, 0x0891cce4
	AddNID sceKernelGetBlockHeadAddr, 0x0891ccec
	AddNID sceKernelFreePartitionMemory, 0x0891ccf4
	AddNID sceKernelStdin, 0x0891ccfc
	AddNID sceKernelStdout, 0x0891cd04
	AddNID sceKernelStderr, 0x0891cd0c
	AddNID sceKernelUnloadModule, 0x0891cd14
	AddNID sceKernelStopModule, 0x0891cd1c
	AddNID sceKernelGetModuleIdByAddress, 0x0891cd24
	AddNID sceKernelGetModuleId, 0x0891cd2c
	AddNID sceKernelStartModule, 0x0891cd34
	AddNID ModuleMgrForUser_8F2DF740, 0x0891cd3c
	AddNID sceKernelLoadModule, 0x0891cd44
	AddNID sceKernelExitGame, 0x0891cd4c
	AddNID sceKernelRegisterExitCallback, 0x0891cd54
	AddNID sceKernelCpuSuspendIntr, 0x0891cd5c
	AddNID Kernel_Library_293B45B8, 0x0891cd64
	AddNID sceKernelCpuResumeIntr, 0x0891cd6c
	AddNID Kernel_Library_A089ECA4, 0x0891cd74
	AddNID Kernel_Library_1839852A, 0x0891cd7c
	AddNID sceIoWrite, 0x0891cd84
	AddNID sceIoRead, 0x0891cd8c
	AddNID sceIoRename, 0x0891cd94
	AddNID sceIoClose, 0x0891cd9c
	AddNID sceIoReadAsync, 0x0891cda4
	AddNID sceIoWaitAsync, 0x0891cdac
	AddNID sceIoOpen, 0x0891cdb4
	AddNID sceIoLseek, 0x0891cdbc
	AddNID sceIoPollAsync, 0x0891cdc4
	AddNID scePowerRegisterCallback, 0x0891cdcc
	AddNID sceGeListSync, 0x0891cdd4
	AddNID sceGeUnsetCallback, 0x0891cddc
	AddNID sceGeRestoreContext, 0x0891cde4
	AddNID sceGeListEnQueueHead, 0x0891cdec
	AddNID sceGeSaveContext, 0x0891cdf4
	AddNID sceGeContinue, 0x0891cdfc
	AddNID sceGeSetCallback, 0x0891ce04
	AddNID sceGeListEnQueue, 0x0891ce0c
	AddNID sceGeDrawSync, 0x0891ce14
	AddNID sceGeEdramSetAddrTranslation, 0x0891ce1c
	AddNID sceGeGetCmd, 0x0891ce24
	AddNID sceGeListUpdateStallAddr, 0x0891ce2c
	AddNID sceGeEdramGetAddr, 0x0891ce34
	AddNID sceDisplaySetMode, 0x0891ce3c
	AddNID sceDisplaySetFrameBuf, 0x0891ce44
	AddNID sceDisplayWaitVblankStart, 0x0891ce4c
	AddNID sceDisplayGetVcount, 0x0891ce54
	AddNID sceCtrlSetSamplingMode, 0x0891ce5c
	AddNID sceCtrlReadBufferPositive, 0x0891ce64
	AddNID sceCtrlSetSamplingCycle, 0x0891ce6c
	AddNID sceCtrlSetIdleCancelThreshold, 0x0891ce74
	AddNID sceUtilityLoadModule, 0x0891ce7c
	AddNID sceUtilitySavedataInitStart, 0x0891ce84
	AddNID sceUtilitySavedataGetStatus, 0x0891ce8c
	AddNID sceUtilitySavedataShutdownStart, 0x0891ce94
	AddNID sceUtilityGetSystemParamInt, 0x0891ce9c
	AddNID sceUtilitySavedataUpdate, 0x0891cea4
	AddNID sceUtilityUnloadModule, 0x0891ceac
	AddNID module_info, 0x0891d050
	AddNID syslib_11B97506, 0x0891d440
	AddNID module_start_thread_parameter, 0x08925640
	AddNID module_stop_thread_parameter, 0x08925650



	.ident	"VAL-SDK"

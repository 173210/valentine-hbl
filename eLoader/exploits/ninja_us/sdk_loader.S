.macro AddNID funcname, nid

	.globl  \funcname
	.ent    \funcname
	.type   \funcname, @function
\funcname = \nid
	.end    \funcname
	.size	\funcname, 8

.endm

	.text
	.align	2


	AddNID sceIoLseek, 0x088F51A98
	AddNID sceAudioOutput2Reserve, 0x088F5CB0
	AddNID sceAudioOutput2OutputBlocking, 0x088F5CB0
	AddNID sceAudioOutput2Release, 0x088F5CB0
	AddNID sceUmdCheckMedium, 0x088F5EF8
	AddNID sceUmdWaitDriveStat, 0x088F5F00
	AddNID sceUmdActive, 0x088F5F08
	AddNID sceCtrlReadLatch, 0x088F5CC8
	AddNID sceCtrlSetSamplingMode, 0x088F5CD0
	AddNID sceCtrlPeekBufferPositive, 0x088F5CD8
	AddNID sceCtrlSetSamplingCycle, 0x088F5CE0
	AddNID sceDisplaySetMode, 0x088F5CE8
	AddNID sceDisplaySetFrameBuf, 0x088F5CF0
	AddNID sceDisplayWaitVblankStart, 0x088F5CF8
	AddNID sceGeListSync, 0x088F5D00
	AddNID sceGeUnsetCallback, 0x088F5D08
	AddNID sceGeListEnQueueHead, 0x088F5D10
	AddNID sceGeEdramGetSize, 0x088F5D18
	AddNID sceGeContinue, 0x088F5D20
	AddNID sceGeSetCallback, 0x088F5D28
	AddNID sceGeListEnQueue, 0x088F5D30
	AddNID sceGeDrawSync, 0x088F5D38
	AddNID sceGeBreak, 0x088F5D40
	AddNID sceGeListUpdateStallAddr, 0x088F5D48
	AddNID sceGeEdramGetAddr, 0x088F5D50
	AddNID sceIoWrite, 0x088F5A40
	AddNID sceIoDevctl, 0x088F5A48
	AddNID sceIoRead, 0x088F5A50
	AddNID sceIoRename, 0x088F5A58
	AddNID sceIoClose, 0x088F5A60
	AddNID sceIoGetstat, 0x088F5A68
	AddNID sceIoDopen, 0x088F5A70
	AddNID sceIoDread, 0x088F5A78
	AddNID sceIoDclose, 0x088F5A80
	AddNID sceIoRemove, 0x088F5A88
	AddNID sceIoOpen, 0x088F5A90
	AddNID sceIoLseek, 0x088F5A98
	AddNID sceKernelCpuSuspendIntr, 0x088F5AA0
	AddNID sceKernelCpuResumeIntr, 0x088F5AA8
	AddNID sceKernelExitGame, 0x088F5AB0
	AddNID sceKernelRegisterExitCallback, 0x088F5AB8
	AddNID sceKernelUnloadModule, 0x088F5AC0
	AddNID sceKernelGetModuleIdByAddress, 0x088F5AC8
	AddNID sceKernelGetModuleID, 0x088F5AD0
	AddNID sceKernelStopModule, 0x088F5AE0
	AddNID sceKernelModule_8F2DF740, 0x088F5AD8
	AddNID sceKernelLibcGettimeofday, 0x088F5C38
	AddNID sceKernelDcacheWritebackAll, 0x088F5C40
	AddNID sceKernelLibcClock, 0x088F5C48
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x088F5C50
	AddNID sceKernelLibcTime, 0x088FC58
	AddNID sceKernelPowerUnUnlock, 0x088F5EF0
	AddNID sceKernelStdin, 0x088F5AE8
	AddNID sceKernelStdout, 0x088F5AF0
	AddNID sceKernelStderr, 0x088F5AF8
	AddNID sceKernelPrintf, 0x088F5B00
	AddNID sceKernelAllocPartitionMemory, 0x088F5B08
	AddNID sceKernelSetCompiledSdkVersion, 0x088F5B10
	AddNID sceKernelGetBlockHeadAddr, 0x088F5B18
	AddNID sceKernelFreePartitionMemory, 0x088F5B20
	AddNID sceKernelSetCompilerVersion, 0x088F5B28
	AddNID sceKernelTotalFreeMemSize, 0x088F5B30
	
	
	AddNID sceUtilityLoadModule, 0x088F5F10
	AddNID sceUtilityMsgDialogInitStart, 0x088F5F18
	AddNID sceUtilityGetSystemParamString, 0x088F5F20
	AddNID sceUtilityNetconfInitStart, 0x088F5F28
	AddNID sceUtilitySavedataInitStart, 0x088F5F30
	AddNID sceUtilityNetconfGetStatus, 0x088F5F38
	AddNID sceUtilityMsgDialogShutdownStart, 0x088F5F40
	AddNID sceUtilitySavedataGetStatus,0x088F5F48 
	AddNID sceUtilityNetconfUpdate, 0x088F5F50
	AddNID sceUtilityMsgDialogUpdate, 0x088F5F58
	AddNID sceUtilitySavedataShutdownStart, 0x088F5F60
	AddNID sceUtilityMsgDialogGetStatus, 0x088F5F68
	AddNID sceUtilitySavedataUpdate, 0x088F5F70
	AddNID sceUtilityUnloadModule, 0x088F5F78
	AddNID sceUtilityNetconfShutdownStart, 0x088F5F80
	

	AddNID sceImposeGetLanguageMode, 0x088F5D58
	AddNID sceImposeSetLanguageMode, 0x088F5D60
	
	AddNID sceNetGetLocalEtherAddr, 0x088F5D68
	AddNID sceNetTerm, 0x088F5D70
	AddNID sceNetInit, 0x088F5D78
	AddNID sceNetAdhocPdpCreate, 0x088F5D80
	AddNID sceNetAdhocPdpDelete, 0x088F5D88
	AddNID sceNetAdhocTerm, 0x088F5D90
	AddNID sceNetAdhocPdpSend, 0x088F5D98
	AddNID sceNetAdhocPdpRecv, 0x088F5DA0
	AddNID sceNetAdhocInit, 0x088F5DA8
	AddNID sceNetAdhocctlAddHandler, 0x088F5E00
	AddNID sceNetAdhocctlDelHandler, 0x088F5E08
	AddNID sceNetAdhocctlGetNameByAddr, 0x088F5E10
	AddNID sceNetAdhocctlTerm, 0x088F5E18
	AddNID sceNetAdhocctlInit, 0x088F5E20
	AddNID sceNetAdhocMatchingInit, 0x088F5DB0
	AddNID sceNetAdhocMatchingStop, 0x088F5DB8
	AddNID sceNetAdhocMatchingSelectTarget, 0x088F5DC0
	AddNID sceNetAdhocMatchingTerm, 0x088F5DC8
	AddNID sceNetAdhocMatchingCancelTargetWithOpt, 0x088F5DD0
	AddNID sceNetAdhocMatchingStart, 0x088F5DD8
	AddNID sceNetAdhocMatchingSetHelloOpt, 0x088F5DE0
	AddNID sceNetAdhocMatchingCreate, 0x088F5DE8
	AddNID sceNetAdhocMatchingCancelTarget, 0x088F5DF0
	AddNID sceNetAdhocMatchingDelete, 0x088F5DF8
	
	
	AddNID sceAtracGetNextSample, 0x088F5C60
	AddNID sceAtracReleaseAtracID, 0x088F5C68
	AddNID sceAtracDecodeData, 0x088F5C70
	AddNID sceAtracSetDataAndGetID, 0x088F5C78
	AddNID sceAtracGetSecondBufferInfo, 0x088F5C80
	AddNID sceAtracSetLoopNum, 0x088F5C88
	AddNID sceAtracGetSoundSample, 0x088F5C90
	AddNID sceAtracGetOutputChannel, 0x088F5C98
	AddNID sceAtracGetMaxSample, 0x088F5CA0
	AddNID sceAtracGetInternalErrorInfo, 0x088F5CA8

	AddNID __sceSasSetADSR, 0x088F5E28
	AddNID __sceSasRevParam, 0x088F5E30
	AddNID __sceSasGetPauseFlag, 0x088F5E38
	AddNID __sceSasRevType, 0x088F5E40
	AddNID __sceSasInit, 0x088F5E48
	AddNID __sceSasSetVolume, 0x088F5E50
	AddNID __sceSasCoreWithMix, 0x088F5E58
	AddNID __sceSasSetSL, 0x088F5E60
	AddNID __sceSasGetEndFlag, 0x088F5E68
	AddNID __sceSasGetEnvelopeHeight, 0x088F5E70
	AddNID __sceSasSetKeyOn, 0x088F5E78
	AddNID __sceSasSetPause, 0x088F5E80
	AddNID __sceSasSetVoice, 0x088F5E88
	AddNID __sceSasSetADSRmode, 0x088F5E90
	AddNID __sceSasSetKeyOff, 0x088F5E98
	AddNID __sceSasCore, 0x088F5EA0
	AddNID __sceSasSetPitch, 0x088F5EA8
	AddNID __sceSasSetNoise, 0x088F5EB0
	AddNID __sceSasGetGrain, 0x088F5EB8
	AddNID __sceSasSetSimpleADSR, 0x088F5EC0
	AddNID __sceSasSetGrain, 0x088F5EC8
	AddNID __sceSasRevEVOL, 0x088F5ED0
	AddNID __sceSasGetOutputmode, 0x088F5ED8
	AddNID __sceSasSetOutputmode, 0x088F5EE0
	AddNID __sceSasRevVON, 0x088F5EE8
		
	AddNID sceKernelDelayThread, 0x088F5B38
	AddNID sceKernelSetEventFlag, 0x088F5B40
	AddNID sceKernelCreateSema, 0x088F5B48
	AddNID sceKernelCreateCallback, 0x088F5B50
	AddNID sceKernelSendMbx, 0x088F5B58
	AddNID sceKernelDeleteEventFlag, 0x088F5B60
	AddNID sceKernelStartThread, 0x088F5B68
	AddNID sceKernelDeleteMutex, 0x088F5B70
	AddNID sceKernelPollEventFlag, 0x088F5B78
	AddNID sceKernelCheckCallback, 0x088F5B80
	AddNID sceKernelReferVplStatus, 0x088F5B88
	AddNID sceKernelPollMbx, 0x088F5B90
	AddNID sceKernelSignalSema, 0x088F5B98
	AddNID sceKernelWaitEventFlag, 0x088F5BA0
	AddNID sceKernelCreateThread, 0x088F5BA8
	AddNID sceKernelCreateEventFlag, 0x088F5BB0
	AddNID sceKernelCreateVpl, 0x088F5BB8
	AddNID sceKernelTerminateThread, 0x088F5BC0
	AddNID sceKernelUnlockMutex, 0x088F5BC8
	AddNID sceKernelWaitSemaCB, 0x088F5BD0
	AddNID sceKernelClearEventFlag, 0x088F5BD8
	AddNID sceKernelCreateMbx, 0x088F5BE0
	AddNID sceKernelSleepThreadCB, 0x088F5BE8
	AddNID sceKernelGetSystemTimeWide, 0x088F5BF0
	AddNID sceKernelWaitThreadEndCB, 0x088F5BF8
	AddNID sceKernelDeleteMbx, 0x088F5C00
	AddNID sceKernelDeleteThread, 0x088F5C08
	AddNID sceKernelExitThread, 0x088F5C10
	AddNID sceKernelLockMutex, 0x088F5C18
	AddNID sceKernelFreeVpl, 0x088F5C20
	AddNID sceKernelCreateMutex, 0x088F5C28
	AddNID sceKernelAllocateVpl, 0x088F5C30
	
	AddNID sceKernelGetThreadId, 0x08800240


	.ident	"VAL-SDK"

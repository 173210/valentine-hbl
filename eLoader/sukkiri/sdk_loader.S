.macro AddNID funcname, nid 

	.globl \funcname
	.ent \funcname
\funcname = \nid
	.end \funcname

.endm

	.file 1 "sdk.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align 2

	AddNID sceKernelUnloadModule , 0x088bdd60
	AddNID sceKernelStartModule , 0x088bdd68
	AddNID ModuleMgrForUser_8F2DF740 , 0x088bdd70
	AddNID sceKernelStopModule , 0x088bdd78
	AddNID sceKernelGetModuleIdByAddress , 0x088bdd80
	AddNID sceKernelGetModuleId , 0x088bdd88
	AddNID sceKernelLoadModuleBufferUsbWlan , 0x088bdd90

	AddNID sceKernelRegisterSubIntrHandler , 0x088bdd98
	AddNID sceKernelReleaseSubIntrHandler , 0x088bdda0
	AddNID sceKernelEnableSubIntr , 0x088bdda8

	AddNID sceIoWrite , 0x088bddb0
	AddNID sceIoChdir , 0x088bddb8
	AddNID sceIoRead , 0x088bddc0
	AddNID sceIoRename , 0x088bddc8
	AddNID sceIoClose , 0x088bddd0
	AddNID sceIoReadAsync , 0x088bddd8
	AddNID sceIoGetstat , 0x088bdde0
	AddNID sceIoOpen , 0x088bdde8
	AddNID sceIoLseek , 0x088bddf0
	AddNID sceIoPollAsync , 0x088bddf8

	AddNID sceKernelCpuSuspendIntr , 0x088bde00
	AddNID sceKernelUnlockLwMutex , 0x088bde08
	AddNID sceKernelCpuResumeIntr , 0x088bde10
	AddNID sceKernelLockLwMutex , 0x088bde18

	AddNID sceKernelExitGame , 0x088bde20
	AddNID sceKernelRegisterExitCallback , 0x088bde28

	AddNID sceKernelStdin , 0x088bde30
	AddNID sceKernelStdout , 0x088bde38
	AddNID sceKernelStderr , 0x088bde40

	AddNID sceKernelPrintf , 0x088bde48
	AddNID SysMemUserForUser_FE707FDF , 0x088bde50
	AddNID sceKernelAllocPartitionMemory , 0x088bde58
	AddNID SysMemUserForUser_91DE343C , 0x088bde60
	AddNID sceKernelGetBlockHeadAddr , 0x088bde68
	AddNID sceKernelFreePartitionMemory , 0x088bde70
	AddNID SysMemUserForUser_DB83A952 , 0x088bde78
	AddNID sceKernelSetCompilerVersion , 0x088bde80

	AddNID sceKernelCreateFpl , 0x088bde88
	AddNID sceKernelDelayThread , 0x088bde90
	AddNID sceKernelCreateSema , 0x088bde98
	AddNID sceKernelSetEventFlag , 0x088bdea0
	AddNID sceKernelCreateCallback , 0x088bdea8
	AddNID sceKernelDeleteFpl , 0x088bdeb0
	AddNID sceKernelDeleteEventFlag , 0x088bdeb8
	AddNID sceKernelStartThread , 0x088bdec0
	AddNID sceKernelWaitThreadEnd , 0x088bdec8
	AddNID sceKernelDeleteSema , 0x088bded0
	AddNID sceKernelGetThreadId , 0x088bded8
	AddNID sceKernelCheckCallback , 0x088bdee0
	AddNID sceKernelGetSystemTimeLow , 0x088bdee8
	AddNID sceKernelTerminateDeleteThread , 0x088bdef0
	AddNID sceKernelSignalSema , 0x088bdef8
	AddNID sceKernelWaitEventFlag , 0x088bdf00
	AddNID sceKernelCreateThread , 0x088bdf08
	AddNID sceKernelWaitSema , 0x088bdf10
	AddNID sceKernelGetThreadStackFreeSize , 0x088bdf18
	AddNID sceKernelCreateEventFlag , 0x088bdf20
	AddNID sceKernelDeleteLwMutex , 0x088bdf28
	AddNID sceKernelTryAllocateFpl , 0x088bdf30
	AddNID sceKernelDelayThreadCB , 0x088bdf38
	AddNID sceKernelResumeThread , 0x088bdf40
	AddNID sceKernelExitDeleteThread , 0x088bdf48
	AddNID sceKernelGetThreadCurrentPriority , 0x088bdf50
	AddNID sceKernelSuspendThread , 0x088bdf58
	AddNID sceKernelSleepThread , 0x088bdf60
	AddNID sceKernelDeleteThread , 0x088bdf68
	AddNID sceKernelExitThread , 0x088bdf70
	AddNID sceKernelCreateLwMutex , 0x088bdf78

	AddNID sceKernelLibcGettimeofday , 0x088bdf80
	AddNID sceKernelDcacheWritebackAll , 0x088bdf88
	AddNID sceKernelLibcClock , 0x088bdf90
	AddNID sceKernelDcacheWritebackInvalidateAll , 0x088bdf98
	AddNID sceKernelLibcTime , 0x088bdfa0
	AddNID sceKernelGetGPI , 0x088bdfa8

	AddNID sceKernelPowerTick , 0x088bdfb0
	AddNID sceKernelPowerUnlock , 0x088bdfb8
	AddNID sceKernelPowerLock , 0x088bdfc0

	AddNID sceGeListSync , 0x088bdfc8
	AddNID sceGeUnsetCallback , 0x088bdfd0
	AddNID sceGeEdramGetSize , 0x088bdfd8
	AddNID sceGeSetCallback , 0x088bdfe0
	AddNID sceGeListEnQueue , 0x088bdfe8
	AddNID sceGeDrawSync , 0x088bdff0
	AddNID sceGeListUpdateStallAddr , 0x088bdff8
	AddNID sceGeEdramGetAddr , 0x088be000

	AddNID sceDisplaySetMode , 0x088be008
	AddNID sceDisplayGetAccumulatedHcount , 0x088be010
	AddNID sceDisplaySetFrameBuf , 0x088be018
	AddNID sceDisplayWaitVblank , 0x088be020
	AddNID sceDisplayWaitVblankStart , 0x088be028
	AddNID sceDisplayGetFrameBuf , 0x088be030

	AddNID sceCtrlSetSamplingMode , 0x088be038
	AddNID sceCtrlReadBufferPositive , 0x088be040
	AddNID sceCtrlPeekBufferPositive , 0x088be048
	AddNID sceCtrlSetSamplingCycle , 0x088be050

	AddNID sceLibFont_0DA7535E , 0x088be058
	AddNID sceLibFont_3AEA8CB6 , 0x088be060
	AddNID sceLibFont_574B6FBC , 0x088be068
	AddNID sceLibFont_67F17ED7 , 0x088be070
	AddNID sceLibFont_980F4895 , 0x088be078
	AddNID sceLibFont_A834319D , 0x088be080
	AddNID sceLibFont_DCC80C2F , 0x088be088

	AddNID sceAudioOutput2Reserve , 0x088be090
	AddNID sceAudioOutputPannedBlocking , 0x088be098
	AddNID sceAudioOutput2OutputBlocking , 0x088be0a0
	AddNID sceAudioOutput2Release , 0x088be0a8
	AddNID sceAudioChReserve , 0x088be0b0
	AddNID sceAudioChRelease , 0x088be0b8
	AddNID sceAudioChangeChannelConfig , 0x088be0c0
	AddNID sceAudioChangeChannelVolume , 0x088be0c8
	AddNID sceAudioSetChannelDataLen , 0x088be0d0

	AddNID __sceSasRevParam , 0x088be0d8
	AddNID __sceSasRevType , 0x088be0e0
	AddNID __sceSasInit , 0x088be0e8
	AddNID __sceSasSetVolume , 0x088be0f0
	AddNID __sceSasCoreWithMix , 0x088be0f8
	AddNID __sceSasGetEndFlag , 0x088be100
	AddNID __sceSasGetEnvelopeHeight , 0x088be108
	AddNID __sceSasSetKeyOn , 0x088be110
	AddNID __sceSasSetPause , 0x088be118
	AddNID __sceSasSetVoice , 0x088be120
	AddNID __sceSasSetKeyOff , 0x088be128
	AddNID __sceSasCore , 0x088be130
	AddNID __sceSasSetPitch , 0x088be138
	AddNID __sceSasSetNoise , 0x088be140
	AddNID __sceSasSetSimpleADSR , 0x088be148
	AddNID __sceSasRevEVOL , 0x088be150
	AddNID __sceSasGetOutputmode , 0x088be158
	AddNID __sceSasRevVON , 0x088be160

	AddNID sceAtracSetData , 0x088be168
	AddNID sceAtracReinit , 0x088be170
	AddNID sceAtracGetNextSample , 0x088be178
	AddNID sceAtracGetStreamDataInfo , 0x088be180
	AddNID sceAtracReleaseAtracID , 0x088be188
	AddNID sceAtracDecodeData , 0x088be190
	AddNID sceAtracGetAtracID , 0x088be198
	AddNID sceAtracAddStreamData , 0x088be1a0
	AddNID sceAtracSetLoopNum , 0x088be1a8
	AddNID sceAtracGetRemainFrame , 0x088be1b0

	AddNID sceUtilityLoadModule , 0x088be1b8
	AddNID sceUtilityMsgDialogInitStart , 0x088be1c0
	AddNID sceUtilityOskShutdownStart , 0x088be1c8
	AddNID sceUtilityOskUpdate , 0x088be1d0
	AddNID sceUtilityNetconfInitStart , 0x088be1d8
	AddNID sceUtilitySavedataInitStart , 0x088be1e0
	AddNID sceUtilityNetconfGetStatus , 0x088be1e8
	AddNID sceUtilityMsgDialogShutdownStart , 0x088be1f0
	AddNID sceUtilityGameSharingUpdate , 0x088be1f8
	AddNID sceUtilitySavedataGetStatus , 0x088be200
	AddNID sceUtilityNetconfUpdate , 0x088be208
	AddNID sceUtilityGameSharingGetStatus , 0x088be210
	AddNID sceUtilityMsgDialogUpdate , 0x088be218
	AddNID sceUtilitySavedataShutdownStart , 0x088be220
	AddNID sceUtilityMsgDialogGetStatus , 0x088be228
	AddNID sceUtilityGetSystemParamInt , 0x088be230
	AddNID sceUtilityGameSharingInitStart , 0x088be238
	AddNID sceUtilitySavedataUpdate , 0x088be240
	AddNID sceUtilityUnloadModule , 0x088be248
	AddNID sceUtilityGameSharingShutdownStart , 0x088be250
	AddNID sceUtilityOskGetStatus , 0x088be258
	AddNID sceUtilityOskInitStart , 0x088be260
	AddNID sceUtilityNetconfShutdownStart , 0x088be268

	AddNID sceUmdGetDriveStat , 0x088be270

	AddNID sceRtcGetCurrentTick , 0x088be278
	AddNID sceRtcGetCurrentClockLocalTime , 0x088be280

	AddNID scePower_EBD177D6 , 0x088be288

	AddNID sceNetGetLocalEtherAddr , 0x088be290
	AddNID sceNetTerm , 0x088be298
	AddNID sceNetInit , 0x088be2a0

	AddNID sceNetAdhocMatchingInit , 0x088be2a8
	AddNID sceNetAdhocMatchingStop , 0x088be2b0
	AddNID sceNetAdhocMatchingTerm , 0x088be2b8
	AddNID sceNetAdhocMatchingGetMembers , 0x088be2c0
	AddNID sceNetAdhocMatchingDelete , 0x088be2c8

	AddNID sceNetAdhocPtpClose , 0x088be2d0
	AddNID sceNetAdhocPtpSend , 0x088be2d8
	AddNID sceNetAdhocPdpCreate , 0x088be2e0
	AddNID sceNetAdhocPdpDelete , 0x088be2e8
	AddNID sceNetAdhocPtpOpen , 0x088be2f0
	AddNID sceNetAdhocPtpRecv , 0x088be2f8
	AddNID sceNetAdhocPtpFlush , 0x088be300
	AddNID sceNetAdhocPtpAccept , 0x088be308
	AddNID sceNetAdhocTerm , 0x088be310
	AddNID sceNetAdhocPdpSend , 0x088be318
	AddNID sceNetAdhocPdpRecv , 0x088be320
	AddNID sceNetAdhocPtpListen , 0x088be328
	AddNID sceNetAdhocInit , 0x088be330
	AddNID sceNetAdhocPtpConnect , 0x088be338

	AddNID sceNetAdhocctlConnect , 0x088be340
	AddNID sceNetAdhocctlJoinEnterGameMode , 0x088be348
	AddNID sceNetAdhocctlAddHandler , 0x088be350
	AddNID sceNetAdhocctlDisconnect , 0x088be358
	AddNID sceNetAdhocctlGetGameModeInfo , 0x088be360
	AddNID sceNetAdhocctlDelHandler , 0x088be368
	AddNID sceNetAdhocctlTerm , 0x088be370
	AddNID sceNetAdhocctlCreateEnterGameMode , 0x088be378
	AddNID sceNetAdhocctlInit , 0x088be380

	AddNID sceImposeSetLanguageMode , 0x088be388
	
	

	.ident "SUK-SDK"
	
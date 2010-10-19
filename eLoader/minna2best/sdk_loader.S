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

# IMPORTANT: the addresses not inside Patapon module are only for 5.00 FW	
# Total libraries: 36 (0x24)
# Total functions: 418 (0x1A2)




	AddNID sceNetInetConnect, 0x08ACD230
	AddNID sceNetAdhocctlGetState, 0x08ACD1B8
	AddNID sceIoLseek, 0x08ACCBC0
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x08ACCDC0
	AddNID sceNetAdhocctlInit, 0x08ACD1D0
	AddNID sceHttpSetResolveTimeOut, 0x08ACCF50
	AddNID sceAudioChReserve, 0x08ACCE58
	AddNID sceUtilityUnloadModule, 0x08ACD460
	AddNID sceUtilityNetconfInitStart, 0x08ACD410
	AddNID sceKernelIcacheInvalidateAll, 0x08ACCDB8
	AddNID sceHttpInit, 0x08ACCF90
	AddNID sceAtracGetAtracID, 0x08ACCE10
	AddNID sceOpenPSIDGetOpenPSID, 0x08ACD2D8
	AddNID sceKernelGetThreadmanIdList, 0x08ACCD60
	AddNID sceKernelGetModuleIdByAddress, 0x08ACCBF8
	AddNID sceKernelTryLockMutex, 0x08ACCD18
	AddNID sceDisplaySetMode, 0x08ACCEA8
	AddNID sceKernelGetSystemTimeLow, 0x08ACCCD8
	AddNID sceUtility_0251B134, 0x08ACD3E8
	AddNID sceNetAdhocGetPdpStat, 0x08ACD160
	AddNID sceKernelGetThreadStackFreeSize, 0x08ACCD08
	AddNID sceGeListSync, 0x08ACCEE0
	AddNID sceUtility_AB083EA9, 0x08ACD448
	AddNID sceLibFont_67F17ED7, 0x08ACCFE8
	AddNID sceMpegQueryStreamSize, 0x08ACD048
	AddNID sceKernelSetEventFlag, 0x08ACCC80
	AddNID sceMpegQueryStreamOffset, 0x08ACD020
	AddNID sceMpegRingbufferDestruct, 0x08ACD010
	AddNID sceNetAdhocctlJoin, 0x08ACD1A8
	AddNID sceNetInetInetAton, 0x08ACD220
	AddNID sceHttpCreateConnectionWithURL, 0x08ACCFA8
	AddNID __sceSasCore, 0x08ACD368
	AddNID sceAtracSetLoopNum, 0x08ACCE20
	AddNID __sceSasGetEndFlag, 0x08ACD330
	AddNID sceNetAdhocPtpFlush, 0x08ACD140
	AddNID sceKernelDeleteThread, 0x08ACCD78
	AddNID sceMpegDelete, 0x08ACD040
	AddNID sceKernelLoadExec, 0x08ACCBE8
	AddNID sceKernelDeleteMutex, 0x08ACCCB8
	AddNID sceUmdActivate, 0x08ACD3D8
	AddNID sceNetTerm, 0x08ACD2C0
	AddNID __sceSasSetOutputmode, 0x08ACD3A8
	AddNID sceUtilitySavedataInitStart, 0x08ACD418
	AddNID sceKernelCreateMutex, 0x08ACCD90
	AddNID sceNetInetClose, 0x08ACD258
	AddNID module_start_thread_parameter, 0x08AEE338
	AddNID sceIoRename, 0x08ACCBA8
	AddNID sceDisplayGetAccumulatedHcount, 0x08ACCEB0
	AddNID sceKernelGetSystemTimeWide, 0x08ACCD50
	AddNID sceNetAdhocInit, 0x08ACD178
	AddNID sceNetAdhocctlTerm, 0x08ACD1C8
	AddNID __sceSasGetEnvelopeHeight, 0x08ACD338
	AddNID sceNetApctlDelHandler, 0x08ACD1E0
	AddNID sceMpegAtracDecode, 0x08ACD068
	AddNID sceKernelStdout, 0x08ACCC30
	AddNID sceNetInetGetsockname, 0x08ACD208
	AddNID sceNetAdhocPtpListen, 0x08ACD170
	AddNID sceKernelCreateSema, 0x08ACCC88
	AddNID sceUtility_F9E0008C, 0x08ACD480
	AddNID sceNetAdhocMatchingSelectTarget, 0x08ACD0D8
	AddNID sceNetAdhocMatchingCancelTarget, 0x08ACD100
	AddNID sceDisplayWaitVblank, 0x08ACCEC0
	AddNID sceNetAdhocPtpRecv, 0x08ACD138
	AddNID sceGeEdramGetAddr, 0x08ACCF30
	AddNID sceUtilityOskShutdownStart, 0x08ACD400
	AddNID sceUmdCheckMedium, 0x08ACD3C0
	AddNID sceKernelExitThread, 0x08ACCD80
	AddNID sceKernelGetModuleId, 0x08ACCC00
	AddNID sceKernelGetGPI, 0x08ACCDD0
	AddNID sceGeEdramGetSize, 0x08ACCEF8
	AddNID sceAudioOutput2Reserve, 0x08ACCE30
	AddNID sceIoRead, 0x08ACCBA0
	AddNID sceMpegFinish, 0x08ACD070
	AddNID sceUtilityHtmlViewerUpdate, 0x08ACD3F0
	AddNID sceKernelReferThreadStatus, 0x08ACCD58
	AddNID sceKernelStopModule, 0x08ACCC20
	AddNID sceKernelAllocPartitionMemory, 0x08ACCC50
	AddNID sceMpegRegistStream, 0x08ACD030
	AddNID sceNetResolverInit, 0x08ACD2B0
	AddNID sceHttpSetSendTimeOut, 0x08ACCF70
	AddNID sceHttpSetResolveRetry, 0x08ACCF40
	AddNID sceMpegQueryMemSize, 0x08ACD090
	AddNID sceUtilityOskGetStatus, 0x08ACD468
	AddNID sceAudioChRelease, 0x08ACCE60
	AddNID sceUtility_D81957B7, 0x08ACD458
	AddNID sceKernelLibcGettimeofday, 0x08ACCDA0
	AddNID sceNetInetInetNtop, 0x08ACD280
	AddNID sceHttpsGetSslError, 0x08ACCF88
	AddNID sceMpegRingbufferAvailableSize, 0x08ACD088
	AddNID sceKernelClearEventFlag, 0x08ACCD48
	AddNID sceKernelSleepThread, 0x08ACCD70
	AddNID sceKernelSignalSema, 0x08ACCCE8
	AddNID sceDisplaySetFrameBuf, 0x08ACCEB8
	AddNID sceGeDrawSync, 0x08ACCF18
	AddNID __sceSasSetADSRmode, 0x08ACD358
	AddNID __sceSasRevParam, 0x08ACD2F8
	AddNID sceIoClose, 0x08ACCBB0
	AddNID sceNetAdhocTerm, 0x08ACD150
	AddNID sceNetAdhocMatchingDelete, 0x08ACD108
	AddNID sceMpegGetAtracAu, 0x08ACD0B0
	AddNID sceKernelUnloadModule, 0x08ACCBF0
	AddNID sceAudioGetChannelRestLength, 0x08ACCE70
	AddNID sceAtracGetNextSample, 0x08ACCDF0
	AddNID sceGeSetCallback, 0x08ACCF08
	AddNID sceKernelDelayThread, 0x08ACCC78
	AddNID sceNetAdhocPtpAccept, 0x08ACD148
	AddNID sceKernelResumeThread, 0x08ACCD40
	AddNID sceNetInetBind, 0x08ACD218
	AddNID sceHttpCreateRequestWithURL, 0x08ACCF98
	AddNID sceMpegMallocAvcEsBuf, 0x08ACD078
	AddNID __sceSasSetNoise, 0x08ACD378
	AddNID sceNetAdhocctlAddHandler, 0x08ACD198
	AddNID sceLibFont_DCC80C2F, 0x08ACD000
	AddNID sceNetInetRecv, 0x08ACD278
	AddNID sceNetApctlInit, 0x08ACD1F8
	AddNID sceCtrlSetSamplingMode, 0x08ACCE90
	AddNID sceUtilityOskUpdate, 0x08ACD408
	AddNID sceLibFont_574B6FBC, 0x08ACCFE0
	AddNID sceNetInetRecvfrom, 0x08ACD270
	AddNID sceMpegRingbufferPut, 0x08ACD080
	AddNID sceNetAdhocPtpSend, 0x08ACD118
	AddNID sceNetEtherNtostr, 0x08ACD2D0
	AddNID sceGeBreak, 0x08ACCF20
	AddNID sceKernelTotalFreeMemSize, 0x08ACCC48
	AddNID sceLibFont_A834319D, 0x08ACCFF8
	AddNID sceNetResolverCreate, 0x08ACD298
	AddNID sceAtracGetStreamDataInfo, 0x08ACCDF8
	AddNID sceKernelDcacheWritebackAll, 0x08ACCDA8
	AddNID sceAudioChangeChannelConfig, 0x08ACCE68
	AddNID __sceSasSetVoice, 0x08ACD350
	AddNID sceNetResolverStartNtoA, 0x08ACD290
	AddNID sceHttpDeleteTemplate, 0x08ACCFC8
	AddNID sceHttpGetContentLength, 0x08ACCF38
	AddNID sceGeContinue, 0x08ACCF00
	AddNID sceGeListEnQueueHead, 0x08ACCEF0
	AddNID sceKernelTerminateDeleteThread, 0x08ACCCE0
	AddNID sceAudioSetChannelDataLen, 0x08ACCE80
	AddNID sceNetAdhocctlGetScanInfo, 0x08ACD1C0
	AddNID sceNetAdhocMatchingStop, 0x08ACD0D0
	AddNID sceNetResolverTerm, 0x08ACD2A0
	AddNID sceNetInetSend, 0x08ACD248
	AddNID sceMpegFlushAllStream, 0x08ACD058
	AddNID sceUtilitySavedataUpdate, 0x08ACD450
	AddNID sceAtracGetRemainFrame, 0x08ACCE28
	AddNID sceNetAdhocctlConnect, 0x08ACD190
	AddNID __sceSasSetVolume, 0x08ACD318
	AddNID sceNetAdhocPtpConnect, 0x08ACD180
	AddNID module_info, 0x08ACD700
	AddNID sceAudioOutput2OutputBlocking, 0x08ACCE48
	AddNID sceAudioOutput2Release, 0x08ACCE50
	AddNID sceKernelDeleteSema, 0x08ACCCC0
	AddNID sceHttpGetNetworkErrno, 0x08ACCFB0
	AddNID sceHttpDeleteRequest, 0x08ACCF80
	AddNID sceUmdWaitDriveStat, 0x08ACD3D0
	AddNID sceNetAdhocctlDisconnect, 0x08ACD1A0
	AddNID sceNetInetSelect, 0x08ACD240
	AddNID sceKernelDelayThreadCB, 0x08ACCD28
	AddNID sceKernelStderr, 0x08ACCC38
	AddNID __sceSasGetOutputmode, 0x08ACD3A0
	AddNID sceUtilityNetconfShutdownStart, 0x08ACD478
	AddNID sceAudioOutputPanned, 0x08ACCE88
	AddNID sceKernelStdin, 0x08ACCC28
	AddNID sceNetApctlTerm, 0x08ACD1F0
	AddNID sceKernelUnlockMutex, 0x08ACCD30
	AddNID module_stop_thread_parameter, 0x08AEE348
	AddNID sceNetAdhocMatchingCreate, 0x08ACD0F8
	AddNID sceNetApctlGetInfo, 0x08ACD1D8
	AddNID sceKernelExitGame, 0x08ACCBD8
	AddNID sceNetAdhocctlDelHandler, 0x08ACD1B0
	AddNID sceNetAdhocPtpClose, 0x08ACD110
	AddNID sceMpegAvcDecodeStop, 0x08ACD060
	AddNID sceNetAdhocPdpRecv, 0x08ACD168
	AddNID sceUtilityNetconfGetStatus, 0x08ACD420
	AddNID sceHttpDeleteConnection, 0x08ACCF60
	AddNID module_stop, 0x08AAF3E8
	AddNID sceUtilityOskInitStart, 0x08ACD470
	AddNID sceNetInetGetsockopt, 0x08ACD238
	AddNID sceMpegInitAu, 0x08ACD018
	AddNID sceKernelGetSystemTime, 0x08ACCC90
	AddNID __sceSasGetPauseFlag, 0x08ACD300
	AddNID sceUtilitySavedataShutdownStart, 0x08ACD438
	AddNID __sceSasSetADSR, 0x08ACD2E8
	AddNID sceKernelEnableSubIntr, 0x08ACCB90
	AddNID __sceSasGetGrain, 0x08ACD380
	AddNID sceKernelReleaseSubIntrHandler, 0x08ACCB88
	AddNID sceNetAdhocPdpCreate, 0x08ACD120
	AddNID sceKernelGetBlockHeadAddr, 0x08ACCC60
	AddNID sceKernelLockMutex, 0x08ACCD88
	AddNID sceKernelCreateCallback, 0x08ACCC98
	AddNID sceKernelDeleteEventFlag, 0x08ACCCA0
	AddNID sceNetGetLocalEtherAddr, 0x08ACD2B8
	AddNID sceKernelFreePartitionMemory, 0x08ACCC68
	AddNID sceHttpEnd, 0x08ACCFB8
	AddNID ModuleMgrForUser_8F2DF740, 0x08ACCC10
	AddNID sceKernelLoadModule, 0x08ACCC18
	AddNID module_start, 0x08AAF044
	AddNID sceKernelPollSema, 0x08ACCD20
	AddNID sceKernelCreateEventFlag, 0x08ACCD10
	AddNID sceKernelSetCompilerVersion, 0x08ACCC70
	AddNID sceMpegUnRegistStream, 0x08ACD038
	AddNID sceUtilityLoadModule, 0x08ACD3F8
	AddNID syslib_11B97506, 0x08AEFC88
	AddNID sceAtracSetData, 0x08ACCDE0
	AddNID sceNetResolverDelete, 0x08ACD2A8
	AddNID sceNetAdhocMatchingInit, 0x08ACD0C8
	AddNID sceDisplayGetFrameBuf, 0x08ACCED8
	AddNID sceKernelPrintf, 0x08ACCC40
	AddNID sceKernelSysClock2USec, 0x08ACCD98
	AddNID sceHttpSetConnectTimeOut, 0x08ACCF68
	AddNID sceKernelStartModule, 0x08ACCC08
	AddNID sceKernelWaitThreadEnd, 0x08ACCCB0
	AddNID sceNetInetInit, 0x08ACD210
	AddNID sceKernelRegisterSubIntrHandler, 0x08ACCB80
	AddNID sceKernelWaitSema, 0x08ACCD00
	AddNID sceKernelLibcTime, 0x08ACCDC8
	AddNID sceKernelCreateThread, 0x08ACCCF8
	AddNID sceKernelCpuSuspendIntr, 0x08ACCBC8
	AddNID sceUmdGetDriveStat, 0x08ACD3C8
	AddNID sceHttpSetRecvTimeOut, 0x08ACCF48
	AddNID __sceSasSetKeyOff, 0x08ACD360
	AddNID sceKernelSuspendThread, 0x08ACCD68
	AddNID __sceSasSetPitch, 0x08ACD370
	AddNID sceMpegQueryAtracEsSize, 0x08ACD0B8
	AddNID __sceSasSetSimpleADSR, 0x08ACD388
	AddNID sceKernelCheckCallback, 0x08ACCCD0
	AddNID sceCtrlSetSamplingCycle, 0x08ACCEA0
	AddNID sceMpegRingbufferConstruct, 0x08ACD028
	AddNID sceCtrlReadBufferPositive, 0x08ACCE98
	AddNID SysMemUserForUser_342061E5, 0x08ACCC58
	AddNID sceMpegRingbufferQueryMemSize, 0x08ACD0A0
	AddNID sceUtilityNetconfUpdate, 0x08ACD430
	AddNID __sceSasCoreWithMix, 0x08ACD320
	AddNID sceLibFont_980F4895, 0x08ACCFF0
	AddNID sceMpegCreate, 0x08ACD0A8
	AddNID sceKernelChangeThreadPriority, 0x08ACCD38
	AddNID sceAudioChangeChannelVolume, 0x08ACCE78
	AddNID sceNetInetGetErrno, 0x08ACD288
	AddNID sceKernelPowerTick, 0x08ACD3B8
	AddNID sceGeListUpdateStallAddr, 0x08ACCF28
	AddNID sceNetInetSocket, 0x08ACD250
	AddNID sceNetAdhocMatchingTerm, 0x08ACD0E0
	AddNID sceDisplayWaitVblankStart, 0x08ACCEC8
	AddNID sceIoWrite, 0x08ACCB98
	AddNID sceUtilitySavedataGetStatus, 0x08ACD428
	AddNID sceAudioOutputPannedBlocking, 0x08ACCE40
	AddNID sceGeUnsetCallback, 0x08ACCEE8
	AddNID sceNetInetInetAddr, 0x08ACD268
	AddNID sceHttpSendRequest, 0x08ACCFA0
	AddNID sceAtracAddStreamData, 0x08ACCE18
	AddNID sceNetInetTerm, 0x08ACD260
	AddNID sceHttpReadData, 0x08ACCFC0
	AddNID sceNetAdhocPdpSend, 0x08ACD158
	AddNID __sceSasGetAllEnvelopeHeights, 0x08ACD2F0
	AddNID sceKernelGetThreadId, 0x08ACCCC8
	AddNID sceMpegFreeAvcEsBuf, 0x08ACD098
	AddNID sceRtcGetCurrentClockLocalTime, 0x08ACD2E0
	AddNID sceMpegAvcDecode, 0x08ACD008
	AddNID __sceSasRevType, 0x08ACD308
	AddNID sceIoOpen, 0x08ACCBB8
	AddNID sceAudioOutputBlocking, 0x08ACCE38
	AddNID __sceSasSetGrain, 0x08ACD390
	AddNID sceKernelWaitEventFlag, 0x08ACCCF0
	AddNID sceNetAdhocPdpDelete, 0x08ACD128
	AddNID sceNetAdhocctlScan, 0x08ACD188
	AddNID __sceSasSetKeyOn, 0x08ACD340
	AddNID sceNetAdhocMatchingStart, 0x08ACD0E8
	AddNID sceNetInetSendto, 0x08ACD200
	AddNID sceAtracReleaseAtracID, 0x08ACCE00
	AddNID sceUtilityGetSystemParamInt, 0x08ACD440
	AddNID sceUmdDeactivate, 0x08ACD3E0
	AddNID sceNetAdhocMatchingSetHelloOpt, 0x08ACD0F0
	AddNID sceKernelLibcClock, 0x08ACCDB0
	AddNID sceKernelRegisterExitCallback, 0x08ACCBE0
	AddNID sceLibFont_3AEA8CB6, 0x08ACCFD8
	AddNID sceMpegGetAvcAu, 0x08ACD0C0
	AddNID sceMpegInit, 0x08ACD050
	AddNID sceKernelStartThread, 0x08ACCCA8
	AddNID __sceSasRevEVOL, 0x08ACD398
	AddNID sceNetInetSetsockopt, 0x08ACD228
	AddNID sceGeListEnQueue, 0x08ACCF10
	AddNID sceAtracDecodeData, 0x08ACCE08
	AddNID sceKernelCpuResumeIntr, 0x08ACCBD0
	AddNID __sceSasSetSL, 0x08ACD328
	AddNID sceLibFont_0DA7535E, 0x08ACCFD0
	AddNID sceHttpCreateTemplate, 0x08ACCF78
	AddNID sceKernelDcacheWritebackRange, 0x08ACCDD8
	AddNID sceNetInit, 0x08ACD2C8
	AddNID sceAtracReinit, 0x08ACCDE8
	AddNID sceNetAdhocPtpOpen, 0x08ACD130
	AddNID __sceSasRevVON, 0x08ACD3B0
	AddNID sceNetApctlAddHandler, 0x08ACD1E8
	AddNID sceHttpGetStatusCode, 0x08ACCF58
	AddNID sceDisplayGetFramePerSec, 0x08ACCED0
	AddNID __sceSasSetPause, 0x08ACD348
	AddNID __sceSasInit, 0x08ACD310


















/*
# InterruptManager (4)
	AddNID sceKernelRegisterSubIntrHandler, 0x08A88550		# 0xCA04A2B9
	AddNID sceKernelReleaseSubIntrHandler, 0x08A88558		# 0xD61E6961
	AddNID sceKernelEnableSubIntr, 0x08A88560				# 0xFB8E22EC
	AddNID sceKernelRegisterUserSpaceIntrStack, 0x088003C8	# 0xEEE43F47

# IoFileMgrForUser (16)
	AddNID sceIoWrite, 0x08A88568				# 0x42EC03AC
	AddNID sceIoDevctl, 0x08A88570				# 0x54F5FB11
	AddNID sceIoRead, 0x08A88578				# 0x6A638D83
	AddNID sceIoLseekAsync, 0x08A88580			# 0x71B19E77
	AddNID sceIoRename, 0x08A88588				# 0x779103A0
	AddNID sceIoClose, 0x08A88590				# 0x810C4BC3
	AddNID sceIoOpenAsync, 0x08A88598			# 0x89AA9906
	AddNID sceIoReadAsync, 0x08A885A0			# 0xA0B5A7C2
	AddNID sceIoSetAsyncCallback, 0x08A885A8	# 0xA12A0514
	AddNID sceIoGetstat, 0x08A885B0				# 0xACE946E8
	AddNID sceIoIoctlAsync, 0x08A885B8			# 0xE95A012B
	AddNID sceIoCloseAsync, 0x08A885C0			# 0xFF5940B6
	AddNID sceIoOpen, 0x08A885C8				# 0x109F50BC
	AddNID sceIoLseek, 0x08A885D0				# 0x27EB27B8
	AddNID sceIoPollAsync, 0x08A885D8			# 0x3251EA56
	AddNID sceIoWaitAsyncCB, 0x08A885E0			# 0x35DBD746
	
# Kernel_Library (2)
	AddNID sceKernelCpuSuspendIntr, 0x08A885E8	# 0x092968F4
	AddNID sceKernelCpuResumeIntr, 0x08A885F0	# 0x5F10D406
	
# LoadExecForUser (2)
	AddNID sceKernelExitGame, 0x08A885F8				# 0x05572A5F
	AddNID sceKernelRegisterExitCallback, 0x08A88600	# 0x4AC57943
	
# ModuleMgrForUser (8)
	AddNID sceKernelUnloadModule, 0x08A88608			# 0x2E0911AA
	AddNID sceKernelGetModuleIdByAddress, 0x08A88610	# 0xD8B73127
	AddNID sceKernelGetModuleId, 0x08A88618				# 0xF0A26395
	AddNID sceKernelStartModule, 0x08A88620				# 0x50F0C1EC
	AddNID ModuleMgrForUser_8F2DF740, 0x08A88628		# 0x8F2DF740
	AddNID sceKernelLoadModule, 0x08A88630				# 0x977DE386
	AddNID sceKernelStopModule, 0x08A88638				# 0xD1FF982A
	AddNID sceKernelGetModuleGPByAddress, 0x09EA8E70	# 0xD2FBC957
	
# StdioForUser (3)
	AddNID sceKernelStdin, 0x08A88640	# 0x172D316E
	AddNID sceKernelStdout, 0x08A88648	# 0xA6BAB2E9
	AddNID sceKernelStderr, 0x08A88650	# 0xF78BA90A
	
# SysMemUserForUser (9)	
	AddNID sceKernelPrintf, 0x08A88658					# 0x13A5ABEF
	AddNID sceKernelSetCompilerVersion, 0x08A88660		# 0xF77D77CB
	AddNID sceKernelAllocPartitionMemory, 0x08A88668	# 0x237DBD4F
	AddNID sceKernelGetBlockHeadAddr, 0x08A88670		# 0x9D9A5BA1
	AddNID sceKernelFreePartitionMemory, 0x08A88678		# 0xB6D61D02
	AddNID SysMemUserForUser_EBD5C3E6, 0x08A88680		# 0xEBD5C3E6
	AddNID SysMemUserForUser_A6848DF8, 0x088003D0		# 0xA6848DF8
	AddNID sceKernelQueryMemoryInfo, 0x09E912B0			# 0x2A3E5280
	AddNID sceKernelGetCompiledSdkVersion, 0x09EA8E80 	# 0xFC114573
	
# ThreadManForUser (58)
	AddNID sceKernelDelayThread, 0x08A88688					# 0xCEADEB47
	AddNID sceKernelCreateSema, 0x08A88690					# 0xD6DA4BA1
	AddNID sceKernelGetSystemTime, 0x08A88698				# 0xDB738F35
	AddNID sceKernelTryReceiveMsgPipe, 0x08A886A0			# 0xDF52098F
	AddNID sceKernelSetEventFlag, 0x08A886A8				# 0x1FB15A32
	AddNID sceKernelCreateCallback, 0x08A886B0				# 0xE81CAF8F
	AddNID sceKernelChangeCurrentThreadAttr, 0x08A886B8		# 0xEA748E31
	AddNID sceKernelDeleteCallback, 0x08A886C0				# 0xEDBA5844
	AddNID sceKernelDeleteEventFlag, 0x08A886C8				# 0xEF9E4C70		
	AddNID sceKernelDeleteMsgPipe, 0x08A886D0				# 0xF0B7DA1C
	AddNID sceKernelStartThread, 0x08A886D8					# 0xF475845D
	AddNID sceKernelWaitThreadEnd, 0x08A886E0				# 0x278C0DF5
	AddNID sceKernelGetThreadId, 0x08A886F0					# 0x293B45B8
	AddNID sceKernelWaitEventFlagCB, 0x08A886F8				# 0x328C546A
	AddNID sceKernelDeleteSema, 0x08A886E8					# 0x28B6489C
	AddNID sceKernelCheckCallback, 0x08A88700				# 0x349D6D6C
	AddNID sceKernelGetSystemTimeLow, 0x08A88708			# 0x369ED59D
	AddNID sceKernelSignalSema, 0x08A88710					# 0x3F53E640
	AddNID sceKernelWaitEventFlag, 0x08A88718				# 0x402FCF22
	AddNID sceKernelCreateThread, 0x08A88720				# 0x446D8DE6
	AddNID sceKernelWaitSema, 0x08A88728					# 0x4E3A1105
	AddNID sceKernelGetThreadStackFreeSize, 0x08A88730		# 0x52089CA1
	AddNID sceKernelCreateEventFlag, 0x08A88738				# 0x55C20A00
	AddNID sceKernelDelayThreadCB, 0x08A88740				# 0x68DA9E36
	AddNID sceKernelWaitSemaCB, 0x08A88748					# 0x6D212BAC
	AddNID sceKernelChangeThreadPriority, 0x08A88750		# 0x71BC9871
	AddNID sceKernelResumeThread, 0x08A887581				# 0x75156E8F
	AddNID sceKernelCreateMsgPipe, 0x08A88760				# 0x7C0DC2A0
	AddNID sceKernelClearEventFlag, 0x08A88768				# 0x812346E4
	AddNID sceKernelGetSystemTimeWide, 0x08A88770			# 0x82BC5777
	AddNID sceKernelWaitThreadEndCB, 0x08A88778				# 0x840E8133
	AddNID sceKernelSendMsgPipe, 0x08A88780					# 0x876DBFAD
	AddNID sceKernelSuspendThread, 0x08A88788				# 0x9944F31F
	AddNID sceKernelDeleteThread, 0x08A88790				# 0x9FA03CD3
	AddNID sceKernelReferEventFlagStatus, 0x08A88798		# 0xA66B0120
	AddNID sceKernelExitThread, 0x08A887A0					# 0xAA73C935
	AddNID ThreadManForUser_31327F19, 0x088003D8			# 0x31327F19
	AddNID sceKernelReferLwMutexStatusByID, 0x088003E0		# 0x4C145944
	AddNID ThreadManForUser_7CFF8CF3, 0x088003E8			# 0x7CFF8CF3
	AddNID ThreadManForUser_BEED3A47, 0x088003F0			# 0xBEED3A47
	AddNID sceKernelCheckThreadStack, 0x09E828EC 			# 0xD13BDE95
	AddNID sceKernelPollSema, 0x09E82914					# 0x58B1F937
	AddNID sceKernelReferSemaStatus, 0x09E8291C				# 0xBC6FEBC5
	AddNID sceKernelSetAlarm, 0x09E91340					# 0x6652B8CA
	AddNID sceKernelCancelAlarm, 0x09E91348 				# 0x7E65B999
	AddNID sceKernelSysClock2USec, 0x09E91360 				# 0xBA6B92E2
	AddNID sceKernelPollEventFlag, 0x09EA8ED0				# 0x30FD48F0
	AddNID sceKernelTerminateThread, 0x09EA8F00				# 0x616403BA
	AddNID sceKernelSendMbx, 0x09EAF5C4 					# 0xE9B3061E
	AddNID sceKernelReferVplStatus, 0x09EAF5F4 				# 0x39810265
	AddNID sceKernelCreateVpl, 0x09EAF614 					# 0x56C039B5
	AddNID sceKernelCreateMbx, 0x09EAF62C 					# 0x8125221D
	AddNID sceKernelDeleteMbx, 0x09EAF634					# 0x86255ADA
	AddNID sceKernelDeleteVpl, 0x09EAF63C 					# 0x89B3D48C
	AddNID sceKernelTryAllocateVpl, 0x09EAF64C 				# 0xAF36D708
	AddNID sceKernelReceiveMbx, 0x09EAF654 					# 0x18260574
	AddNID sceKernelFreeVpl, 0x09EAF65C 					# 0xB736E9FF
	AddNID sceKernelCancelSema, 0x09EB32F8 					# 0x8FFDF9A2
	
# UtilsForUser (10)
	AddNID sceKernelLibcGettimeofday, 0x08A887A8				# 0x71EC4271
	AddNID sceKernelDcacheWritebackAll, 0x08A887B0				# 0x79D1C3FA
	AddNID sceKernelLibcClock, 0x08A887B8						# 0x91E4F6A7
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x08A887C0	# 0xB435DEC5
	AddNID sceKernelLibcTime, 0x08A887C8						# 0x27CC57F0
	AddNID sceKernelDcacheWritebackRange, 0x08A887D0			# 0x3EE30821
	AddNID sceKernelDcacheWritebackInvalidateRange, 0x09E82924	# 0x34B9FA9E
	AddNID sceKernelUtilsSha1BlockResult, 0x09E91368			# 0x585F1C09
	AddNID sceKernelUtilsSha1BlockInit, 0x09E91370				# 0xF8FCD5BA
	AddNID sceKernelUtilsSha1BlockUpdate, 0x09E91378			# 0x346F6DA8
	
# scePower (2)
	AddNID sceKernelPowerTick, 0x08A887D8		# 0x090CCB3F
	AddNID scePowerRequestSuspend, 0x09EA9060 	# 0xAC32C9CC
	
# sceGe_user (9)
	AddNID sceGeListSync, 0x08A887E0
	AddNID sceGeUnsetCallback, 0x08A887E8
	AddNID sceGeListEnQueueHead, 0x08A887F0
	AddNID sceGeEdramGetSize, 0x08A887F8
	AddNID sceGeSetCallback, 0x08A88800
	AddNID sceGeListEnQueue, 0x08A88808
	AddNID sceGeDrawSync, 0x08A88810
	AddNID sceGeListUpdateStallAddr, 0x08A88818
	AddNID sceGeEdramGetAddr, 0x08A88820
	
# sceDisplay (9)
	AddNID sceDisplaySetMode, 0x08A88828
	AddNID sceDisplaySetFrameBuf, 0x08A88830
	AddNID sceDisplayWaitVblank, 0x08A88838
	AddNID sceDisplayWaitVblankStartCB, 0x08A88840
	AddNID sceDisplayIsVblank, 0x08A88848
	AddNID sceDisplayWaitVblankStart, 0x08A88850
	AddNID sceDisplayGetVcount, 0x08A88858
	AddNID sceDisplayGetFramePerSec, 0x08A88860
	AddNID sceDisplayGetFrameBuf, 0x08A88868
	
# sceVideocodec (12)
	AddNID sceVideocodecInit, 0x09E82984 			# 0x17099F0A
	AddNID sceVideocodecGetFrameCrop, 0x09E8298C	# 0x17CF7D2C
	AddNID sceVideocodecGetVersion, 0x09E82994		# 0x26927D19
	AddNID sceVideocodecGetEDRAM, 0x09E8299C		# 0x2D31F5B1
	AddNID sceVideocodecDelete, 0x09E829A4			# 0x307E6E1C
	AddNID sceVideocodecReleaseEDRAM, 0x09E829AC	# 0x4F160BF4
	AddNID sceVideocodecGetSEI, 0x09E829B4			# 0x627B7D42
	AddNID sceVideocodecSetMemory, 0x09E829BC		# 0x745A7B7A
	AddNID sceVideocodec_893B32B1, 0x09E829C4		# 0x893B32B1
	AddNID sceVideocodecStop, 0x09E829CC			# 0xA2F0564E
	AddNID sceVideocodecOpen, 0x09E829D4			# 0xC01EC829
	AddNID sceVideocodecDecode, 0x09E829DC			# 0xDBA273FA
	
# sceMpeg (27)
	AddNID sceMpegRingbufferDestruct, 0x08A88C28
	AddNID sceMpegInitAu, 0x08A88C30
	AddNID sceMpegAvcQueryYCbCrSize, 0x08A88C38
	AddNID sceMpegQueryStreamOffset, 0x08A88C40
	AddNID sceMpegAvcCsc, 0x08A88C48
	AddNID sceMpegRingbufferConstruct, 0x08A88C50
	AddNID sceMpegRegistStream, 0x08A88C58
	AddNID sceMpegUnRegistStream, 0x08A88C60
	AddNID sceMpegDelete, 0x08A88C68
	AddNID sceMpegQueryStreamSize, 0x08A88C70
	AddNID sceMpegAvcInitYCbCr, 0x08A88C78
	AddNID sceMpegInit, 0x08A88C80
	AddNID sceMpegFlushAllStream, 0x08A88C88
	AddNID sceMpegAtracDecode, 0x08A88C90
	AddNID sceMpegFinish, 0x08A88C98
	AddNID sceMpegMallocAvcEsBuf, 0x08A88CA0
	AddNID sceMpegRingbufferPut, 0x08A88CA8
	AddNID sceMpegRingbufferAvailableSize, 0x08A88CB0
	AddNID sceMpegQueryMemSize, 0x08A88CB8
	AddNID sceMpegFreeAvcEsBuf, 0x08A88CC0
	AddNID sceMpegRingbufferQueryMemSize, 0x08A88CC8
	AddNID sceMpegCreate, 0x08A88CD0
	AddNID sceMpegGetAtracAu, 0x08A88CD8
	AddNID sceMpegAvcDecodeYCbCr, 0x08A88CE0
	AddNID sceMpegAvcDecodeStopYCbCr, 0x08A88CE8
	AddNID sceMpegQueryAtracEsSize, 0x08A88CF0
	AddNID sceMpegGetAvcAu, 0x08A88CF8
	
# sceMpegbase (6)
	AddNID sceMpegbase_0530BE4E, 0x09E82954 	# 0x0530BE4E
	AddNID sceMpegBaseCscAvcRange, 0x09E8295C 	# 0x304882E1
	AddNID sceMpegBaseCscInit, 0x09E82964 		# 0x492B5E4B
	AddNID sceMpegBaseYCrCbCopy, 0x09E8296C 	# 0x7AC0321A
	AddNID sceMpegBaseCscAvc, 0x09E82974 		# 0x91929A21
	AddNID sceMpegbase_BEA18F91, 0x09E8297C 	# 0xBEA18F91	
	
# sceCtrl (4)
	AddNID sceCtrlSetSamplingMode, 0x08A88870
	AddNID sceCtrlReadBufferPositive, 0x08A88878
	AddNID sceCtrlSetSamplingCycle, 0x08A88880
	AddNID sceCtrlSetIdleCancelThreshold, 0x08A88888
	
# sceRtc (2)
	AddNID sceRtcGetAccumulativeTime, 0x08A88890
	AddNID sceRtcGetCurrentClockLocalTime, 0x08A88898
	
# sceUmdUser (9)
	AddNID sceUmdGetErrorStat, 0x08A888A0
	AddNID sceUmdCheckMedium, 0x08A888A8
	AddNID sceUmdWaitDriveStatCB, 0x08A888B0
	AddNID sceUmdCancelWaitDriveStat, 0x08A888B8
	AddNID sceUmdGetDriveStat, 0x08A888C0
	AddNID sceUmdWaitDriveStat, 0x08A888C8
	AddNID sceUmdRegisterUMDCallBack, 0x08A888D0
	AddNID sceUmdUnRegisterUMDCallBack, 0x08A888D8
	AddNID sceUmdActivate, 0x08A888E0
	
# sceSasCore (18)
	AddNID __sceSasRevParam, 0x08A888E8
	AddNID __sceSasRevType, 0x08A888F0
	AddNID __sceSasInit, 0x08A888F8
	AddNID __sceSasSetVolume, 0x08A88900
	AddNID __sceSasCoreWithMix, 0x08A88908
	AddNID __sceSasGetEndFlag, 0x08A88910
	AddNID __sceSasGetEnvelopeHeight, 0x08A88918
	AddNID __sceSasSetKeyOn, 0x08A88920
	AddNID __sceSasSetPause, 0x08A88928
	AddNID __sceSasSetVoice, 0x08A88930
	AddNID __sceSasSetKeyOff, 0x08A88938
	AddNID __sceSasCore, 0x08A88940
	AddNID __sceSasSetPitch, 0x08A88948
	AddNID __sceSasSetNoise, 0x08A88950
	AddNID __sceSasSetSimpleADSR, 0x08A88958
	AddNID __sceSasRevEVOL, 0x08A88960
	AddNID __sceSasGetOutputmode, 0x08A88968
	AddNID __sceSasRevVON, 0x08A88970
	
# sceAtrac3plus (10)
	AddNID sceAtracSetData, 0x08A88978
	AddNID sceAtracReinit, 0x08A88980
	AddNID sceAtracGetNextSample, 0x08A88988
	AddNID sceAtracGetStreamDataInfo, 0x08A88990
	AddNID sceAtracReleaseAtracID, 0x08A88998
	AddNID sceAtracDecodeData, 0x08A889A0
	AddNID sceAtracGetAtracID, 0x08A889A8
	AddNID sceAtracAddStreamData, 0x08A889B0
	AddNID sceAtracSetLoopNum, 0x08A889B8
	AddNID sceAtracGetRemainFrame, 0x08A889C0
	
# sceAudio (9)
	AddNID sceAudioOutput2Reserve, 0x08A889C8
	AddNID sceAudioOutputPannedBlocking, 0x08A889D0
	AddNID sceAudioOutput2OutputBlocking, 0x08A889D8
	AddNID sceAudioOutput2Release, 0x08A889E0
	AddNID sceAudioChReserve, 0x08A889E8
	AddNID sceAudioChRelease, 0x08A889F0
	AddNID sceAudioChangeChannelConfig, 0x08A889F8
	AddNID sceAudioChangeChannelVolume, 0x08A88A00
	AddNID sceAudioSetChannelDataLen, 0x08A88A08
	
# sceAudiocodec (6)
	AddNID sceAudiocodecReleaseEDRAM, 0x09E76DD0 	# 0x29681260
	AddNID sceAudiocodecGetEDRAM,0x09E76DD8			# 0x3A20A200
	AddNID sceAudiocodec_3DD7EE1A, 0x09E76DE0		# 0x3DD7EE1A
	AddNID sceAudiocodecInit, 0x09E76DE8			# 0x5B37EB1D
	AddNID sceAudiocodecDecode, 0x09E76DF0			# 0x70A703F8
	AddNID sceAudiocodecCheckNeedMem, 0x09E76DF8	# 0x9D3F790C
	
# sceUtility (23)
	AddNID sceUtilityLoadModule, 0x08A88A10					# 0x2A2B3DE0
	AddNID sceUtilityMsgDialogInitStart, 0x08A88A18			# 0x2AD8E239
	AddNID sceUtilityOskShutdownStart, 0x08A88A20			# 0x3DFAEBA9
	AddNID sceUtilityOskUpdate, 0x08A88A28					# 0x4B85C861
	AddNID sceUtilityNetconfInitStart, 0x08A88A30			# 0x4DB1E739
	AddNID sceUtilitySavedataInitStart, 0x08A88A38			# 0x50C4CD57
	AddNID sceUtilityNetconfGetStatus, 0x08A88A40			# 0x6332AA39
	AddNID sceUtilityMsgDialogShutdownStart, 0x08A88A48		# 0x67AF3428
	AddNID sceUtilityGameSharingUpdate, 0x08A88A50			# 0x7853182D
	AddNID sceUtilitySavedataGetStatus, 0x08A88A58			# 0x8874DBE0
	AddNID sceUtilityNetconfUpdate, 0x08A88A60				# 0x91E70E35
	AddNID sceUtilityGameSharingGetStatus, 0x08A88A68		# 0x946963F3
	AddNID sceUtilityMsgDialogUpdate, 0x08A88A70			# 0x95FC253B
	AddNID sceUtilitySavedataShutdownStart, 0x08A88A78		# 0x9790B33C
	AddNID sceUtilityMsgDialogGetStatus, 0x08A88A80			# 0x9A1C91D7
	AddNID sceUtilityGetSystemParamInt, 0x08A88A88			# 0xA5DA2406
	AddNID sceUtilityGameSharingInitStart, 0x08A88A90		# 0xC492F751
	AddNID sceUtilitySavedataUpdate, 0x08A88A98				# 0xD4B95FFB
	AddNID sceUtilityGameSharingShutdownStart, 0x08A88AA0	# 0xEFC6F80F
	AddNID sceUtilityOskGetStatus, 0x08A88AA8				# 0xF3F76017
	AddNID sceUtilityOskInitStart, 0x08A88AB0				# 0xF6269B82
	AddNID sceUtilityNetconfShutdownStart, 0x08A88AB8		# 0xF88155F6
	AddNID sceUtilityGetSystemParamString, 0x09EA9068 		# 0x34B78343
	
# sceNetAdhoc (15)
	AddNID sceNetAdhocPtpClose, 0x08A88AC0			# 0x157E6225
	AddNID sceNetAdhocPtpSend, 0x08A88AC8			# 0x4DA4C788
	AddNID sceNetAdhocPdpCreate, 0x08A88AD0			# 0x6F92741B
	AddNID sceNetAdhocPdpDelete, 0x08A88AD8			# 0x7F27BB5E
	AddNID sceNetAdhocPtpOpen, 0x08A88AE0			# 0x877F6D66
	AddNID sceNetAdhocPtpRecv, 0x08A88AE8			# 0x8BEA2B3E
	AddNID sceNetAdhocPtpFlush, 0x08A88AF0			# 0x9AC2EEAC
	AddNID sceNetAdhocPtpAccept, 0x08A88AF8			# 0x9DF81198
	AddNID sceNetAdhocTerm, 0x08A88B00				# 0xA62C6F57
	AddNID sceNetAdhocPdpSend, 0x08A88B08			# 0xABED3790
	AddNID sceNetAdhocPdpRecv, 0x08A88B10			# 0xDFE53E03
	AddNID sceNetAdhocPtpListen, 0x08A88B18			# 0xE08BDAC1
	AddNID sceNetAdhocInit, 0x08A88B20				# 0xE1D621D7
	AddNID sceNetAdhocPtpConnect, 0x08A88B28		# 0xFC6FC07B
	AddNID sceNetAdhocSetSocketAlert, 0x09EAF674	# 0x73BFD52D
	
# sceNetAdhocctl (11)
	AddNID sceNetAdhocctlScan, 0x08A88B30			# 0x08FFF7A0
	AddNID sceNetAdhocctlAddHandler, 0x08A88B38		# 0x20B317A0
	AddNID sceNetAdhocctlDisconnect, 0x08A88B40		# 0x34401D65
	AddNID sceNetAdhocctlJoin, 0x08A88B48			# 0x5E7F79C9
	AddNID sceNetAdhocctlDelHandler, 0x08A88B50		# 0x6402490B
	AddNID sceNetAdhocctlGetScanInfo, 0x08A88B58	# 0x81AEE1BE
	AddNID sceNetAdhocctlGetPeerInfo, 0x08A88B60	# 0x8DB83FDC
	AddNID sceNetAdhocctlTerm, 0x08A88B68			# 0x9D689E13
	AddNID sceNetAdhocctlGetPeerList, 0x08A88B70	# 0xE162CB14
	AddNID sceNetAdhocctlInit, 0x08A88B78			# 0xE26F226E
	AddNID sceNetAdhocctlCreate, 0x08A88B80			# 0xEC0635C1
	
# sceNetAdhocctl_lib (5)
	AddNID sceNetAdhocctl_lib_1572422C, 0x09EB47B4 	# 0x1572422C
	AddNID sceNetAdhocctl_lib_1A2B6D03, 0x09EB47BC 	# 0x1A2B6D03
	AddNID LinkDiscoverSkip, 0x09EB47C4 			# 0x1C679240
	AddNID sceNetAdhocctl_lib_F6057CC4, 0x09EB47CC 	# 0xF6057CC4
	AddNID sceNetAdhocctl_lib_F8BABD85, 0x09EB47D4 	# 0xF8BABD85
	
# sceNet (6)
	AddNID sceNetTerm, 0x08A88B88				# 0x281928A9
	AddNID sceNetInit, 0x08A88B90				# 0x39AF39A6
	AddNID sceNetEtherNtostr, 0x08A88B98		# 0x89360950
	AddNID sceNetEtherStrton, 0x08A88BA0		# 0xD27961C9
	AddNID sceNetGetLocalEtherAddr, 0x09EA2C34	# 0x0BF0A3AE
	AddNID sceNetFreeThreadinfo, 0x09EAF664		# 0x50647530
	
# sceNetAdhocMatching (12)
	AddNID sceNetAdhocMatchingInit, 0x08A88BA8					# 0x2A2A1E07
	AddNID sceNetAdhocMatchingStop, 0x08A88BB0					# 0x32B156B3
	AddNID sceNetAdhocMatchingSelectTarget, 0x08A88BB8			# 0x5E3D4B79
	AddNID sceNetAdhocMatchingTerm, 0x08A88BC0					# 0x7945ECDA
	AddNID sceNetAdhocMatchingCancelTargetWithOpt, 0x08A88BC8	# 0x8F58BEDF
	AddNID sceNetAdhocMatchingStart, 0x08A88BD0					# 0x93EF3843
	AddNID sceNetAdhocMatchingSetHelloOpt, 0x08A88BD8			# 0xB58E61B7
	AddNID sceNetAdhocMatchingCreate, 0x08A88BE0				# 0xCA5EDA6F
	AddNID sceNetAdhocMatchingCancelTarget, 0x08A88BE8			# 0xEA3C6108
	AddNID sceNetAdhocMatchingAbortSendData, 0x08A88BF0			# 0xEC19337D
	AddNID sceNetAdhocMatchingDelete, 0x08A88BF8				# 0xF16EAF4F
	AddNID sceNetAdhocMatchingSendData, 0x08A88C00				# 0xF79472D7
		
# sceImpose (4)
	AddNID sceImposeGetLanguageMode, 0x08A88C08
	AddNID sceImposeSetLanguageMode, 0x08A88C10
	AddNID sceImposeSetUMDPopup, 0x08A88C18	
	# Wrong address??????? (1)
	AddNID scePower_EBD177D6, 0x08A88C20
	
# scePsmf (11)
	AddNID scePsmf_0BA514E5, 0x08A88D00
	AddNID scePsmf_4BC9BDE0, 0x08A88D08
	AddNID scePsmf_68D42328, 0x08A88D10
	AddNID scePsmf_C22C8327, 0x08A88D18
	AddNID scePsmf_C7DB3A5B, 0x08A88D20
	AddNID scePsmf_E1283895, 0x08A88D28
	AddNID scePsmf_EAED89CD, 0x08A88D30
	AddNID sceWlanGetEtherAddr, 0x08A88D38
	AddNID sceWlanGetSwitchState, 0x08A88D40
	AddNID sceOpenPSIDGetOpenPSID, 0x08A88D48
	AddNID sceHprmIsHeadphoneExist, 0x08A88D50
	
# sceNetIfhandle_lib (24)
	AddNID sceNetIfhandleIfIoctl, 0x09E91380			# 0x0296C7D6
	AddNID sceNetIfhandle_lib_0B258B5E, 0x09E91388		# 0x0B258B5E
	AddNID sceNetIfhandle_lib_0C391E9F, 0x09E91390		# 0x0C391E9F
	AddNID sceNetIfhandle_lib_0FB8AE0D, 0x09E91398		# 0x0FB8AE0D
	AddNID sceNetMallocInternal, 0x09E913A0				# 0x15CFE3C0
	AddNID sceNetIfhandle_lib_29ED84C5, 0x09E913A8		# 0x29ED84C5
	AddNID sceNetIfhandleInit, 0x09E913B0				# 0x30F69334
	AddNID sceNetIfhandle_lib_35FAB6A2, 0x09E913B8		# 0x35FAB6A2
	AddNID sceNetGetMallocStatInternal, 0x09E913C0		# 0x4C2886CB
	AddNID sceNetIfhandleGetDetachEther, 0x09E913C8		# 0x4FB43BCE
	AddNID sceNetFreeInternal, 0x09E913D0				# 0x76BAD213
	AddNID sceNetIfhandleIfUp, 0x09E913D8				# 0x8FCB05A1
	AddNID sceNetIfhandleIfStart, 0x09E913E0			# 0xB1F5BB87
	AddNID sceNetIfhandleGetAttachEther, 0x09E913E8		# 0xB8188F96
	AddNID sceNetIfhandleTerm, 0x09E913F0				# 0xB9096E48
	AddNID sceNetMAdj, 0x09E913F8						# 0xBFF3CEA5
	AddNID sceNetIfhandleIfDequeue, 0x09E91400			# 0xE440A7D8
	AddNID sceNetIfhandleIfDown, 0x09E91408				# 0xEAD3A759
	AddNID sceNetMFreem, 0x09E91410						# 0xF56FAC82
	AddNID sceNetIfhandle_driver_07505747, 0x09EA2C4C	# 0x07505747
	AddNID sceNetMCat, 0x09EA2C5C						# 0x3E8DD3F8
	AddNID sceNetMPullup, 0x09EA2C64					# 0x49EDBB18
	AddNID sceNetMCopydata, 0x09EA2C74					# 0x9A6261EC
	AddNID sceNetMFree, 0x09EA2C8C						# 0xF8825DC4
	
# sceWlanDrv_lib (12)
	AddNID sceWlanDrv_lib_8D5F551B, 0x09E91418		# 0x8D5F551B
	AddNID sceWlanDevIsGameMode,0x09EA2D44 			# 0x5E7C8D94
	AddNID sceWlanGPPrevEstablishActive, 0x09E91378	# 0x5ED4049A
	AddNID sceWlanGPRecv, 0x09EA2D54				# 0xA447103A
	AddNID sceWlanGPSend, 0x09EA2D5C				# 0xB4D7CB74
	AddNID sceWlanDrv_lib_2D0FAE4E, 0x09EA9080 		# 0x2D0FAE4E
	AddNID sceWlanDevAttach, 0x09EA9088 			# 0x482CAE9A
	AddNID sceWlanDrv_lib_56F467CA, 0x09EA9090 		# 0x56F467CA
	AddNID sceWlanDrv_lib_5BAA1FE5, 0x09EA9098 		# 0x5BAA1FE5
	AddNID sceWlanSetHostDiscover, 0x09EA90A0 		# 0x749B813A
	AddNID sceWlanDevDetach, 0x09EA90A8 			# 0xC9A8CAB7
	AddNID sceWlanSetWakeUp, 0x09EA90B0 			# 0xFE8A0B46
	
# sceNetAdhocAuth_lib (14)
	AddNID sceNetAdhocAuth_lib_015A8A64, 0x09EA2C3C		# 0x015A8A64
	AddNID sceNetAdhocAuth_lib_AAB06250, 0x09EA2C44		# 0xAAB06250
	AddNID sceNetAdhocAuth_lib_1F9A90B8, 0x09EA8F70		# 0x1F9A90B8
	AddNID sceNetAdhocAuth_lib_2E6AA271, 0x09EA8F78 	# 0x2E6AA271
	AddNID sceNetAdhocAuth_lib_312BD812, 0x09EA8F80 	# 0x312BD812
	AddNID sceNetAdhocAuthTerm, 0x09EA8F88 				# 0x6074D8F1
	AddNID sceNetAdhocAuth_lib_6CE209A3, 0x09EA8F90 	# 0x6CE209A3
	AddNID sceNetAdhocAuth_lib_72AAC6D3, 0x09EA8F98 	# 0x72AAC6D3
	AddNID sceNetAdhocAuth_lib_76F26AB0, 0x09EA8FA0 	# 0x76F26AB0
	AddNID sceNetAdhocAuthInit, 0x09EA8FA8 				# 0x86004235
	AddNID sceNetAdhocAuth_lib_89F2A732, 0x09EA8FB0 	# 0x89F2A732
	AddNID sceNetAdhocAuth_driver_89F2A732, 0x09EA8FB0 	# 0x89F2A732 Wrong library??
	AddNID sceNetAdhocAuth_driver_BD144DA6, 0x09EA8FB8	# 0xBD144DA6 Wrong library??
	AddNID sceNetAdhocAuth_driver_CF4D9BED, 0x09EA8FC0 	# 0xCF4D9BED Wrong library??
	
# sceNet_lib (38)
	AddNID sceNet_lib_05D525E4, 0x09EA2C94			# 0x05D525E4
	AddNID sceNet_lib_0A5A8751, 0x09EA2C9C			# 0x0A5A8751
	AddNID sceNetRand, 0x09EA2CA4					# 0x1858883D
	AddNID sceNet_lib_1F94AFD9, 0x09EA2CAC			# 0x1F94AFD9
	AddNID _sce_pspnet_callout_init, 0x09EA2CB4		# 0x25CC373A
	AddNID sceNet_lib_2E005032, 0x09EA2CBC			# 0x2E005032
	AddNID sceNet_lib_33B230BD,0x09EA2CC4 			# 0x33B230BD
	AddNID _sce_pspnet_splunlock,0x09EA2CCC 		# 0x515B2F33
	AddNID sceNetMemset, 0x09EA8FE8					# 0x57CC57DA
	AddNID sceNetLook_ctype_table, 0x09EA8FF0		# 0x750F705D
	AddNID sceNet_lib_7BA3ED91, 0x09EA8FF8			# 0x7BA3ED91
	AddNID sceNet_lib_A55C914F, 0x09EA9000			# 0xA55C914F	
	AddNID sceNetConfigSetIfEventFlag, 0x09EA9008	# 0xAB7DD9A5
	AddNID sceNet_lib_AFA11338, 0x09EA9010			# 0xAFA11338
	AddNID sceNet_lib_B20F84F8, 0x09EA9018			# 0xB20F84F8
	AddNID sceNetStrncpy, 0x09EA9020				# 0xB5CE388A
	AddNID sceNetConfigGetIfEvent, 0x09EA9028		# 0xD1BE2CE9
	AddNID sceNetConfigDownInterface, 0x09EA9030	# 0xD2422E4D
	AddNID sceNet_lib_D5B64E37, 0x09EA9038 			# 0xD5B64E37
	AddNID sceNetStrlen, 0x09EA9040					# 0xD8722983
	AddNID sceNet_lib_DA02F383, 0x09EA9048			# 0xDA02F383
	AddNID sceNetMemcmp, 0x09EA9050					# 0xE0A81C7C
	AddNID sceNetMemcpy, 0x09EA9058					# 0xEF097716
	AddNID sceNet_lib_6B294EE4, 0x09EA2CDC 			# 0x6B294EE4
	AddNID sceNet_lib_757085B0, 0x09EA2CE4 			# 0x757085B0
	AddNID _sce_pspnet_wakeup, 0x09EA2CEC 			# 0x7574FDA1
	AddNID sceNet_lib_949F1FBB, 0x09EA2CF4 			# 0x949F1FBB
	AddNID _sce_pspnet_spllock, 0x09EA2CFC 			# 0x94B44F26
	AddNID sceNet_lib_A8B6205A, 0x09EA2D04 			# 0xA8B6205A
	AddNID _sce_pspnet_callout_stop, 0x09EA2D0C 	# 0xA93A93E9
	AddNID sceNet_lib_B3A48B7F, 0x09EA2D14 			# 0xB3A48B7F
	AddNID _sce_pspnet_thread_enter, 0x09EA2D1C 	# 0xCA3CF5EB
	AddNID sceNet_lib_D60225A3, 0x09EA2D24 			# 0xD60225A3
	AddNID sceNet_lib_F6DB0A0B, 0x09EA2D3C 			# 0xF6DB0A0B
	AddNID sceNet_lib_03164B12, 0x09EA8FC8 			# 0x03164B12
	AddNID sceNet_lib_0D633F53, 0x09EA8FD0 			# 0x0D633F53
	AddNID sceNet_lib_389728AB, 0x09EA8FD8 			# 0x389728AB
	AddNID sceNetConfigUpInterface, 0x09EA8FE0 		# 0x5216CBF5
	
# sceWlanDrv (2)
	AddNID sceWlanGetEtherAddr, 0x09EAF6B4 		# 0x0C622081
	AddNID sceWlanGetSwitchState, 0x09EA9078 	# 0xD7763699
	
# sceReg (6)
	AddNID sceRegCloseCategory, 0x09EC6B04 		# 0x0CAE832B
	AddNID sceRegOpenCategory, 0x09EC6B0C 		# 0x1D8A762E
	AddNID sceRegGetKeyValue, 0x09EC6B14 		# 0x28A8E98A
	AddNID sceRegOpenRegistry, 0x09EC6B1C		# 0x92E41280
	AddNID sceRegGetKeyInfo, 0x09EC6B24 		# 0xD4475AA8
	AddNID sceRegCloseRegistry, 0x09EC6B2C 		# 0xFA8A5739

# syslib Patapon exports
	AddNID module_start_thread_parameter, 0x08AA8160
	AddNID module_info, 0x08A88FD0
	AddNID module_stop_thread_parameter, 0x08AA8170
	AddNID module_stop, 0x0897AC7C	
	AddNID module_start, 0x0897A8D8	
	AddNID syslib_11B97506, 0x08ACB0C0
*/
	.ident	"VAL-SDK"

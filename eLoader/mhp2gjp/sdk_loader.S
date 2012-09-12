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

# InterruptManager (4)3
	AddNID sceKernelRegisterSubIntrHandler, 0x0890B8E0		# 0xCA04A2B9
	AddNID sceKernelReleaseSubIntrHandler, 0x0890B8E8		# 0xD61E6961
	AddNID sceKernelEnableSubIntr, 0x0890B8F0				# 0xFB8E22EC

# IoFileMgrForUser (16)10
	AddNID sceIoWrite, 0x0890B8F8				# 0x42EC03AC
	AddNID sceIoDevctl, 0x0890B900				# 0x54F5FB11
	AddNID sceIoRead, 0x0890B908				# 0x6A638D83
	AddNID sceIoRename, 0x0890B910				# 0x779103A0
	AddNID sceIoClose, 0x0890B918				# 0x810C4BC3
	AddNID sceIoGetstat, 0x0890B920				# 0xACE946E8
	AddNID sceIoDopen, 0x0890B928				# 0x0890B930
	AddNID sceIoDclose, 0x0890B930				# 0xEB092469
	AddNID sceIoOpen, 0x0890B938				# 0x109F50BC
	AddNID sceIoLseek, 0x0890B940				# 0x27EB27B8
	
# Kernel_Library (2)2
	AddNID sceKernelCpuSuspendIntr, 0x0890B948	# 0x092968F4
	AddNID sceKernelCpuResumeIntr, 0x0890B950	# 0x5F10D406
	
# LoadExecForUser (2)2
	AddNID sceKernelExitGame, 0x0890B958				# 0x05572A5F
	AddNID sceKernelRegisterExitCallback, 0x0890B960	# 0x4AC57943
	
# ModuleMgrForUser (8)7
	AddNID sceKernelUnloadModule, 0x0890B968			# 0x2E0911AA
	AddNID sceKernelGetModuleIdByAddress, 0x0890B970	# 0xD8B73127
	AddNID sceKernelGetModuleId, 0x0890B978				# 0xF0A26395
	AddNID sceKernelStartModule, 0x0890B980				# 0x50F0C1EC
	AddNID ModuleMgrForUser_8F2DF740, 0x0890B988		# 0x8F2DF740
	AddNID sceKernelLoadModuleByID, 0x0890B990			# 0xB7F46618 
	AddNID sceKernelStopModule, 0x0890B998				# 0xD1FF982A
	
# StdioForUser (3)3
	AddNID sceKernelStdin, 0x0890B9A0	# 0x172D316E
	AddNID sceKernelStdout, 0x0890B9A8	# 0xA6BAB2E9
	AddNID sceKernelStderr, 0x0890B9B0	# 0xF78BA90A
	
# SysMemUserForUser (9)	6
	AddNID sceKernelPrintf, 0x0890B9B8					# 0x13A5ABEF
	AddNID sceKernelSetCompilerVersion, 0x0890B9E0		# 0xF77D77CB
	AddNID sceKernelAllocPartitionMemory, 0x0890B9C0	# 0x237DBD4F
	AddNID sceKernelGetBlockHeadAddr, 0x0890B9D0		# 0x9D9A5BA1
	AddNID sceKernelFreePartitionMemory, 0x0890B9D8		# 0xB6D61D02
	AddNID SysMemUserForUser_342061E5, 0x0890B9C8		# 0x342061E5

	
# ThreadManForUser (58)38
	AddNID sceKernelStartVTimer, 0x0890B9E8					# 0xC68D9437
	AddNID sceKernelDelayThread, 0x0890B9F0					# 0xCEADEB47
	AddNID sceKernelWakeupThread, 0x0890B9F8				# 0xD59EAD2F
	AddNID sceKernelSetEventFlag, 0x0890BA00				# 0x1FB15A32
	AddNID sceKernelCreateSema, 0x0890BA08					# 0xD6DA4BA1
	AddNID sceKernelSysClock2USecWide, 0x0890BA10			# 0xE1619D7C
	AddNID sceKernelCreateCallback, 0x0890BA18				# 0xE81CAF8F
	AddNID sceKernelCreateVTimer, 0x0890BA20				# 0x20FFF560
	AddNID sceKernelChangeCurrentThreadAttr, 0x0890BA28		# 0xEA748E31
	AddNID sceKernelDeleteEventFlag, 0x0890BA30				# 0xEF9E4C70
	AddNID sceKernelStartThread, 0x0890BA38					# 0xF475845D
	AddNID sceKernelWaitThreadEnd, 0x0890BA40				# 0x278C0DF5
	AddNID sceKernelResumeDispatchThread, 0x0890BA48		# 0x27E22EC2
	AddNID sceKernelDeleteSema, 0x0890BA50					# 0x28B6489C
	AddNID sceKernelGetThreadId, 0x0890BA58					# 0x293B45B8
	AddNID sceKernelPollEventFlag, 0x0890BA60				# 0x30FD48F0
	AddNID sceKernelGetSystemTimeLow, 0x0890BA68			# 0x369ED59D
	AddNID sceKernelTerminateDeleteThread, 0x0890BA70		# 0x383F7BCC 
	AddNID sceKernelSuspendDispatchThread, 0x0890BA78		# 0x3AD58B8C 
	AddNID sceKernelGetThreadExitStatus, 0x0890BA80			# 0x3B183E26
	AddNID sceKernelSignalSema, 0x0890BA88					# 0x3F53E640
	AddNID sceKernelWaitEventFlag, 0x0890BA90				# 0x402FCF22
	AddNID sceKernelCreateThread, 0x0890BA98				# 0x446D8DE6
	AddNID sceKernelWaitSema, 0x0890BAA0					# 0x4E3A1105
	AddNID sceKernelSetVTimerHandlerWide, 0x0890BAA8		# 0x53B00E9A
	AddNID sceKernelCreateEventFlag, 0x0890BAB0				# 0x55C20A00
	AddNID sceKernelPollSema, 0x0890BAB8					# 0x58B1F937
	AddNID sceKernelTerminateThread, 0x0890BAC0				# 0x616403BA
	AddNID sceKernelDelayThreadCB, 0x0890BAC8				# 0x68DA9E36
	AddNID sceKernelChangeThreadPriority, 0x0890BAD0		# 0x71BC9871
	AddNID sceKernelExitDeleteThread, 0x0890BAD8			# 0x809CE29B
	AddNID sceKernelClearEventFlag, 0x0890BAE0				# 0x812346E4
	AddNID sceKernelSleepThreadCB, 0x0890BAE8				# 0x82826F70
	AddNID sceKernelGetSystemTimeWide, 0x08A88770			# 0x82BC5777
	AddNID sceKernelGetThreadCurrentPriority, 0x0890BAF8	# 0x94AA61EE
	AddNID sceKernelSleepThread, 0x0890BB00					# 0x9ACE131E
	AddNID sceKernelDeleteThread, 0x08A88790				# 0x9FA03CD3
	AddNID sceKernelExitThread, 0x08A887A0					# 0xAA73C935
	
# UtilsForUser (10)8
	AddNID sceKernelLibcGettimeofday, 0x0890BB18				# 0x71EC4271
	AddNID sceKernelDcacheWritebackAll, 0x0890BB20				# 0x79D1C3FA
	AddNID sceKernelLibcClock, 0x0890BB28						# 0x91E4F6A7
	AddNID sceKernelIcacheInvalidateAll, 0x0890BB30				# 0x920F104A 
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x0890BB38	# 0xB435DEC5
	AddNID sceKernelDcacheInvalidateRange, 0x0890BB40			# 0xBFA98062 
	AddNID sceKernelLibcTime, 0x0890BB48						# 0x27CC57F0
	AddNID sceKernelDcacheWritebackRange, 0x0890BB50			# 0x3EE30821

	
# sceSuspendForUser ()3
	AddNID sceKernelPowerTick, 0x0890C100			# 0x090CCB3F
	AddNID sceKernelVolatileMemLock, 0x0890C108 	# 0x3E0271D3
	AddNID sceKernelVolatileMemUnlock, 0x0890C110 	# 0xA569E425
	
# sceGe_user (9)12
	AddNID sceGeListSync, 0x0890BC20
	AddNID sceGeUnsetCallback, 0x0890BC28
	AddNID sceGeListEnQueueHead, 0x0890BC30
	AddNID sceGeEdramGetSize, 0x0890BC38
	AddNID sceGeContinue, 0x0890BC40
	AddNID sceGeSetCallback, 0x0890BC48
	AddNID sceGeListEnQueue, 0x0890BC50
	AddNID sceGeDrawSync, 0x0890BC58
	AddNID sceGeBreak, 0x0890BC60
	AddNID sceGeEdramSetAddrTranslation, 0x0890BC68
	AddNID sceGeListUpdateStallAddr, 0x0890BC70
	AddNID sceGeEdramGetAddr, 0x0890BC78
	
# sceDisplay (9)2
	AddNID sceDisplaySetMode, 0x0890BC08
	AddNID sceDisplaySetFrameBuf, 0x0890BC10
	
# sceCtrl (4)3
	AddNID sceCtrlSetSamplingMode, 0x0890BBF0
	AddNID sceCtrlReadBufferPositive, 0x0890BBF8
	AddNID sceCtrlSetSamplingCycle, 0x0890BC00
		
# sceLibFont()9
	AddNID sceFontFindOptimumFont, 0x0890BD60	#0x099EF33C
	AddNID sceFontGetFontInfo, 0x0890BD68		#0x0DA7535E
	AddNID sceFontGetNumFontList, 0x0890BD70	#0x27F6E642
	AddNID sceFontClose, 0x0890BD78				#0x3AEA8CB6
	AddNID sceFontDoneLib, 0x0890BD80			#0x574B6FBC
	AddNID sceFontNewLib, 0x0890BD88			#0x67F17ED7
	AddNID sceFontGetCharGlyphImage, 0x0890BD90 #0x980F4895
	AddNID sceFontOpen, 0x0890BD98				#0xA834319D
	AddNID sceFontGetCharInfo, 0x0890BDA0		#0xDCC80C2F

# sceSasCore (18)26
	AddNID __sceSasSetADSR, 0x0890C030
	AddNID __sceSasGetAllEnvelopeHeights, 0x0890C038
	AddNID __sceSasRevParam, 0x0890C048
	AddNID __sceSasGetPauseFlag, 0x0890C050	
	AddNID __sceSasRevType, 0x0890C050
	AddNID __sceSasInit, 0x0890C058
	AddNID __sceSasSetVolume, 0x0890C060
	AddNID __sceSasCoreWithMix, 0x0890C068
	AddNID __sceSasSetSL, 0x0890C070
	AddNID __sceSasGetEndFlag, 0x0890C078
	AddNID __sceSasGetEnvelopeHeight, 0x0890C080
	AddNID __sceSasSetKeyOn, 0x0890C088
	AddNID __sceSasSetPause, 0x0890C090
	AddNID __sceSasSetVoice, 0x0890C098
	AddNID __sceSasSetADSRmode, 0x0890C0A0
	AddNID __sceSasSetKeyOff, 0x0890C0A8
	AddNID __sceSasCore, 0x0890C0B0
	AddNID __sceSasSetPitch, 0x0890C0B8
	AddNID __sceSasSetNoise, 0x0890C0C0
	AddNID __sceSasGetGrain, 0x0890C0C8
	AddNID __sceSasSetSimpleADSR, 0x0890C0D0
	AddNID __sceSasSetGrain, 0x0890C0D8
	AddNID __sceSasRevEVOL, 0x0890C0E0
	AddNID __sceSasGetOutputmode, 0x0890C0E8
	AddNID __sceSasSetOutputmode, 0x0890C0F0
	AddNID __sceSasRevVON, 0x0890C0F8

# sceAudio (9)9
	AddNID sceAudioOutputBlocking, 0x0890BBA8
	AddNID sceAudioOutputPannedBlocking, 0x0890BBB0
	AddNID sceAudioChReserve, 0x0890BBB8
	AddNID sceAudioChRelease, 0x0890BBC0
	AddNID sceAudioChangeChannelConfig, 0x0890BBC8
	AddNID sceAudioGetChannelRestLength, 0x0890BBD0
	AddNID sceAudioChangeChannelVolume, 0x0890BBD8
	AddNID sceAudioSetChannelDataLen, 0x0890BBE0
	AddNID sceAudioOutputPanned, 0x0890BBE8

# sceUmdUser (9)4
	AddNID sceUmdGetErrorStat, 0x0890C118
	AddNID sceUmdGetDriveStat, 0x0890C120
	AddNID sceUmdActivate, 0x0890C128
	AddNID sceUmdDeactivate, 0x0890C130
		
# sceMpeg (27)24
	AddNID sceMpegAvcDecode, 0x0890BDA8
	AddNID sceMpegRingbufferDestruct, 0x0890BDB0
	AddNID sceMpegInitAu, 0x0890BDB8
	AddNID sceMpegQueryStreamOffset, 0x0890BDC0
	AddNID sceMpegRingbufferConstruct, 0x0890BDC8
	AddNID sceMpegRegistStream, 0x0890BDD0
	AddNID sceMpegUnRegistStream, 0x0890BDD8
	AddNID sceMpegDelete, 0x0890BDE0
	AddNID sceMpegQueryStreamSize, 0x0890BDE8
	AddNID sceMpegInit, 0x0890BDF0
	AddNID sceMpegFlushAllStream, 0x0890BDF8
	AddNID sceMpegAvcDecodeStop, 0x0890BE00
	AddNID sceMpegAtracDecode, 0x0890BE08
	AddNID sceMpegFinish, 0x0890BE10
	AddNID sceMpegMallocAvcEsBuf, 0x0890BE18
	AddNID sceMpegRingbufferPut, 0x0890BE20
	AddNID sceMpegRingbufferAvailableSize, 0x0890BE28
	AddNID sceMpegQueryMemSize, 0x0890BE30
	AddNID sceMpegFreeAvcEsBuf, 0x0890BE38
	AddNID sceMpegRingbufferQueryMemSize, 0x0890BE40
	AddNID sceMpegCreate, 0x0890BE48
	AddNID sceMpegGetAtracAu, 0x0890BE50
	AddNID sceMpegQueryAtracEsSize, 0x0890BE58
	AddNID sceMpegGetAvcAu, 0x0890BE60

# sceRtc (2)1
	AddNID sceRtcGetCurrentClockLocalTime, 0x0890C028
	
# sceAtrac3plus (10)10
	AddNID sceAtracGetStreamDataInfo, 0x0890BB58
	AddNID sceAtracReleaseAtracID, 0x0890BB60
	AddNID sceAtracDecodeData, 0x0890BB68
	AddNID sceAtracSetDataAndGetID, 0x0890BB70
	AddNID sceAtracSetLoopNum, 0x0890BB78
	AddNID sceAtracGetRemainFrame, 0x0890BB80
	AddNID sceAtracGetSoundSample, 0x0890BB88
	AddNID sceAtracGetBitrate, 0x0890BB90
	AddNID sceAtracGetNextDecodePosition, 0x0890BB98
	AddNID sceAtracGetLoopStatus, 0x0890BBA0
	
# sceUtility (23)17
	AddNID sceUtilityLoadModule, 0x0890C138					# 0x2A2B3DE0
	AddNID sceUtilityMsgDialogInitStart, 0x0890C140			# 0x2AD8E239
	AddNID sceUtilityOskShutdownStart, 0x0890C148			# 0x3DFAEBA9
	AddNID sceUtilityOskUpdate, 0x0890C150					# 0x4B85C861
	AddNID sceUtilityNetconfInitStart, 0x0890C158			# 0x4DB1E739
	AddNID sceUtilitySavedataInitStart, 0x0890C160			# 0x50C4CD57
	AddNID sceUtilityNetconfGetStatus, 0x0890C168			# 0x6332AA39
	AddNID sceUtilityMsgDialogShutdownStart, 0x0890C170		# 0x67AF3428
	AddNID sceUtilitySavedataGetStatus, 0x0890C178			# 0x8874DBE0
	AddNID sceUtilityNetconfUpdate, 0x0890C180				# 0x91E70E35
	AddNID sceUtilityMsgDialogUpdate, 0x0890C188			# 0x95FC253B
	AddNID sceUtilitySavedataShutdownStart, 0x0890C190		# 0x9790B33C
	AddNID sceUtilityMsgDialogGetStatus, 0x0890C198			# 0x9A1C91D7
	AddNID sceUtilityGetSystemParamInt, 0x0890C1A0			# 0xA5DA2406
	AddNID sceUtilitySavedataUpdate, 0x0890C1A8				# 0xD4B95FFB
	AddNID sceUtilityUnloadModule, 0x0890C1B0		 		# 0xE49BFE92
	AddNID sceUtilityOskGetStatus, 0x0890C1B8				# 0xF3F76017
	AddNID sceUtilityOskInitStart, 0x0890C1C0				# 0xF6269B82
	AddNID sceUtilityNetconfShutdownStart, 0x0890C1C8		# 0xF88155F6
				
# sceNetAdhoc (15)15
	AddNID sceNetAdhocPtpClose, 0x0890BE68			# 0x157E6225
	AddNID sceNetAdhocPtpSend, 0x0890BE70			# 0x4DA4C788
	AddNID sceNetAdhocPdpCreate, 0x0890BE78			# 0x6F92741B
	AddNID sceNetAdhocPdpDelete, 0x0890BE80			# 0x7F27BB5E
	AddNID sceNetAdhocPtpOpen, 0x0890BE88			# 0x877F6D66
	AddNID sceNetAdhocPtpRecv, 0x0890BE90			# 0x8BEA2B3E
	AddNID sceNetAdhocPtpFlush, 0x0890BE98			# 0x9AC2EEAC
	AddNID sceNetAdhocPtpAccept, 0x0890BEA0			# 0x9DF81198
	AddNID sceNetAdhocTerm, 0x0890BEA8				# 0xA62C6F57
	AddNID sceNetAdhocPdpSend, 0x0890BEB0			# 0xABED3790
	AddNID sceNetAdhocGetPtpStat, 0x0890BEB8		# 0xB9685118
	AddNID sceNetAdhocPdpRecv, 0x0890BEC0			# 0xDFE53E03
	AddNID sceNetAdhocPtpListen, 0x0890BEC8			# 0xE08BDAC1
	AddNID sceNetAdhocInit, 0x0890BED0				# 0xE1D621D7
	AddNID sceNetAdhocPtpConnect, 0x0890BED8		# 0xFC6FC07B

# sceNet (6)4
	AddNID sceNetGetLocalEtherAddr, 0x0890BFF8	# 0x0BF0A3AE
	AddNID sceNetTerm, 0x0890C000				# 0x281928A9
	AddNID sceNetInit, 0x0890C008				# 0x39AF39A6
	AddNID sceNetFreeThreadinfo, 0x0890C010		# 0x50647530

# sceWlanDrv (2)2
	AddNID sceWlanGetEtherAddr, 0x0890C1D0 		# 0x0C622081
	AddNID sceWlanGetSwitchState, 0x0890C1D8 	# 0xD7763699
			
# sceNetAdhocctl (11)8
	AddNID sceNetAdhocctlScan, 0x0890BF08			# 0x08FFF7A0
	AddNID sceNetAdhocctlAddHandler, 0x0890BF10		# 0x20B317A0
	AddNID sceNetAdhocctlDisconnect, 0x0890BF18		# 0x34401D65
	AddNID sceNetAdhocctlDelHandler, 0x0890BF20		# 0x6402490B
	AddNID sceNetAdhocctlGetScanInfo, 0x0890BF28	# 0x81AEE1BE
	AddNID sceNetAdhocctlTerm, 0x0890BF30			# 0x9D689E13
	AddNID sceNetAdhocctlGetPeerList, 0x0890BF38	# 0xE162CB14
	AddNID sceNetAdhocctlInit, 0x0890BF40			# 0xE26F226E
	
# sceNetApctl()6
	AddNID sceNetApctlDisconnect, 0x0890BF48	#0x24FE91A1 
	AddNID sceNetApctlGetInfo, 0x0890BF50		#0x2BEFDF23 
	AddNID sceNetApctlDelHandler, 0x0890BF58	#0x5963991B 
	AddNID sceNetApctlAddHandler, 0x0890BF60	#0x8ABADD51 
	AddNID sceNetApctlTerm, 0x0890BF68			#0xB3EDD0EC 
	AddNID sceNetApctlInit, 0x0890BF70			#0xE2F91F9B 


# sceNetInet()16
	AddNID sceNetInetInit, 0x0890BF78			#0x17943399 
	AddNID sceNetInetBind, 0x0890BF80			#0x1A33F9AE 
	AddNID sceNetInetSetsockopt, 0x0890BF88		#0x2FE71FE7 
	AddNID sceNetInetConnect, 0x0890BF90		#0x410B34AA 
	AddNID sceNetInetSend, 0x0890BF98			#0x7AA671BC 
	AddNID sceNetInetCloseWithRST, 0x0890BFA0	#0x805502DD
	AddNID sceNetInetSocket, 0x0890BFA8			#0x8B7B220F 
	AddNID sceNetInetGetPspError, 0x0890BFB0	#0x8CA3A97E 
	AddNID sceNetInetClose, 0x0890BFB8			#0x8D7284EA 
	AddNID sceNetInetTerm, 0x0890BFC0			#0xA9ED66B9 
	AddNID sceNetInetGetTcpcbstat, 0x0890BFC8	#0xB3888AD4 
	AddNID sceNetInetInetAddr, 0x0890BFD0		#0xB75D5B0A 
	AddNID sceNetInetRecv, 0x0890BFD8			#0xCDA85C99 
	AddNID sceNetInetListen, 0x0890BFE0			#0xD10A1A7A 
	AddNID sceNetInetAccept, 0x0890BFE8			#0xDB094E1B 
	AddNID sceNetInetGetpeername, 0x0890BFF0	#0xE247B6D6 

# scePower (4)1
	AddNID scePowerRegisterCallback, 0x0890C020
	
# sceOpenPSID()1
	AddNID sceOpenPSIDGetOpenPSID, 0x0890C018	#0xC69BEBCE

# sceDmac()1
	AddNID sceDmacMemcpy, 0x0890BC18	#0x617F3FE6

# sceNetAdhocDiscover()5
	AddNID sceNetAdhocDiscoverUpdate, 0x0890BEE0			#0x52DE1B97 
	AddNID sceNetAdhocDiscoverInitStart, 0x0890BEE8			#0x941B3877 
	AddNID sceNetAdhocDiscoverGetStatus, 0x0890BEF0			#0x944DDBC6 
	AddNID sceNetAdhocDiscoverTerm, 0x0890BEF8				#0xA2246614 
	AddNID sceNetAdhocDiscoverRequestSuspend, 0x0890BF00	#0xA423A21B 


# sceHttp()28
	AddNID sceHttpGetContentLength, 0x0890BC80		#0x0282A3BD 
	AddNID sceHttpDisableCookie, 0x0890BC88			#0x0B12ABFB 
	AddNID sceHttpSetRecvTimeOut, 0x0890BC90		#0x1F0FC3E3 
	AddNID sceHttpGetNetworkPspError, 0x0890BC98	#0x2255551E 
	AddNID sceHttpAddExtraHeader, 0x0890BCA0		#0x3EABA285 
	AddNID sceHttpGetStatusCode, 0x0890BCA8			#0x4CC7D78F 
	AddNID sceHttpDeleteConnection, 0x0890BCB0		#0x5152773B 
	AddNID sceHttpsLoadDefaultCert, 0x0890BCB8		#0x87797BDD 
	AddNID sceHttpSetConnectTimeOut, 0x0890BCC0		#0x8ACD1F73 
	AddNID sceHttpSetSendTimeOut, 0x0890BCC8		#0x9988172D 
	AddNID sceHttpCreateTemplate, 0x0890BCD0		#0x9B1F1F36 
	AddNID sceHttpSetRedirectCallback, 0x0890BCD8	#0xA4496DE5 
	AddNID sceHttpDeleteRequest, 0x0890BCE0			#0xA5512E01 
	AddNID sceHttpsGetSslError, 0x0890BCE8			#0xAB1540D5 
	AddNID sceHttpInit, 0x0890BCF0					#0xAB1ABE07 
	AddNID sceHttpDisableAuth, 0x0890BCF8			#0xAE948FEE 
	AddNID sceHttpCreateRequestWithURL, 0x0890BD00	#0xB509B09E 
	AddNID sceHttpSendRequest, 0x0890BD08			#0xBB70706F 
	AddNID sceHttpAbortRequest, 0x0890BD10			#0xC10B6BD9 
	AddNID sceHttpDisableCache, 0x0890BD18			#0xCCBD167A 
	AddNID sceHttpCreateConnectionWithURL, 0x0890BD20	#0xCDF8ECB9 
	AddNID sceHttpEnd, 0x0890BD28					#0xD1C8945E 
	AddNID sceHttpGetAllHeader, 0x0890BD30			#0xDB266CCF 
	AddNID sceHttpsInit, 0x0890BD38					#0xE4D21302 
	AddNID sceHttpReadData, 0x0890BD40				#0xEDEEB999 
	AddNID sceHttpSetMallocFunction, 0x0890BD48		#0xF49934F6 
	AddNID sceHttpsEnd, 0x0890BD50					#0xF9D8EB63 
	AddNID sceHttpDeleteTemplate, 0x0890BD58		#0xFCF8C055 

# sceSsl()2
	AddNID sceSslEnd, 0x0890C1E0	#0x191CDEFF 
	AddNID sceSslInit, 0x0890C1E8	#0x957ECBE2 

# syslib Patapon exports
	AddNID module_start, 0x089094D4	
	AddNID module_stop, 0x08909878	
	AddNID module_start_thread_parameter, 0x089302A0
	AddNID module_stop_thread_parameter, 0x089302B0
	AddNID module_info, 0x0890C4A4
	AddNID syslib_11B97506, 0x089AE780

	.ident	"VAL-SDK"

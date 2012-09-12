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

# InterruptManager
	AddNID sceKernelRegisterSubIntrHandler, 0x088E7058		# 0xCA04A2B9
	AddNID sceKernelReleaseSubIntrHandler, 0x088E7060		# 0xD61E6961
	AddNID sceKernelEnableSubIntr, 0x088E7068		# 0xFB8E22EC

# IoFileMgrForUser
	AddNID sceIoWrite, 0x088E7070		# 0x42EC03AC
	AddNID sceIoDevctl, 0x088E7078		# 0x54F5FB11
	AddNID sceIoRead, 0x088E7080		# 0x6A638D83
	AddNID sceIoLseekAsync, 0x088E7088		# 0x71B19E77
	AddNID sceIoRename, 0x088E7090		# 0x779103A0
	AddNID sceIoClose, 0x088E7098		# 0x810C4BC3
	AddNID sceIoReadAsync, 0x088E70A0		# 0xA0B5A7C2
	AddNID sceIoGetstat, 0x088E70A8		# 0xACE946E8
	AddNID sceIoOpen, 0x088E70B0		# 0x109F50BC
	AddNID sceIoLseek, 0x088E70B8		# 0x27EB27B8
	AddNID sceIoPollAsync, 0x088E70C0		# 0x3251EA56

# Kernel_Library
	AddNID sceKernelCpuSuspendIntr, 0x088E70C8		# 0x092968F4
	AddNID sceKernelCpuResumeIntr, 0x088E70D0		# 0x5F10D406

# LoadExecForUser
	AddNID sceKernelExitGame, 0x088E70D8		# 0x05572A5F
	AddNID sceKernelRegisterExitCallback, 0x088E70E0		# 0x4AC57943

# ModuleMgrForUser
	AddNID sceKernelUnloadModule, 0x088E70E8		# 0x2E0911AA
	AddNID sceKernelGetModuleIdByAddress, 0x088E70F0		# 0xD8B73127
	AddNID sceKernelGetModuleId, 0x088E70F8		# 0xF0A26395
	AddNID sceKernelStartModule, 0x088E7100		# 0x50F0C1EC
	AddNID ModuleMgrForUser_8F2DF740, 0x088E7108		# 0x8F2DF740
	AddNID sceKernelLoadModuleByID, 0x088E7110		# 0xB7F46618
	AddNID sceKernelStopModule, 0x088E7118		# 0xD1FF982A

# StdioForUser
	AddNID sceKernelStdin, 0x088E7120		# 0x172D316E
	AddNID sceKernelStdout, 0x088E7128		# 0xA6BAB2E9
	AddNID sceKernelStderr, 0x088E7130		# 0xF78BA90A

# SysMemUserForUser
	AddNID sceKernelPrintf, 0x088E7138		# 0x13A5ABEF
	AddNID sceKernelAllocPartitionMemory, 0x088E7140		# 0x237DBD4F
	AddNID sceKernelSetCompiledSdkVersion, 0x088E7148		# 0x7591C7DB
	AddNID sceKernelGetBlockHeadAddr, 0x088E7150		# 0x9D9A5BA1
	AddNID sceKernelFreePartitionMemory, 0x088E7158		# 0xB6D61D02
	AddNID sceKernelSetCompilerVersion, 0x088E7160		# 0xF77D77CB

# ThreadManForUser
	AddNID sceKernelStartVTimer, 0x088E7168		# 0xC68D9437
	AddNID sceKernelDelayThread, 0x088E7170		# 0xCEADEB47
	AddNID sceKernelWakeupThread, 0x088E7178		# 0xD59EAD2F
	AddNID sceKernelSetEventFlag, 0x088E7180		# 0x1FB15A32
	AddNID sceKernelCreateSema, 0x088E7188		# 0xD6DA4BA1
	AddNID sceKernelSysClock2USecWide, 0x088E7190		# 0xE1619D7C
	AddNID sceKernelCreateCallback, 0x088E7198		# 0xE81CAF8F
	AddNID sceKernelCreateVTimer, 0x088E71A0		# 0x20FFF560
	AddNID sceKernelChangeCurrentThreadAttr, 0x088E71A8		# 0xEA748E31
	AddNID sceKernelDeleteEventFlag, 0x088E71B0		# 0xEF9E4C70
	AddNID sceKernelStartThread, 0x088E71B8		# 0xF475845D
	AddNID sceKernelWaitThreadEnd, 0x088E71C0		# 0x278C0DF5
	AddNID sceKernelResumeDispatchThread, 0x088E71C8		# 0x27E22EC2
	AddNID sceKernelDeleteSema, 0x088E71D0		# 0x28B6489C
	AddNID sceKernelGetThreadId, 0x088E71D8		# 0x293B45B8
	AddNID sceKernelPollEventFlag, 0x088E71E0		# 0x30FD48F0
	AddNID sceKernelGetSystemTimeLow, 0x088E71E8		# 0x369ED59D
	AddNID sceKernelTerminateDeleteThread, 0x088E71F0		# 0x383F7BCC
	AddNID sceKernelSuspendDispatchThread, 0x088E71F8		# 0x3AD58B8C
	AddNID sceKernelGetThreadExitStatus, 0x088E7200		# 0x3B183E26
	AddNID sceKernelSignalSema, 0x088E7208		# 0x3F53E640
	AddNID sceKernelWaitEventFlag, 0x088E7210		# 0x402FCF22
	AddNID sceKernelCreateThread, 0x088E7218		# 0x446D8DE6
	AddNID sceKernelWaitSema, 0x088E7220		# 0x4E3A1105
	AddNID sceKernelGetThreadStackFreeSize, 0x088E7228		# 0x52089CA1
	AddNID sceKernelSetVTimerHandlerWide, 0x088E7230		# 0x53B00E9A
	AddNID sceKernelCreateEventFlag, 0x088E7238		# 0x55C20A00
	AddNID sceKernelPollSema, 0x088E7240		# 0x58B1F937
	AddNID sceKernelDelayThreadCB, 0x088E7248		# 0x68DA9E36
	AddNID sceKernelChangeThreadPriority, 0x088E7250		# 0x71BC9871
	AddNID sceKernelExitDeleteThread, 0x088E7258		# 0x809CE29B
	AddNID sceKernelClearEventFlag, 0x088E7260		# 0x812346E4
	AddNID sceKernelSleepThreadCB, 0x088E7268		# 0x82826F70
	AddNID sceKernelGetSystemTimeWide, 0x088E7270		# 0x82BC5777
	AddNID sceKernelGetThreadCurrentPriority, 0x088E7278		# 0x94AA61EE
	AddNID sceKernelSleepThread, 0x088E7280		# 0x9ACE131E
	AddNID sceKernelDeleteThread, 0x088E7288		# 0x9FA03CD3
	AddNID sceKernelExitThread, 0x088E7290		# 0xAA73C935

# UtilsForUser
	AddNID sceKernelLibcGettimeofday, 0x088E7298		# 0x71EC4271
	AddNID sceKernelDcacheWritebackAll, 0x088E72A0		# 0x79D1C3FA
	AddNID sceKernelLibcClock, 0x088E72A8		# 0x91E4F6A7
	AddNID sceKernelIcacheInvalidateAll, 0x088E72B0		# 0x920F104A
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x088E72B8		# 0xB435DEC5
	AddNID sceKernelDcacheInvalidateRange, 0x088E72C0		# 0xBFA98062
	AddNID sceKernelLibcTime, 0x088E72C8		# 0x27CC57F0
	AddNID sceKernelDcacheWritebackRange, 0x088E72D0		# 0x3EE30821

# sceSuspendForUser
	AddNID sceKernelPowerTick, 0x088E7848		# 0x090CCB3F
	AddNID sceKernelVolatileMemLock, 0x088E7850		# 0x3E0271D3
	AddNID sceKernelVolatileMemUnlock, 0x088E7858		# 0xA569E425

# sceGe_user
	AddNID sceGeListSync, 0x088E73A0		# 0x03444EB4
	AddNID sceGeUnsetCallback, 0x088E73A8		# 0x05DB22CE
	AddNID sceGeListEnQueueHead, 0x088E73B0		# 0x1C0D95A6
	AddNID sceGeEdramGetSize, 0x088E73B8		# 0x1F6752AD
	AddNID sceGeContinue, 0x088E73C0		# 0x4C06E472
	AddNID sceGeSetCallback, 0x088E73C8		# 0xA4FC06A4
	AddNID sceGeListEnQueue, 0x088E73D0		# 0xAB49E76A
	AddNID sceGeDrawSync, 0x088E73D8		# 0xB287BD61
	AddNID sceGeBreak, 0x088E73E0		# 0xB448EC0D
	AddNID sceGeEdramSetAddrTranslation, 0x088E73E8		# 0xB77905EA
	AddNID sceGeListUpdateStallAddr, 0x088E73F0		# 0xE0D68148
	AddNID sceGeEdramGetAddr, 0x088E73F8		# 0xE47E40E4

# sceDisplay
	AddNID sceDisplaySetMode, 0x088E7388		# 0x0E20F177
	AddNID sceDisplaySetFrameBuf, 0x088E7390		# 0x289D82FE

# sceCtrl
	AddNID sceCtrlSetSamplingMode, 0x088E7370		# 0x1F4011E6
	AddNID sceCtrlReadBufferPositive, 0x088E7378		# 0x1F803938
	AddNID sceCtrlSetSamplingCycle, 0x088E7380		# 0x6A2774F3

# sceLibFont
	AddNID sceFontFindOptimumFont, 0x088E74E0		# 0x099EF33C
	AddNID sceFontGetFontInfo, 0x088E74E8			# 0x0DA7535E
	AddNID sceFontGetNumFontList, 0x088E74F0		# 0x27F6E642
	AddNID sceFontClose, 0x088E74F8					# 0x3AEA8CB6
	AddNID sceFontDoneLib, 0x088E7500				# 0x574B6FBC
	AddNID sceFontNewLib, 0x088E7508				# 0x67F17ED7
	AddNID sceFontGetCharGlyphImage, 0x088E7510		# 0x980F4895
	AddNID sceFontOpen, 0x088E7518					# 0xA834319D
	AddNID sceFontGetCharInfo, 0x088E7520			# 0xDCC80C2F

# sceSasCore
	AddNID __sceSasSetADSR, 0x088E7780		# 0x019B25EB
	AddNID __sceSasRevParam, 0x088E7788		# 0x267A6DD2
	AddNID __sceSasGetPauseFlag, 0x088E7790		# 0x2C8E6AB3
	AddNID __sceSasRevType, 0x088E7798		# 0x33D4AB37
	AddNID __sceSasInit, 0x088E77A0		# 0x42778A9F
	AddNID __sceSasSetVolume, 0x088E77A8		# 0x440CA7D8
	AddNID __sceSasCoreWithMix, 0x088E77B0		# 0x50A14DFC
	AddNID __sceSasSetSL, 0x088E77B8		# 0x5F9529F6
	AddNID __sceSasGetEndFlag, 0x088E77C0		# 0x68A46B95
	AddNID __sceSasGetEnvelopeHeight, 0x088E77C8		# 0x74AE582A
	AddNID __sceSasSetKeyOn, 0x088E77D0		# 0x76F01ACA
	AddNID __sceSasSetPause, 0x088E77D8		# 0x787D04D5
	AddNID __sceSasSetVoice, 0x088E77E0		# 0x99944089
	AddNID __sceSasSetADSRmode, 0x088E77E8		# 0x9EC3676A
	AddNID __sceSasSetKeyOff, 0x088E77F0		# 0xA0CF2FA4
	AddNID __sceSasCore, 0x088E77F8		# 0xA3589D81
	AddNID __sceSasSetPitch, 0x088E7800		# 0xAD84D37F
	AddNID __sceSasSetNoise, 0x088E7808		# 0xB7660A23
	AddNID __sceSasGetGrain, 0x088E7810		# 0xBD11B7C2
	AddNID __sceSasSetSimpleADSR, 0x088E7818		# 0xCBCD4F79
	AddNID __sceSasSetGrain, 0x088E7820		# 0xD1E0A01E
	AddNID __sceSasRevEVOL, 0x088E7828		# 0xD5A229C9
	AddNID __sceSasGetOutputmode, 0x088E7830		# 0xE175EF66
	AddNID __sceSasSetOutputmode, 0x088E7838		# 0xE855BF76
	AddNID __sceSasRevVON, 0x088E7840		# 0xF983B186

# sceAudio
	AddNID sceAudioOutputBlocking, 0x088E7328		# 0x136CAF51
	AddNID sceAudioOutputPannedBlocking, 0x088E7330		# 0x13F592BC
	AddNID sceAudioChReserve, 0x088E7338		# 0x5EC81C55
	AddNID sceAudioChRelease, 0x088E7340		# 0x6FC46853
	AddNID sceAudioChangeChannelConfig, 0x088E7348		# 0x95FD0C2D
	AddNID sceAudioGetChannelRestLength, 0x088E7350		# 0xB011922F
	AddNID sceAudioChangeChannelVolume, 0x088E7358		# 0xB7E1D8E7
	AddNID sceAudioSetChannelDataLen, 0x088E7360		# 0xCB2E439E
	AddNID sceAudioOutputPanned, 0x088E7368		# 0xE2D56B2D

# sceUmdUser
	AddNID sceUmdGetErrorStat, 0x088E7860		# 0x20628E6F
	AddNID sceUmdGetDriveStat, 0x088E7868		# 0x6B4A146C
	AddNID sceUmdActivate, 0x088E7870		# 0xC6183D47
	AddNID sceUmdDeactivate, 0x088E7878		# 0xE83742BA

# sceMpeg
	AddNID sceMpegAvcDecode, 0x088E7528		# 0x0E3C2E9D
	AddNID sceMpegRingbufferDestruct, 0x088E7530		# 0x13407F13
	AddNID sceMpegInitAu, 0x088E7538		# 0x167AFD9E
	AddNID sceMpegQueryStreamOffset, 0x088E7540		# 0x21FF80E4
	AddNID sceMpegRingbufferConstruct, 0x088E7548		# 0x37295ED8
	AddNID sceMpegRegistStream, 0x088E7550		# 0x42560F23
	AddNID sceMpegUnRegistStream, 0x088E7558		# 0x591A4AA2
	AddNID sceMpegDelete, 0x088E7560		# 0x606A4649
	AddNID sceMpegQueryStreamSize, 0x088E7568		# 0x611E9E11
	AddNID sceMpegInit, 0x088E7570		# 0x682A619B
	AddNID sceMpegFlushAllStream, 0x088E7578		# 0x707B7629
	AddNID sceMpegAvcDecodeStop, 0x088E7580		# 0x740FCCD1
	AddNID sceMpegAtracDecode, 0x088E7588		# 0x800C44DF
	AddNID sceMpegFinish, 0x088E7590		# 0x874624D6
	AddNID sceMpegMallocAvcEsBuf, 0x088E7598		# 0xA780CF7E
	AddNID sceMpegRingbufferPut, 0x088E75A0		# 0xB240A59E
	AddNID sceMpegRingbufferAvailableSize, 0x088E75A8		# 0xB5F6DC87
	AddNID sceMpegQueryMemSize, 0x088E75B0		# 0xC132E22F
	AddNID sceMpegFreeAvcEsBuf, 0x088E75B8		# 0xCEB870B1
	AddNID sceMpegRingbufferQueryMemSize, 0x088E75C0		# 0xD7A29F46
	AddNID sceMpegCreate, 0x088E75C8		# 0xD8C5F121
	AddNID sceMpegGetAtracAu, 0x088E75D0		# 0xE1CE83A7
	AddNID sceMpegQueryAtracEsSize, 0x088E75D8		# 0xF8DCB679
	AddNID sceMpegGetAvcAu, 0x088E75E0		# 0xFE246728

# sceRtc
	AddNID sceRtcGetCurrentClockLocalTime, 0x088E7778		# 0xE7C27D1B

# sceAtrac3plus
	AddNID sceAtracGetStreamDataInfo, 0x088E72D8		# 0x5D268707
	AddNID sceAtracReleaseAtracID, 0x088E72E0		# 0x61EB33F5
	AddNID sceAtracDecodeData, 0x088E72E8		# 0x6A8C3CD5
	AddNID sceAtracSetDataAndGetID, 0x088E72F0		# 0x7A20E7AF
	AddNID sceAtracSetLoopNum, 0x088E72F8		# 0x868120B5
	AddNID sceAtracGetRemainFrame, 0x088E7300		# 0x9AE849A7
	AddNID sceAtracGetSoundSample, 0x088E7308		# 0xA2BBA8BE
	AddNID sceAtracGetBitrate, 0x088E7310		# 0xA554A158
	AddNID sceAtracGetNextDecodePosition, 0x088E7318		# 0xE23E3A35
	AddNID sceAtracGetLoopStatus, 0x088E7320		# 0xFAA4F89B

# sceUtility
	AddNID sceUtilityLoadModule, 0x088E7880		# 0x2A2B3DE0
	AddNID sceUtilityMsgDialogInitStart, 0x088E7888		# 0x2AD8E239
	AddNID sceUtilityOskShutdownStart, 0x088E7890		# 0x3DFAEBA9
	AddNID sceUtilityOskUpdate, 0x088E7898		# 0x4B85C861
	AddNID sceUtilityNetconfInitStart, 0x088E78A0		# 0x4DB1E739
	AddNID sceUtilitySavedataInitStart, 0x088E78A8		# 0x50C4CD57
	AddNID sceUtilityNetconfGetStatus, 0x088E78B0		# 0x6332AA39
	AddNID sceUtilityMsgDialogShutdownStart, 0x088E78B8		# 0x67AF3428
	AddNID sceUtilitySavedataGetStatus, 0x088E78C0		# 0x8874DBE0
	AddNID sceUtilityNetconfUpdate, 0x088E78C8		# 0x91E70E35
	AddNID sceUtilityMsgDialogUpdate, 0x088E78D0		# 0x95FC253B
	AddNID sceUtilitySavedataShutdownStart, 0x088E78D8		# 0x9790B33C
	AddNID sceUtilityMsgDialogGetStatus, 0x088E78E0		# 0x9A1C91D7
	AddNID sceUtilityGetSystemParamInt, 0x088E78E8		# 0xA5DA2406
	AddNID sceUtilitySavedataUpdate, 0x088E78F0		# 0xD4B95FFB
	AddNID sceUtilityUnloadModule, 0x088E78F8		# 0xE49BFE92
	AddNID sceUtilityOskGetStatus, 0x088E7900		# 0xF3F76017
	AddNID sceUtilityOskInitStart, 0x088E7908		# 0xF6269B82
	AddNID sceUtilityNetconfShutdownStart, 0x088E7910		# 0xF88155F6

# sceNetAdhoc
	AddNID sceNetAdhocPtpClose, 0x088E75E8		# 0x157E6225
	AddNID sceNetAdhocPtpSend, 0x088E75F0		# 0x4DA4C788
	AddNID sceNetAdhocPdpCreate, 0x088E75F8		# 0x6F92741B
	AddNID sceNetAdhocPdpDelete, 0x088E7600		# 0x7F27BB5E
	AddNID sceNetAdhocPtpOpen, 0x088E7608		# 0x877F6D66
	AddNID sceNetAdhocPtpRecv, 0x088E7610		# 0x8BEA2B3E
	AddNID sceNetAdhocPtpAccept, 0x088E7618		# 0x9DF81198
	AddNID sceNetAdhocTerm, 0x088E7620		# 0xA62C6F57
	AddNID sceNetAdhocPdpSend, 0x088E7628		# 0xABED3790
	AddNID sceNetAdhocGetPtpStat, 0x088E7630		# 0xB9685118
	AddNID sceNetAdhocPdpRecv, 0x088E7638		# 0xDFE53E03
	AddNID sceNetAdhocPtpListen, 0x088E7640		# 0xE08BDAC1
	AddNID sceNetAdhocInit, 0x088E7648		# 0xE1D621D7
	AddNID sceNetAdhocPtpConnect, 0x088E7650		# 0xFC6FC07B

# sceNet
	AddNID sceNetGetLocalEtherAddr, 0x088E7748		# 0x0BF0A3AE
	AddNID sceNetTerm, 0x088E7750		# 0x281928A9
	AddNID sceNetInit, 0x088E7758		# 0x39AF39A6
	AddNID sceNetFreeThreadinfo, 0x088E7760		# 0x50647530

# sceWlanDrv
	AddNID sceWlanGetEtherAddr, 0x088E7918		# 0x0C622081
	AddNID sceWlanGetSwitchState, 0x088E7920		# 0xD7763699

# sceNetAdhocctl
	AddNID sceNetAdhocctlScan, 0x088E7658		# 0x08FFF7A0
	AddNID sceNetAdhocctlAddHandler, 0x088E7660		# 0x20B317A0
	AddNID sceNetAdhocctlDisconnect, 0x088E7668		# 0x34401D65
	AddNID sceNetAdhocctlDelHandler, 0x088E7670		# 0x6402490B
	AddNID sceNetAdhocctlGetScanInfo, 0x088E7678		# 0x81AEE1BE
	AddNID sceNetAdhocctlTerm, 0x088E7680		# 0x9D689E13
	AddNID sceNetAdhocctlGetPeerList, 0x088E7688		# 0xE162CB14
	AddNID sceNetAdhocctlInit, 0x088E7690		# 0xE26F226E

# sceNetApctl
	AddNID sceNetApctlDisconnect, 0x088E7698		# 0x24FE91A1
	AddNID sceNetApctlGetInfo, 0x088E76A0		# 0x2BEFDF23
	AddNID sceNetApctlDelHandler, 0x088E76A8		# 0x5963991B
	AddNID sceNetApctlAddHandler, 0x088E76B0		# 0x8ABADD51
	AddNID sceNetApctlTerm, 0x088E76B8		# 0xB3EDD0EC
	AddNID sceNetApctlInit, 0x088E76C0		# 0xE2F91F9B

# sceNetInet
	AddNID sceNetInetInit, 0x088E76C8		# 0x17943399
	AddNID sceNetInetBind, 0x088E76D0		# 0x1A33F9AE
	AddNID sceNetInetSetsockopt, 0x088E76D8		# 0x2FE71FE7
	AddNID sceNetInetConnect, 0x088E76E0		# 0x410B34AA
	AddNID sceNetInetSend, 0x088E76E8		# 0x7AA671BC
	AddNID sceNetInetCloseWithRST, 0x088E76F0		# 0x805502DD
	AddNID sceNetInetSocket, 0x088E76F8		# 0x8B7B220F
	AddNID sceNetInetGetPspError, 0x088E7700		# 0x8CA3A97E
	AddNID sceNetInetClose, 0x088E7708		# 0x8D7284EA
	AddNID sceNetInetTerm, 0x088E7710		# 0xA9ED66B9
	AddNID sceNetInetGetTcpcbstat, 0x088E7718		# 0xB3888AD4
	AddNID sceNetInetInetAddr, 0x088E7720		# 0xB75D5B0A
	AddNID sceNetInetRecv, 0x088E7728		# 0xCDA85C99
	AddNID sceNetInetListen, 0x088E7730		# 0xD10A1A7A
	AddNID sceNetInetAccept, 0x088E7738		# 0xDB094E1B
	AddNID sceNetInetGetpeername, 0x088E7740		# 0xE247B6D6

# scePower
	AddNID scePowerRegisterCallback, 0x088E7770		# 0x04B7766E

# sceOpenPSID
	AddNID sceOpenPSIDGetOpenPSID, 0x088E7768		# 0xC69BEBCE

# sceImpose
	AddNID sceImposeSetLanguageMode, 0x088E7928		# 0x36AA6E91

# sceDmac
	AddNID sceDmacMemcpy, 0x088E7398		# 0x617F3FE6

# sceHttp
	AddNID sceHttpGetContentLength, 0x088E7400		# 0x0282A3BD
	AddNID sceHttpDisableCookie, 0x088E7408		# 0x0B12ABFB
	AddNID sceHttpSetRecvTimeOut, 0x088E7410		# 0x1F0FC3E3
	AddNID sceHttpGetNetworkPspError, 0x088E7418		# 0x2255551E
	AddNID sceHttpAddExtraHeader, 0x088E7420		# 0x3EABA285
	AddNID sceHttpGetStatusCode, 0x088E7428		# 0x4CC7D78F
	AddNID sceHttpDeleteConnection, 0x088E7430		# 0x5152773B
	AddNID sceHttpsLoadDefaultCert, 0x088E7438		# 0x87797BDD
	AddNID sceHttpSetConnectTimeOut, 0x088E7440		# 0x8ACD1F73
	AddNID sceHttpSetSendTimeOut, 0x088E7448		# 0x9988172D
	AddNID sceHttpCreateTemplate, 0x088E7450		# 0x9B1F1F36
	AddNID sceHttpSetRedirectCallback, 0x088E7458		# 0xA4496DE5
	AddNID sceHttpDeleteRequest, 0x088E7460		# 0xA5512E01
	AddNID sceHttpsGetSslError, 0x088E7468		# 0xAB1540D5
	AddNID sceHttpInit, 0x088E7470		# 0xAB1ABE07
	AddNID sceHttpDisableAuth, 0x088E7478		# 0xAE948FEE
	AddNID sceHttpCreateRequestWithURL, 0x088E7480		# 0xB509B09E
	AddNID sceHttpSendRequest, 0x088E7488		# 0xBB70706F
	AddNID sceHttpAbortRequest, 0x088E7490		# 0xC10B6BD9
	AddNID sceHttpDisableCache, 0x088E7498		# 0xCCBD167A
	AddNID sceHttpCreateConnectionWithURL, 0x088E74A0		# 0xCDF8ECB9
	AddNID sceHttpEnd, 0x088E74A8		# 0xD1C8945E
	AddNID sceHttpGetAllHeader, 0x088E74B0		# 0xDB266CCF
	AddNID sceHttpsInit, 0x088E74B8		# 0xE4D21302
	AddNID sceHttpReadData, 0x088E74C0		# 0xEDEEB999
	AddNID sceHttpSetMallocFunction, 0x088E74C8		# 0xF49934F6
	AddNID sceHttpsEnd, 0x088E74D0		# 0xF9D8EB63
	AddNID sceHttpDeleteTemplate, 0x088E74D8		# 0xFCF8C055

# sceSsl
	AddNID sceSslEnd, 0x088E7930		# 0x191CDEFF
	AddNID sceSslInit, 0x088E7938		# 0x957ECBE2
	AddNID sceSslGetUsedMemoryMax, 0x088E7940		# 0xB99EDE6A

# syslib exports

	AddNID module_start, 0x088AB8A0		# 0xD632ACDB
	AddNID module_stop, 0x088ABC04		# 0xCEE8593C
	AddNID module_start_thread_parameter, 0x0890B0CC		# 0x0F7C276C
	AddNID module_stop_thread_parameter, 0x0890B0DC		# 0xCF0CC697
	AddNID module_info, 0x088E7BFC		# 0xF01D73A7
	AddNID syslib_11B97506, 0x08945120		# 0x11B97506

	.ident	"VAL-SDK"

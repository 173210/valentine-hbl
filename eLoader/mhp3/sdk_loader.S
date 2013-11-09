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


	AddNID module_start, 0x088214C8		# 0xD632ACDB
	AddNID module_stop, 0x088218A4		# 0xCEE8593C
	AddNID module_start_thread_parameter, 0x08964B44		# 0x0F7C276C
	AddNID module_stop_thread_parameter, 0x08964B54		# 0xCF0CC697
	AddNID module_info, 0x08961620		# 0xF01D73A7
	AddNID syslib_11B97506, 0x08966CA0		# 0x11B97506

# InterruptManager
	AddNID sceKernelRegisterSubIntrHandler, 0x089609E8		# 0xCA04A2B9
	AddNID sceKernelReleaseSubIntrHandler, 0x089609F0		# 0xD61E6961
	AddNID sceKernelEnableSubIntr, 0x089609F8		# 0xFB8E22EC

# IoFileMgrForUser
	AddNID sceIoWrite, 0x08960A00		# 0x42EC03AC
	AddNID sceIoDevctl, 0x08960A08		# 0x54F5FB11
	AddNID sceIoRead, 0x08960A10		# 0x6A638D83
	AddNID sceIoRename, 0x08960A18		# 0x779103A0
	AddNID sceIoClose, 0x08960A20		# 0x810C4BC3
	AddNID sceIoGetstat, 0x08960A28		# 0xACE946E8
	AddNID sceIoDopen, 0x08960A30		# 0xB29DDF9C
	AddNID sceIoDclose, 0x08960A38		# 0xEB092469
	AddNID sceIoOpen, 0x08960A40		# 0x109F50BC
	AddNID sceIoLseek, 0x08960A48		# 0x27EB27B8

# Kernel_Library
	AddNID sceKernelCpuSuspendIntr, 0x08960A50		# 0x092968F4
	AddNID Kernel_Library_A089ECA4, 0x08960A58		# 0xA089ECA4
	AddNID Kernel_Library_293B45B8, 0x08960A60		# 0x293B45B8
	AddNID sceKernelCpuResumeIntr, 0x08960A68		# 0x5F10D406

# LoadExecForUser
	AddNID sceKernelExitGame, 0x08960A70		# 0x05572A5F
	AddNID sceKernelRegisterExitCallback, 0x08960A78		# 0x4AC57943

# ModuleMgrForUser
	AddNID sceKernelUnloadModule, 0x08960A80		# 0x2E0911AA
	AddNID sceKernelStopModule, 0x08960A88		# 0xD1FF982A
	AddNID sceKernelGetModuleIdByAddress, 0x08960A90		# 0xD8B73127
	AddNID sceKernelGetModuleId, 0x08960A98		# 0xF0A26395
	AddNID sceKernelStartModule, 0x08960AA0		# 0x50F0C1EC
	AddNID ModuleMgrForUser_8F2DF740, 0x08960AA8		# 0x8F2DF740
	AddNID sceKernelLoadModuleByID, 0x08960AB0		# 0xB7F46618

# StdioForUser
	AddNID sceKernelStdin, 0x08960AB8		# 0x172D316E
	AddNID sceKernelStdout, 0x08960AC0		# 0xA6BAB2E9
	AddNID sceKernelStderr, 0x08960AC8		# 0xF78BA90A

# SysMemUserForUser
	AddNID sceKernelPrintf, 0x08960AD0		# 0x13A5ABEF
	AddNID SysMemUserForUser_DB83A952, 0x08960AD8		# 0xDB83A952
	AddNID sceKernelSetCompilerVersion, 0x08960AE0		# 0xF77D77CB
	AddNID SysMemUserForUser_FE707FDF, 0x08960AE8		# 0xFE707FDF
	AddNID SysMemUserForUser_1B4217BC, 0x08960AF0		# 0x1B4217BC
	AddNID sceKernelAllocPartitionMemory, 0x08960AF8		# 0x237DBD4F
	AddNID SysMemUserForUser_50F61D8A, 0x08960B00		# 0x50F61D8A
	AddNID sceKernelGetBlockHeadAddr, 0x08960B08		# 0x9D9A5BA1
	AddNID sceKernelFreePartitionMemory, 0x08960B10		# 0xB6D61D02

# ThreadManForUser
	AddNID sceKernelStartVTimer, 0x08960B18		# 0xC68D9437
	AddNID sceKernelDelayThread, 0x08960B20		# 0xCEADEB47
	AddNID sceKernelWakeupThread, 0x08960B28		# 0xD59EAD2F
	AddNID sceKernelCreateSema, 0x08960B30		# 0xD6DA4BA1
	AddNID sceKernelGetSystemTime, 0x08960B38		# 0xDB738F35
	AddNID sceKernelSetEventFlag, 0x08960B40		# 0x1FB15A32
	AddNID sceKernelSysClock2USecWide, 0x08960B48		# 0xE1619D7C
	AddNID sceKernelCreateCallback, 0x08960B50		# 0xE81CAF8F
	AddNID sceKernelChangeCurrentThreadAttr, 0x08960B58		# 0xEA748E31
	AddNID sceKernelDeleteCallback, 0x08960B60		# 0xEDBA5844
	AddNID sceKernelDeleteEventFlag, 0x08960B68		# 0xEF9E4C70
	AddNID sceKernelCreateVTimer, 0x08960B70		# 0x20FFF560
	AddNID sceKernelStartThread, 0x08960B78		# 0xF475845D
	AddNID sceKernelDeleteMutex, 0x08960B80		# 0xF8170FBE
	AddNID sceKernelResumeDispatchThread, 0x08960B88		# 0x27E22EC2
	AddNID sceKernelDeleteSema, 0x08960B90		# 0x28B6489C
	AddNID sceKernelPollEventFlag, 0x08960B98		# 0x30FD48F0
	AddNID sceKernelGetSystemTimeLow, 0x08960BA0		# 0x369ED59D
	AddNID sceKernelTerminateDeleteThread, 0x08960BA8		# 0x383F7BCC
	AddNID sceKernelSuspendDispatchThread, 0x08960BB0		# 0x3AD58B8C
	AddNID sceKernelGetThreadExitStatus, 0x08960BB8		# 0x3B183E26
	AddNID sceKernelSignalSema, 0x08960BC0		# 0x3F53E640
	AddNID sceKernelWaitEventFlag, 0x08960BC8		# 0x402FCF22
	AddNID sceKernelCreateThread, 0x08960BD0		# 0x446D8DE6
	AddNID sceKernelWaitSema, 0x08960BD8		# 0x4E3A1105
	AddNID sceKernelSetVTimerHandlerWide, 0x08960BE0		# 0x53B00E9A
	AddNID sceKernelCreateEventFlag, 0x08960BE8		# 0x55C20A00
	AddNID sceKernelPollSema, 0x08960BF0		# 0x58B1F937
	AddNID sceKernelTerminateThread, 0x08960BF8		# 0x616403BA
	AddNID sceKernelDelayThreadCB, 0x08960C00		# 0x68DA9E36
	AddNID sceKernelUnlockMutex, 0x08960C08		# 0x6B30100F
	AddNID sceKernelChangeThreadPriority, 0x08960C10		# 0x71BC9871
	AddNID sceKernelExitDeleteThread, 0x08960C18		# 0x809CE29B
	AddNID sceKernelClearEventFlag, 0x08960C20		# 0x812346E4
	AddNID sceKernelSleepThreadCB, 0x08960C28		# 0x82826F70
	AddNID sceKernelGetSystemTimeWide, 0x08960C30		# 0x82BC5777
	AddNID sceKernelGetThreadCurrentPriority, 0x08960C38		# 0x94AA61EE
	AddNID sceKernelSleepThread, 0x08960C40		# 0x9ACE131E
	AddNID sceKernelDeleteThread, 0x08960C48		# 0x9FA03CD3
	AddNID sceKernelExitThread, 0x08960C50		# 0xAA73C935
	AddNID sceKernelLockMutex, 0x08960C58		# 0xB011B11F
	AddNID sceKernelCreateMutex, 0x08960C60		# 0xB7D098C6

# UtilsForUser
	AddNID sceKernelLibcGettimeofday, 0x08960C68		# 0x71EC4271
	AddNID sceKernelDcacheWritebackAll, 0x08960C70		# 0x79D1C3FA
	AddNID sceKernelLibcClock, 0x08960C78		# 0x91E4F6A7
	AddNID sceKernelIcacheInvalidateAll, 0x08960C80		# 0x920F104A
	AddNID sceKernelDcacheWritebackInvalidateAll, 0x08960C88		# 0xB435DEC5
	AddNID sceKernelDcacheInvalidateRange, 0x08960C90		# 0xBFA98062
	AddNID sceKernelLibcTime, 0x08960C98		# 0x27CC57F0
	AddNID sceKernelGetGPI, 0x08960CA0		# 0x37FB5C42
	AddNID sceKernelDcacheWritebackRange, 0x08960CA8		# 0x3EE30821

# sceSuspendForUser
	AddNID sceKernelPowerTick, 0x08960CB0		# 0x090CCB3F
	AddNID sceKernelVolatileMemLock, 0x08960CB8		# 0x3E0271D3
	AddNID sceKernelVolatileMemUnlock, 0x08960CC0		# 0xA569E425

# sceGe_user
	AddNID sceGeListSync, 0x08960CC8		# 0x03444EB4
	AddNID sceGeUnsetCallback, 0x08960CD0		# 0x05DB22CE
	AddNID sceGeListEnQueueHead, 0x08960CD8		# 0x1C0D95A6
	AddNID sceGeEdramGetSize, 0x08960CE0		# 0x1F6752AD
	AddNID sceGeContinue, 0x08960CE8		# 0x4C06E472
	AddNID sceGeSetCallback, 0x08960CF0		# 0xA4FC06A4
	AddNID sceGeListEnQueue, 0x08960CF8		# 0xAB49E76A
	AddNID sceGeDrawSync, 0x08960D00		# 0xB287BD61
	AddNID sceGeBreak, 0x08960D08		# 0xB448EC0D
	AddNID sceGeEdramSetAddrTranslation, 0x08960D10		# 0xB77905EA
	AddNID sceGeListUpdateStallAddr, 0x08960D18		# 0xE0D68148
	AddNID sceGeEdramGetAddr, 0x08960D20		# 0xE47E40E4

# sceDisplay
	AddNID sceDisplaySetMode, 0x08960D28		# 0x0E20F177
	AddNID sceDisplaySetFrameBuf, 0x08960D30		# 0x289D82FE

# sceCtrl
	AddNID sceCtrlSetSamplingMode, 0x08960D38		# 0x1F4011E6
	AddNID sceCtrlReadBufferPositive, 0x08960D40		# 0x1F803938
	AddNID sceCtrlSetSamplingCycle, 0x08960D48		# 0x6A2774F3

# sceLibFont
	AddNID sceLibFont_099EF33C, 0x08960D50		# 0x099EF33C
	AddNID sceLibFont_0DA7535E, 0x08960D58		# 0x0DA7535E
	AddNID sceLibFont_27F6E642, 0x08960D60		# 0x27F6E642
	AddNID sceLibFont_3AEA8CB6, 0x08960D68		# 0x3AEA8CB6
	AddNID sceLibFont_574B6FBC, 0x08960D70		# 0x574B6FBC
	AddNID sceLibFont_67F17ED7, 0x08960D78		# 0x67F17ED7
	AddNID sceLibFont_980F4895, 0x08960D80		# 0x980F4895
	AddNID sceLibFont_A834319D, 0x08960D88		# 0xA834319D
	AddNID sceLibFont_DCC80C2F, 0x08960D90		# 0xDCC80C2F

# sceSasCore
	AddNID __sceSasRevParam, 0x08960D98		# 0x267A6DD2
	AddNID __sceSasRevType, 0x08960DA0		# 0x33D4AB37
	AddNID __sceSasInit, 0x08960DA8		# 0x42778A9F
	AddNID __sceSasSetVolume, 0x08960DB0		# 0x440CA7D8
	AddNID __sceSasGetEndFlag, 0x08960DB8		# 0x68A46B95
	AddNID __sceSasSetKeyOn, 0x08960DC0		# 0x76F01ACA
	AddNID __sceSasSetVoice, 0x08960DC8		# 0x99944089
	AddNID __sceSasSetKeyOff, 0x08960DD0		# 0xA0CF2FA4
	AddNID __sceSasCore, 0x08960DD8		# 0xA3589D81
	AddNID __sceSasSetPitch, 0x08960DE0		# 0xAD84D37F
	AddNID __sceSasSetSimpleADSR, 0x08960DE8		# 0xCBCD4F79
	AddNID __sceSasRevEVOL, 0x08960DF0		# 0xD5A229C9
	AddNID __sceSasGetOutputmode, 0x08960DF8		# 0xE175EF66
	AddNID __sceSasRevVON, 0x08960E00		# 0xF983B186

# sceAudio
	AddNID sceAudioOutputPannedBlocking, 0x08960E08		# 0x13F592BC
	AddNID sceAudioChReserve, 0x08960E10		# 0x5EC81C55
	AddNID sceAudioChRelease, 0x08960E18		# 0x6FC46853
	AddNID sceAudioChangeChannelConfig, 0x08960E20		# 0x95FD0C2D
	AddNID sceAudioGetChannelRestLength, 0x08960E28		# 0xB011922F
	AddNID sceAudioChangeChannelVolume, 0x08960E30		# 0xB7E1D8E7
	AddNID sceAudioSetChannelDataLen, 0x08960E38		# 0xCB2E439E

# sceUmdUser
	AddNID sceUmdGetErrorStat, 0x08960E40		# 0x20628E6F
	AddNID sceUmdGetDriveStat, 0x08960E48		# 0x6B4A146C
	AddNID sceUmdActivate, 0x08960E50		# 0xC6183D47

# sceMpeg
	AddNID sceMpegAvcDecodeDetail, 0x08960E58		# 0x0F6C18D7
	AddNID sceMpegRingbufferDestruct, 0x08960E60		# 0x13407F13
	AddNID sceMpegInitAu, 0x08960E68		# 0x167AFD9E
	AddNID sceMpegAvcQueryYCbCrSize, 0x08960E70		# 0x211A057C
	AddNID sceMpegQueryStreamOffset, 0x08960E78		# 0x21FF80E4
	AddNID sceMpegRingbufferConstruct, 0x08960E80		# 0x37295ED8
	AddNID sceMpegRegistStream, 0x08960E88		# 0x42560F23
	AddNID sceMpegUnRegistStream, 0x08960E90		# 0x591A4AA2
	AddNID sceMpegDelete, 0x08960E98		# 0x606A4649
	AddNID sceMpegQueryStreamSize, 0x08960EA0		# 0x611E9E11
	AddNID sceMpegAvcInitYCbCr, 0x08960EA8		# 0x67179B1B
	AddNID sceMpegInit, 0x08960EB0		# 0x682A619B
	AddNID sceMpegFlushAllStream, 0x08960EB8		# 0x707B7629
	AddNID sceMpegAtracDecode, 0x08960EC0		# 0x800C44DF
	AddNID sceMpegFinish, 0x08960EC8		# 0x874624D6
	AddNID sceMpegMallocAvcEsBuf, 0x08960ED0		# 0xA780CF7E
	AddNID sceMpegRingbufferPut, 0x08960ED8		# 0xB240A59E
	AddNID sceMpegRingbufferAvailableSize, 0x08960EE0		# 0xB5F6DC87
	AddNID sceMpegQueryMemSize, 0x08960EE8		# 0xC132E22F
	AddNID sceMpegFreeAvcEsBuf, 0x08960EF0		# 0xCEB870B1
	AddNID sceMpegRingbufferQueryMemSize, 0x08960EF8		# 0xD7A29F46
	AddNID sceMpegCreate, 0x08960F00		# 0xD8C5F121
	AddNID sceMpegGetAtracAu, 0x08960F08		# 0xE1CE83A7
	AddNID sceMpegAvcDecodeYCbCr, 0x08960F10		# 0xF0EB1125
	AddNID sceMpegAvcDecodeStopYCbCr, 0x08960F18		# 0xF2930C9C
	AddNID sceMpeg_F5E7EA31, 0x08960F20		# 0xF5E7EA31
	AddNID sceMpegQueryAtracEsSize, 0x08960F28		# 0xF8DCB679
	AddNID sceMpegGetAvcAu, 0x08960F30		# 0xFE246728

# sceRtc
	AddNID sceRtcGetCurrentClockLocalTime, 0x08960F38		# 0xE7C27D1B

# sceAtrac3plus
	AddNID sceAtrac3plus_2DD3E298, 0x08960F40		# 0x2DD3E298
	AddNID sceAtracGetStreamDataInfo, 0x08960F48		# 0x5D268707
	AddNID sceAtracReleaseAtracID, 0x08960F50		# 0x61EB33F5
	AddNID sceAtracResetPlayPosition, 0x08960F58		# 0x644E5607
	AddNID sceAtracDecodeData, 0x08960F60		# 0x6A8C3CD5
	AddNID sceAtracSetDataAndGetID, 0x08960F68		# 0x7A20E7AF
	AddNID sceAtracSetLoopNum, 0x08960F70		# 0x868120B5
	AddNID sceAtracGetRemainFrame, 0x08960F78		# 0x9AE849A7
	AddNID sceAtracGetSoundSample, 0x08960F80		# 0xA2BBA8BE
	AddNID sceAtracGetBitrate, 0x08960F88		# 0xA554A158
	AddNID sceAtracGetNextDecodePosition, 0x08960F90		# 0xE23E3A35
	AddNID sceAtracGetLoopStatus, 0x08960F98		# 0xFAA4F89B

# sceUtility
	AddNID sceUtilityLoadModule, 0x08960FA0		# 0x2A2B3DE0
	AddNID sceUtilityMsgDialogInitStart, 0x08960FA8		# 0x2AD8E239
	AddNID sceUtilityOskShutdownStart, 0x08960FB0		# 0x3DFAEBA9
	AddNID sceUtilityOskUpdate, 0x08960FB8		# 0x4B85C861
	AddNID sceUtilityNetconfInitStart, 0x08960FC0		# 0x4DB1E739
	AddNID sceUtilitySavedataInitStart, 0x08960FC8		# 0x50C4CD57
	AddNID sceUtilityNetconfGetStatus, 0x08960FD0		# 0x6332AA39
	AddNID sceUtilityMsgDialogShutdownStart, 0x08960FD8		# 0x67AF3428
	AddNID sceUtilitySavedataGetStatus, 0x08960FE0		# 0x8874DBE0
	AddNID sceUtilityNetconfUpdate, 0x08960FE8		# 0x91E70E35
	AddNID sceUtilityMsgDialogUpdate, 0x08960FF0		# 0x95FC253B
	AddNID sceUtilitySavedataShutdownStart, 0x08960FF8		# 0x9790B33C
	AddNID sceUtilityMsgDialogGetStatus, 0x08961000		# 0x9A1C91D7
	AddNID sceUtilityGetSystemParamInt, 0x08961008		# 0xA5DA2406
	AddNID sceUtilitySavedataUpdate, 0x08961010		# 0xD4B95FFB
	AddNID sceUtilityUnloadModule, 0x08961018		# 0xE49BFE92
	AddNID sceUtilityOskGetStatus, 0x08961020		# 0xF3F76017
	AddNID sceUtilityOskInitStart, 0x08961028		# 0xF6269B82
	AddNID sceUtilityNetconfShutdownStart, 0x08961030		# 0xF88155F6

# sceNetAdhoc
	AddNID sceNetAdhocPtpClose, 0x08961038		# 0x157E6225
	AddNID sceNetAdhocPtpSend, 0x08961040		# 0x4DA4C788
	AddNID sceNetAdhocPdpCreate, 0x08961048		# 0x6F92741B
	AddNID sceNetAdhocPdpDelete, 0x08961050		# 0x7F27BB5E
	AddNID sceNetAdhocPtpOpen, 0x08961058		# 0x877F6D66
	AddNID sceNetAdhocPtpRecv, 0x08961060		# 0x8BEA2B3E
	AddNID sceNetAdhocPtpFlush, 0x08961068		# 0x9AC2EEAC
	AddNID sceNetAdhocPtpAccept, 0x08961070		# 0x9DF81198
	AddNID sceNetAdhocTerm, 0x08961078		# 0xA62C6F57
	AddNID sceNetAdhocPdpSend, 0x08961080		# 0xABED3790
	AddNID sceNetAdhocGetPtpStat, 0x08961088		# 0xB9685118
	AddNID sceNetAdhocPdpRecv, 0x08961090		# 0xDFE53E03
	AddNID sceNetAdhocPtpListen, 0x08961098		# 0xE08BDAC1
	AddNID sceNetAdhocInit, 0x089610A0		# 0xE1D621D7
	AddNID sceNetAdhocPtpConnect, 0x089610A8		# 0xFC6FC07B

# sceNet
	AddNID sceNetGetLocalEtherAddr, 0x089610B0		# 0x0BF0A3AE
	AddNID sceNetTerm, 0x089610B8		# 0x281928A9
	AddNID sceNetInit, 0x089610C0		# 0x39AF39A6
	AddNID sceNetFreeThreadinfo, 0x089610C8		# 0x50647530

# sceWlanDrv
	AddNID sceWlanGetEtherAddr, 0x089610D0		# 0x0C622081
	AddNID sceWlanGetSwitchState, 0x089610D8		# 0xD7763699

# sceNetAdhocctl
	AddNID sceNetAdhocctlScan, 0x089610E0		# 0x08FFF7A0
	AddNID sceNetAdhocctlAddHandler, 0x089610E8		# 0x20B317A0
	AddNID sceNetAdhocctlDisconnect, 0x089610F0		# 0x34401D65
	AddNID sceNetAdhocctlDelHandler, 0x089610F8		# 0x6402490B
	AddNID sceNetAdhocctlGetScanInfo, 0x08961100		# 0x81AEE1BE
	AddNID sceNetAdhocctlTerm, 0x08961108		# 0x9D689E13
	AddNID sceNetAdhocctlGetPeerList, 0x08961110		# 0xE162CB14
	AddNID sceNetAdhocctlInit, 0x08961118		# 0xE26F226E

# sceNetApctl
	AddNID sceNetApctlDisconnect, 0x08961120		# 0x24FE91A1
	AddNID sceNetApctlGetInfo, 0x08961128		# 0x2BEFDF23
	AddNID sceNetApctlDelHandler, 0x08961130		# 0x5963991B
	AddNID sceNetApctlAddHandler, 0x08961138		# 0x8ABADD51
	AddNID sceNetApctlTerm, 0x08961140		# 0xB3EDD0EC
	AddNID sceNetApctlInit, 0x08961148		# 0xE2F91F9B

# sceNetInet
	AddNID sceNetInetInit, 0x08961150		# 0x17943399
	AddNID sceNetInetBind, 0x08961158		# 0x1A33F9AE
	AddNID sceNetInetSetsockopt, 0x08961160		# 0x2FE71FE7
	AddNID sceNetInetConnect, 0x08961168		# 0x410B34AA
	AddNID sceNetInetSend, 0x08961170		# 0x7AA671BC
	AddNID sceNetInetCloseWithRST, 0x08961178		# 0x805502DD
	AddNID sceNetInetSocket, 0x08961180		# 0x8B7B220F
	AddNID sceNetInetGetPspError, 0x08961188		# 0x8CA3A97E
	AddNID sceNetInetClose, 0x08961190		# 0x8D7284EA
	AddNID sceNetInetTerm, 0x08961198		# 0xA9ED66B9
	AddNID sceNetInetGetTcpcbstat, 0x089611A0		# 0xB3888AD4
	AddNID sceNetInetInetAddr, 0x089611A8		# 0xB75D5B0A
	AddNID sceNetInetRecv, 0x089611B0		# 0xCDA85C99
	AddNID sceNetInetListen, 0x089611B8		# 0xD10A1A7A
	AddNID sceNetInetAccept, 0x089611C0		# 0xDB094E1B
	AddNID sceNetInetGetpeername, 0x089611C8		# 0xE247B6D6

# scePower
	AddNID scePowerRegisterCallback, 0x089611D0		# 0x04B7766E
	AddNID scePower_469989AD, 0x089611D8		# 0x469989AD
	AddNID scePower_A85880D0, 0x089611E0		# 0xA85880D0

# sceOpenPSID
	AddNID sceOpenPSIDGetOpenPSID, 0x089611E8		# 0xC69BEBCE

# sceImpose
	AddNID sceImposeSetLanguageMode, 0x089611F0		# 0x36AA6E91

# sceDmac
	AddNID sceDmacMemcpy, 0x089611F8		# 0x617F3FE6

# sceNetAdhocDiscover
	AddNID sceNetAdhocDiscoverUpdate, 0x08961200		# 0x52DE1B97
	AddNID sceNetAdhocDiscoverInitStart, 0x08961208		# 0x941B3877
	AddNID sceNetAdhocDiscoverGetStatus, 0x08961210		# 0x944DDBC6
	AddNID sceNetAdhocDiscoverTerm, 0x08961218		# 0xA2246614
	AddNID sceNetAdhocDiscoverRequestSuspend, 0x08961220		# 0xA423A21B
	AddNID sceNetAdhocDiscoverStop, 0x08961228		# 0xF7D13214

# sceHttp
	AddNID sceHttpGetContentLength, 0x08961230		# 0x0282A3BD
	AddNID sceHttpDisableCookie, 0x08961238		# 0x0B12ABFB
	AddNID sceHttpSetRecvTimeOut, 0x08961240		# 0x1F0FC3E3
	AddNID sceHttpGetNetworkPspError, 0x08961248		# 0x2255551E
	AddNID sceHttpAddExtraHeader, 0x08961250		# 0x3EABA285
	AddNID sceHttpGetStatusCode, 0x08961258		# 0x4CC7D78F
	AddNID sceHttpDeleteConnection, 0x08961260		# 0x5152773B
	AddNID sceHttpsLoadDefaultCert, 0x08961268		# 0x87797BDD
	AddNID sceHttpSetConnectTimeOut, 0x08961270		# 0x8ACD1F73
	AddNID sceHttpSetSendTimeOut, 0x08961278		# 0x9988172D
	AddNID sceHttpCreateTemplate, 0x08961280		# 0x9B1F1F36
	AddNID sceHttpSetRedirectCallback, 0x08961288		# 0xA4496DE5
	AddNID sceHttpDeleteRequest, 0x08961290		# 0xA5512E01
	AddNID sceHttpsGetSslError, 0x08961298		# 0xAB1540D5
	AddNID sceHttpInit, 0x089612A0		# 0xAB1ABE07
	AddNID sceHttpDisableAuth, 0x089612A8		# 0xAE948FEE
	AddNID sceHttpCreateRequestWithURL, 0x089612B0		# 0xB509B09E
	AddNID sceHttpSendRequest, 0x089612B8		# 0xBB70706F
	AddNID sceHttpAbortRequest, 0x089612C0		# 0xC10B6BD9
	AddNID sceHttpDisableCache, 0x089612C8		# 0xCCBD167A
	AddNID sceHttpCreateConnectionWithURL, 0x089612D0		# 0xCDF8ECB9
	AddNID sceHttpEnd, 0x089612D8		# 0xD1C8945E
	AddNID sceHttpGetAllHeader, 0x089612E0		# 0xDB266CCF
	AddNID sceHttpsInit, 0x089612E8		# 0xE4D21302
	AddNID sceHttpReadData, 0x089612F0		# 0xEDEEB999
	AddNID sceHttpSetMallocFunction, 0x089612F8		# 0xF49934F6
	AddNID sceHttpsEnd, 0x08961300		# 0xF9D8EB63
	AddNID sceHttpDeleteTemplate, 0x08961308		# 0xFCF8C055

# sceSsl
	AddNID sceSslEnd, 0x08961310		# 0x191CDEFF
	AddNID sceSslInit, 0x08961318		# 0x957ECBE2

# sceJpeg
	AddNID sceJpegCsc, 0x08961320		# 0x67F0ED84

# scePauth
	AddNID scePauth_98B83B5D, 0x08961328		# 0x98B83B5D


	.ident	"VAL-SDK"

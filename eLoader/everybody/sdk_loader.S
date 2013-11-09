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


AddNID sceCtrlSetSamplingCycle , 0x08a38154
AddNID sceCtrlSetSamplingMode , 0x08a3815c
AddNID sceCtrlReadBufferPositive , 0x08a38164


AddNID sceDisplaySetMode , 0x08a3816c
AddNID sceDisplayGetFramePerSec , 0x08a38174
AddNID sceDisplaySetFrameBuf , 0x08a3817c
AddNID sceDisplayGetFrameBuf , 0x08a38184
AddNID sceDisplayWaitVblank , 0x08a3818c
AddNID sceDisplayWaitVblankStart , 0x08a38194
AddNID sceDisplayGetAccumulatedHcount , 0x08a3819c


AddNID sceAudioOutputBlocking , 0x08a3810c
AddNID sceAudioOutputPanned , 0x08a38114
AddNID sceAudioOutputPannedBlocking , 0x08a3811c
AddNID sceAudioChReserve , 0x08a38124
AddNID sceAudioChRelease , 0x08a3812c
AddNID sceAudioGetChannelRestLen , 0x08a38134
AddNID sceAudioSetChannelDataLen , 0x08a3813c
AddNID sceAudioChangeChannelConfig , 0x08a38144
AddNID sceAudioChangeChannelVolume , 0x08a3814c


AddNID sceAtracGetAtracID , 0x08a380cc
AddNID sceAtracReleaseAtracID , 0x08a380d4
AddNID sceAtracSetData , 0x08a380dc
AddNID sceAtracDecodeData , 0x08a380e4
AddNID sceAtracGetStreamDataInfo , 0x08a380ec
AddNID sceAtracAddStreamData , 0x08a380f4
AddNID sceAtracGetNextSample , 0x08a380fc
AddNID sceAtracSetLoopNum , 0x08a38104


AddNID __sceSasCore , 0x08a383e4
AddNID __sceSasGetEndFlag , 0x08a383ec
AddNID __sceSasSetVolume , 0x08a383f4
AddNID __sceSasSetPitch , 0x08a383fc
AddNID __sceSasSetVoice , 0x08a38404
AddNID __sceSasSetNoise , 0x08a3840c
AddNID __sceSasSetADSR , 0x08a38414
AddNID __sceSasSetADSRmode , 0x08a3841c
AddNID __sceSasSetSL , 0x08a38424
AddNID __sceSasGetEnvelopeHeight , 0x08a3842c
AddNID __sceSasSetSimpleADSR , 0x08a38434
AddNID __sceSasInit , 0x08a3843c
AddNID __sceSasSetKeyOff , 0x08a38444
AddNID __sceSasSetKeyOn , 0x08a3844c
AddNID __sceSasRevVON , 0x08a38454
AddNID __sceSasRevEVOL , 0x08a3845c
AddNID __sceSasRevType , 0x08a38464
AddNID __sceSasRevParam , 0x08a3846c


AddNID sceGeEdramGetSize , 0x08a381a4
AddNID sceGeEdramGetAddr , 0x08a381ac
AddNID sceGeListEnQueue , 0x08a381b4
AddNID sceGeListEnQueueHead , 0x08a381bc
AddNID sceGeListUpdateStallAddr , 0x08a381c4
AddNID sceGeListSync , 0x08a381cc
AddNID sceGeDrawSync , 0x08a381d4
AddNID sceGeBreak , 0x08a381dc
AddNID sceGeContinue , 0x08a381e4
AddNID sceGeSetCallback , 0x08a381ec
AddNID sceGeUnsetCallback , 0x08a381f4


AddNID sceUmdCheckMedium , 0x08a3847c
AddNID sceUmdActivate , 0x08a38484
AddNID sceUmdDeactivate , 0x08a3848c
AddNID sceUmdWaitDriveStat , 0x08a38494
AddNID sceUmdGetDriveStat , 0x08a3849c


AddNID sceNetInit , 0x08a382bc
AddNID sceNetTerm , 0x08a382c4
AddNID sceNetGetLocalEtherAddr , 0x08a382cc


AddNID sceNetAdhocInit , 0x08a382d4
AddNID sceNetAdhocTerm , 0x08a382dc
AddNID sceNetAdhocPdpCreate , 0x08a382e4
AddNID sceNetAdhocPdpSend , 0x08a382ec
AddNID sceNetAdhocPdpRecv , 0x08a382f4
AddNID sceNetAdhocPdpDelete , 0x08a382fc
AddNID sceNetAdhocPtpOpen , 0x08a38304
AddNID sceNetAdhocPtpConnect , 0x08a3830c
AddNID sceNetAdhocPtpListen , 0x08a38314
AddNID sceNetAdhocPtpAccept , 0x08a3831c
AddNID sceNetAdhocPtpSend , 0x08a38324
AddNID sceNetAdhocPtpRecv , 0x08a3832c
AddNID sceNetAdhocPtpFlush , 0x08a38334
AddNID sceNetAdhocPtpClose , 0x08a3833c


AddNID sceNetAdhocctlInit , 0x08a3838c
AddNID sceNetAdhocctlTerm , 0x08a38394
AddNID sceNetAdhocctlConnect , 0x08a3839c
AddNID sceNetAdhocctlJoin , 0x08a383a4
AddNID sceNetAdhocctlScan , 0x08a383ac
AddNID sceNetAdhocctlDisconnect , 0x08a383b4
AddNID sceNetAdhocctlAddHandler , 0x08a383bc
AddNID sceNetAdhocctlDelHandler , 0x08a383c4
AddNID sceNetAdhocctlGetState , 0x08a383cc
AddNID sceNetAdhocctlGetScanInfo , 0x08a383d4


AddNID sceNetAdhocMatchingInit , 0x08a38344
AddNID sceNetAdhocMatchingTerm , 0x08a3834c
AddNID sceNetAdhocMatchingCreate , 0x08a38354
AddNID sceNetAdhocMatchingStart , 0x08a3835c
AddNID sceNetAdhocMatchingStop , 0x08a38364
AddNID sceNetAdhocMatchingDelete , 0x08a3836c
AddNID sceNetAdhocMatchingSelectTarget , 0x08a38374
AddNID sceNetAdhocMatchingCancelTarget , 0x08a3837c
AddNID sceNetAdhocMatchingSetHelloOpt , 0x08a38384


AddNID sceUtilitySavedataInitStart , 0x08a384a4
AddNID sceUtilitySavedataShutdownStart , 0x08a384ac
AddNID sceUtilitySavedataUpdate , 0x08a384b4
AddNID sceUtilitySavedataGetStatus , 0x08a384bc
AddNID sceUtilityGetSystemParamInt , 0x08a384c4


AddNID sceRtcGetCurrentClockLocalTime , 0x08a383dc


AddNID sceMpegQueryStreamOffset , 0x08a381fc
AddNID sceMpegQueryStreamSize , 0x08a38204
AddNID sceMpegInit , 0x08a3820c
AddNID sceMpegFinish , 0x08a38214
AddNID sceMpegQueryMemSize , 0x08a3821c
AddNID sceMpegCreate , 0x08a38224
AddNID sceMpegDelete , 0x08a3822c
AddNID sceMpegRegistStream , 0x08a38234
AddNID sceMpegUnRegistStream , 0x08a3823c
AddNID sceMpegMallocAvcEsBuf , 0x08a38244
AddNID sceMpegFreeAvcEsBuf , 0x08a3824c
AddNID sceMpegQueryAtracEsSize , 0x08a38254
AddNID sceMpegInitAu , 0x08a3825c
AddNID sceMpegGetAvcAu , 0x08a38264
AddNID sceMpegGetAtracAu , 0x08a3826c
AddNID sceMpegFlushAllStream , 0x08a38274
AddNID sceMpegAvcDecode , 0x08a3827c
AddNID sceMpegAvcDecodeStop , 0x08a38284
AddNID sceMpegAtracDecode , 0x08a3828c
AddNID sceMpegRingbufferQueryMemSize , 0x08a38294
AddNID sceMpegRingbufferConstruct , 0x08a3829c
AddNID sceMpegRingbufferDestruct , 0x08a382a4
AddNID sceMpegRingbufferPut , 0x08a382ac
AddNID sceMpegRingbufferAvailableSize , 0x08a382b4


AddNID sceKernelRegisterSubIntrHandler , 0x08a37f24
AddNID sceKernelReleaseSubIntrHandler , 0x08a37f2c
AddNID sceKernelEnableSubIntr , 0x08a37f34


AddNID sceIoRead , 0x08a37f3c
AddNID sceIoReadAsync , 0x08a37f44
AddNID sceIoWrite , 0x08a37f4c
AddNID sceIoLseek , 0x08a37f54
AddNID sceIoGetAsyncStat , 0x08a37f5c
AddNID sceIoClose , 0x08a37f64
AddNID sceIoOpen , 0x08a37f6c


AddNID sceKernelCpuSuspendIntr , 0x08a37f74
AddNID sceKernelCpuResumeIntr , 0x08a37f7c


AddNID sceKernelExitGame , 0x08a37f84
AddNID sceKernelRegisterExitCallback , 0x08a37f8c


AddNID sceKernelLoadModule , 0x08a37f94
AddNID sceKernelStartModule , 0x08a37f9c
AddNID sceKernelSelfStopUnloadModule , 0x08a37fa4


AddNID sceKernelStdin , 0x08a37fac
AddNID sceKernelStdout , 0x08a37fb4
AddNID sceKernelStderr , 0x08a37fbc


AddNID sceKernelAllocPartitionMemory , 0x08a37fc4
AddNID sceKernelFreePartitionMemory , 0x08a37fcc
AddNID sceKernelGetBlockHeadAddr , 0x08a37fd4


AddNID sceKernelCheckCallback , 0x08a37fdc
AddNID sceKernelCreateThread , 0x08a37fe4
AddNID sceKernelDeleteThread , 0x08a37fec
AddNID sceKernelStartThread , 0x08a37ff4
AddNID sceKernelExitThread , 0x08a37ffc
AddNID sceKernelTerminateDeleteThread , 0x08a38004
AddNID sceKernelChangeThreadPriority , 0x08a3800c
AddNID sceKernelGetThreadId , 0x08a38014
AddNID sceKernelSleepThread , 0x08a3801c
AddNID sceKernelSuspendThread , 0x08a38024
AddNID sceKernelResumeThread , 0x08a3802c
AddNID sceKernelWaitThreadEnd , 0x08a38034
AddNID sceKernelDelayThread , 0x08a3803c
AddNID sceKernelDelayThreadCB , 0x08a38044
AddNID sceKernelCreateSema , 0x08a3804c
AddNID sceKernelDeleteSema , 0x08a38054
AddNID sceKernelSignalSema , 0x08a3805c
AddNID sceKernelWaitSema , 0x08a38064
AddNID sceKernelPollSema , 0x08a3806c
AddNID sceKernelCreateEventFlag , 0x08a38074
AddNID sceKernelDeleteEventFlag , 0x08a3807c
AddNID sceKernelSetEventFlag , 0x08a38084
AddNID sceKernelClearEventFlag , 0x08a3808c
AddNID sceKernelWaitEventFlag , 0x08a38094
AddNID sceKernelCreateCallback , 0x08a3809c


AddNID sceKernelLibcClock , 0x08a380a4
AddNID sceKernelLibcTime , 0x08a380ac
AddNID sceKernelLibcGettimeofday , 0x08a380b4
AddNID sceKernelDcacheWritebackAll , 0x08a380bc
AddNID sceKernelDcacheWritebackInvalidateAll , 0x08a380c4


AddNID sceKernelPowerTick , 0x08a38474

.ident "FREDDYSDK"

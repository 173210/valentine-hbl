	.set noreorder

#include "pspstub.s"

	STUB_START	"sceAudio",0x40010011,0x00100005
	STUB_FUNC	0x086E5895,sceAudioInputBlocking
	STUB_FUNC	0x136CAF51,sceAudioOutputBlocking
	STUB_FUNC	0x13F592BC,sceAudioOutputPannedBlocking
	STUB_FUNC	0x41EFADE7,sceAudioOneshotOutput
	STUB_FUNC	0x5EC81C55,sceAudioChReserve
	STUB_FUNC	0x6D4BEC68,sceAudioInput
	STUB_FUNC	0x6FC46853,sceAudioChRelease
	STUB_FUNC	0x7DE61688,sceAudioInputInit
	STUB_FUNC	0x87B2E651,sceAudioWaitInputEnd
	STUB_FUNC	0x8C1009B2,sceAudioOutput
	STUB_FUNC	0x95FD0C2D,sceAudioChangeChannelConfig
	STUB_FUNC	0xA708C6A6,sceAudioGetInputLength
	STUB_FUNC	0xB7E1D8E7,sceAudioChangeChannelVolume
	STUB_FUNC	0xCB2E439E,sceAudioSetChannelDataLen
	STUB_FUNC	0xE2D56B2D,sceAudioOutputPanned
	STUB_FUNC	0xE9D97901,sceAudioGetChannelRestLen
	STUB_END

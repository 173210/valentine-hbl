	.set noreorder

#include "pspstub.s"

	STUB_START	"sceCtrl",0x40010011,0x000A0005
	STUB_FUNC	0x02BAAD91,sceCtrlGetSamplingCycle
	STUB_FUNC	0x0B588501,sceCtrlReadLatch
	STUB_FUNC	0x1F4011E6,sceCtrlSetSamplingMode
	STUB_FUNC	0x1F803938,sceCtrlReadBufferPositive
	STUB_FUNC	0x3A622550,sceCtrlPeekBufferPositive
	STUB_FUNC	0x60B81F86,sceCtrlReadBufferNegative
	STUB_FUNC	0x6A2774F3,sceCtrlSetSamplingCycle
	STUB_FUNC	0xB1D0E5CD,sceCtrlPeekLatch
	STUB_FUNC	0xC152080A,sceCtrlPeekBufferNegative
	STUB_FUNC	0xDA6B76A1,sceCtrlGetSamplingMode
	STUB_END

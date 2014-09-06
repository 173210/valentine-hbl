	.set noreorder

#include "pspstub.s"

	STUB_START	"sceDisplay",0x40010011,0x00170005
	STUB_FUNC	0x0E20F177,sceDisplaySetMode
	STUB_FUNC	0x21038913,sceDisplay_21038913
	STUB_FUNC	0x210EAB3A,sceDisplayGetAccumulatedHcount
	STUB_FUNC	0x289D82FE,sceDisplaySetFrameBuf
	STUB_FUNC	0x31C4BAA8,sceDisplay_31C4BAA8
	STUB_FUNC	0x36CDFADE,sceDisplayWaitVblank
	STUB_FUNC	0x40F1469C,sceDisplay_40F1469C
	STUB_FUNC	0x46F186C3,sceDisplayWaitVblankStartCB
	STUB_FUNC	0x4D4E10EC,sceDisplayIsVblank
	STUB_FUNC	0x69B53541,sceDisplay_69B53541
	STUB_FUNC	0x773DD3A3,sceDisplayGetCurrentHcount
	STUB_FUNC	0x77ED8B3A,sceDisplay_77ED8B3A
	STUB_FUNC	0x7ED59BC4,sceDisplaySetHoldMode
	STUB_FUNC	0x8EB9EC49,sceDisplayWaitVblankCB
	STUB_FUNC	0x984C27E7,sceDisplayWaitVblankStart
	STUB_FUNC	0x9C6EAAD7,sceDisplayGetVcount
	STUB_FUNC	0xA544C486,sceDisplaySetResumeMode
	STUB_FUNC	0xA83EF139,sceDisplay_A83EF139
	STUB_FUNC	0xB4F378FA,sceDisplayIsForeground
	STUB_FUNC	0xBF79F646,sceDisplay_BF79F646
	STUB_FUNC	0xDBA6C4C4,sceDisplayGetFramePerSec
	STUB_FUNC	0xDEA197D4,sceDisplayGetMode
	STUB_FUNC	0xEEDA2E54,sceDisplayGetFrameBuf
	STUB_END

.macro AddNID funcname, offset

	.globl  \funcname
	.ent	\funcname
\funcname:
	lui $fp, 0x1
	lw $fp, 24($fp) # 0x00010018, see eloader.c or loader.c for more info
	addi $fp, $fp, \offset
	jr $fp
	nop
	.end	\funcname

.endm

	.file	1 "sdk.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2


# IoFileMgrForUser(5)
	AddNid sceIoOpen, 0x0000  # 0x109f50bc
	AddNid sceIoWrite, 0x0008 # 0x42ec03ac
	AddNid sceIoClose, 0x0010 # 0x810c4bc3
	AddNid sceIoLseek, 0x0018 # 0x27eb27b8
	AddNid sceIoRead, 0x0020  # 0x6a638d83

# ModuleMgrForUser(3)
	AddNid sceKernelQueryModuleInfo, 0x0028      # 0x748cbed9
	AddNid sceKernelGetModuleIdList, 0x0030      # 0x644395e2
	AddNid sceKernelSelfStopUnloadModule, 0x0038 # 0xd675ebb8

# UtilsForUser(1)
	AddNid sceKernelDcacheWritebackInvalidateAll, 0x0040 # 0xb435dec5

# ThreadManForUser(15)
	AddNid sceKernelDelayThread, 0x0048           # 0xceadeb47
	AddNid sceKernelStartThread, 0x0050           # 0xf475845d
	AddNid sceKernelCreateThread, 0x0058          # 0x446d8de6
	AddNid sceKernelTerminateThread, 0x0060       # 0x616403ba
	AddNid sceKernelExitDeleteThread, 0x0068      # 0x809ce29b
	AddNid sceKernelDeleteThread, 0x0070          # 0x9fa03cd3
	AddNid sceKernelReferThreadStatus, 0x0078     # 0x17c1684e
	AddNid sceKernelTerminateDeleteThread, 0x0080 # 0x383f7bcc
	AddNid sceKernelGetThreadmanIdList, 0x0088    # 0x94416130
	AddNid sceKernelReferSemaStatus, 0x0090       # 0xbc6febc5
	AddNid sceKernelReferFplStatus, 0x0098        # 0xd8199e4c
	AddNid sceKernelReferEventFlagStatus, 0x00a0  # 0xa66b0120
	AddNid sceKernelDeleteSema, 0x00a8            # 0x28b6489c
	AddNid sceKernelDeleteFpl, 0x00b0             # 0xed1410e0
	AddNid sceKernelDeleteEventFlag, 0x00b8       # 0xef9e4c70

# LoadExecForUser(1)
	AddNid sceKernelExitGame, 0x00c0 # 0x5572a5f

# SysMemUserForUser(3)
	AddNid sceKernelAllocPartitionMemory, 0x00c8 # 0x237dbd4f
	AddNid sceKernelFreePartitionMemory, 0x00d0  # 0xb6d61d02
	AddNid sceKernelTotalFreeMemSize, 0x00d8     # 0xf919f628



	.ident	"HBL-SDK"

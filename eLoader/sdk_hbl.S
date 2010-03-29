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


# IoFileMgrForUser(5)
	AddNid sceIoOpen, 0x13f00  # 0x109f50bc
	AddNid sceIoWrite, 0x13f08 # 0x42ec03ac
	AddNid sceIoClose, 0x13f10 # 0x810c4bc3
	AddNid sceIoLseek, 0x13f18 # 0x27eb27b8
	AddNid sceIoRead, 0x13f20  # 0x6a638d83

# ModuleMgrForUser(3)
	AddNid sceKernelQueryModuleInfo, 0x13f28      # 0x748cbed9
	AddNid sceKernelGetModuleIdList, 0x13f30      # 0x644395e2
	AddNid sceKernelSelfStopUnloadModule, 0x13f38 # 0xd675ebb8

# UtilsForUser(1)
	AddNid sceKernelDcacheWritebackInvalidateAll, 0x13f40 # 0xb435dec5

# ThreadManForUser(13)
	AddNid sceKernelDelayThread, 0x13f48           # 0xceadeb47
	AddNid sceKernelStartThread, 0x13f50           # 0xf475845d
	AddNid sceKernelCreateThread, 0x13f58          # 0x446d8de6
	AddNid sceKernelTerminateThread, 0x13f60       # 0x616403ba
	AddNid sceKernelExitDeleteThread, 0x13f68      # 0x809ce29b
	AddNid sceKernelDeleteThread, 0x13f70          # 0x9fa03cd3
	AddNid sceKernelReferThreadStatus, 0x13f78     # 0x17c1684e
	AddNid sceKernelTerminateDeleteThread, 0x13f80 # 0x383f7bcc
	AddNid sceKernelGetThreadmanIdList, 0x13f88    # 0x94416130
	AddNid sceKernelReferSemaStatus, 0x13f90       # 0xbc6febc5
	AddNid sceKernelReferFplStatus, 0x13f98        # 0xd8199e4c
	AddNid sceKernelDeleteSema, 0x13fa0            # 0x28b6489c
	AddNid sceKernelDeleteFpl, 0x13fa8             # 0xed1410e0

# LoadExecForUser(1)
	AddNid sceKernelExitGame, 0x13fb0 # 0x5572a5f

# SysMemUserForUser(2)
	AddNid sceKernelFreePartitionMemory, 0x13fb8 # 0xb6d61d02
	AddNid sceKernelTotalFreeMemSize, 0x13fc0    # 0xf919f628



	.ident	"HBL-SDK"

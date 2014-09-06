.set noreorder

b skip
nop

relocate:
addiu $t3, $t3, -1
relocateloop:
lw $t2, 0($t0)
sw $t2, 0($t1)
addiu $t0, $t0, 4
addiu $t1, $t1, 4
bgtz $t3, relocateloop
addiu $t3, $t3, -1
jr $ra
nop

skip:

/*Load MOD.BIN into memory*/
b mod
nop
/*sceIoOpen*/
returnname1:
move $a0, $ra
li $a1, 1
jal 0x0888c3d4
li $a2, 31
/*sceIoRead*/
move $a0, $v0
lui $a1, 0x08c8
lui $a2, 1
jal 0x0888c3bc
move $s0, $a0
/*sceIoClose*/
jal 0x0888c3cc
move $a0, $s0

/*sceUtilityLoadModule(0x102)*/
jal 0x0888c3fc
li $a0, 0x102

/*Relocate calls*/
lui $t0, 0x9f7
ori $t0, $t0, 0x180
lui $t1, 0x8c8
bal relocate /*sceKernelGetModuleGPByAddress*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x458
lui $t1, 0x8c8
ori $t1, $t1, 0x48
bal relocate /*sceWlanGetSwitchState*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x468
lui $t1, 0x8c8
ori $t1, $t1, 0x50
bal relocate /*sceWlanDevAttach*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x478
lui $t1, 0x8c8
ori $t1, $t1, 0x58
bal relocate /*sceWlanDevDetach*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x188
lui $t1, 0x8c8
ori $t1, $t1, 0x60
bal relocate /*sceKernelQueryMemoryInfo*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x220
lui $t1, 0x8c8
ori $t1, $t1, 0x68
bal relocate /*sceKernelCancelAlarm*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x240
lui $t1, 0x8c8
ori $t1, $t1, 0x70
bal relocate /*sceKernelCreateLwMutex*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x208
lui $t1, 0x8c8
ori $t1, $t1, 0x98
bal relocate /*sceKernelDeleteLwMutex*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x1a0
lui $t1, 0x8c8
ori $t1, $t1, 0xc8
bal relocate /*sceKernelGetSystemTime*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x9d14
lui $t1, 0x8c8
ori $t1, $t1, 0xd0
bal relocate /*sceKernelGetSystemTimeWide*/
li $t3, 2

lui $t0, 0x888
ori $t0, $t0, 0xc3a4
lui $t1, 0x8c8
ori $t1, $t1, 0xd8
bal relocate /*sceKernelGetThreadId*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x200
lui $t1, 0x8c8
ori $t1, $t1, 0xf0
bal relocate /*sceKernelGetThreadId*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x218
lui $t1, 0x8c8
ori $t1, $t1, 0x120
bal relocate /*sceKernelSetAlarm*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x210
lui $t1, 0x8c8
ori $t1, $t1, 0x128
bal relocate /*sceKernelTerminateThread*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x258
lui $t1, 0x8c8
ori $t1, $t1, 0x140
bal relocate /*sceKernelUtilsMd5BlockInit*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x260
lui $t1, 0x8c8
ori $t1, $t1, 0x148
bal relocate /*sceKernelUtilsMd5BlockResult*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x250
lui $t1, 0x8c8
ori $t1, $t1, 0x150
bal relocate /*sceKernelUtilsMd5BlockUpdate*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x268
lui $t1, 0x8c8
ori $t1, $t1, 0x158
bal relocate /*sceKernelUtilsSha1BlockInit*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x248
lui $t1, 0x8c8
ori $t1, $t1, 0x160
bal relocate /*sceKernelUtilsSha1BlockResult*/
li $t3, 2

lui $t0, 0x9f7
ori $t0, $t0, 0x270
lui $t1, 0x8c8
ori $t1, $t1, 0x168
bal relocate /*sceKernelUtilsSha1BlockUpdate*/
li $t3, 2

/*sceUtilityUnloadModule(0x102)*/
jal 0x0888c444
li $a0, 0x102

/*sceUtilityLoadModule(0x400)*/
jal 0x0888c3fc
li $a0, 0x400

lui $t0, 0x9f2
ori $t0, $t0, 0xd7d0
lui $t1, 0x8c8
ori $t1, $t1, 0x10
bal relocate /*sceRegCloseCategory*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7f8
lui $t1, 0x8c8
ori $t1, $t1, 0x18
bal relocate /*sceRegCloseRegistry*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7f0
lui $t1, 0x8c8
ori $t1, $t1, 0x20
bal relocate /*sceRegGetKeyInfo*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7e0
lui $t1, 0x8c8
ori $t1, $t1, 0x28
bal relocate /*sceRegGetKeyValue*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7d8
lui $t1, 0x8c8
ori $t1, $t1, 0x30
bal relocate /*sceRegOpenCategory*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7e8
lui $t1, 0x8c8
ori $t1, $t1, 0x38
bal relocate /*sceRegOpenRegistry*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x22f8
lui $t1, 0x8c8
ori $t1, $t1, 0x78
bal relocate /*sceKernelCreateMbx*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7b8
lui $t1, 0x8c8
ori $t1, $t1, 0x88
bal relocate /*sceKernelCreateMutex*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x22f0
lui $t1, 0x8c8
ori $t1, $t1, 0x90
bal relocate /*sceKernelCreateVpl*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x2300
lui $t1, 0x8c8
ori $t1, $t1, 0xa0
bal relocate /*sceKernelDeleteMbx*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd790
lui $t1, 0x8c8
ori $t1, $t1, 0xb0
bal relocate /*sceKernelDeleteMutex*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x2308
lui $t1, 0x8c8
ori $t1, $t1, 0xb8
bal relocate /*sceKernelDeleteVpl*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x2328
lui $t1, 0x8c8
ori $t1, $t1, 0xc0
bal relocate /*sceKernelFreeVpl*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7b0
lui $t1, 0x8c8
ori $t1, $t1, 0xe0
bal relocate /*sceKernelLockMutex*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x2310
lui $t1, 0x8c8
ori $t1, $t1, 0xf8
bal relocate /*sceKernelReceiveMbx*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x22e0
lui $t1, 0x8c8
ori $t1, $t1, 0x108
bal relocate /*sceKernelReferVplStatus*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x22c8
lui $t1, 0x8c8
ori $t1, $t1, 0x110
bal relocate /*sceKernelSendMbx*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x2320
lui $t1, 0x8c8
ori $t1, $t1, 0x130
bal relocate /*sceKernelTryAllocateVpl*/
li $t3, 2

lui $t0, 0x9f2
ori $t0, $t0, 0xd7a0
lui $t1, 0x8c8
ori $t1, $t1, 0x138
bal relocate /*sceKernelUnlockMutex*/
li $t3, 2

/*sceUtilityUnloadModule(0x400)*/
jal 0x0888c444
li $a0, 0x400

/*sceUtilityLoadModule(0x401)*/
jal 0x0888c3fc
li $a0, 0x401

lui $t0, 0x9f3
ori $t0, $t0, 0x8ec8
lui $t1, 0x8c8
ori $t1, $t1, 0x40
bal relocate /*sceRtcParseDateTime*/
li $t3, 2

/*sceUtilityUnloadModule(0x401)*/
jal 0x0888c444
li $a0, 0x401

/*sceUtilityLoadModule(0x402)*/
jal 0x0888c3fc
li $a0, 0x402

lui $t0, 0x9f5
ori $t0, $t0, 0x5f88
lui $t1, 0x8c8
ori $t1, $t1, 0x8
bal relocate /*sceOpenPSIDGetOpenPSID*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5d80
lui $t1, 0x8c8
ori $t1, $t1, 0x80
bal relocate /*sceKernelCreateMsgPipe*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5cf0
lui $t1, 0x8c8
ori $t1, $t1, 0xa8
bal relocate /*sceKernelDeleteMsgPipe*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5d28
lui $t1, 0x8c8
ori $t1, $t1, 0xe8
bal relocate /*sceKernelPollMbx*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5d78
lui $t1, 0x8c8
ori $t1, $t1, 0x100
bal relocate /*sceKernelReceiveMsgPipe*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5da0
lui $t1, 0x8c8
ori $t1, $t1, 0x118
bal relocate /*sceKernelSendMsgPipe*/
li $t3, 2

/*sceUtilityUnloadModule(0x402)*/
jal 0x0888c444
li $a0, 0x402

/*Load and execute H.BIN*/
b hbin
nop
/*sceIoOpen*/
returnname2:
move $a0, $ra
li $a1, 1
jal 0x0888c3d4
li $a2, 31
/*sceIoRead*/
move $a0, $v0
lui $a1, 0x08d2
lui $a2, 1
jal 0x0888c3bc
move $s0, $a0
/*sceIoClose*/
jal 0x0888c3cc
move $a0, $s0
lui $a0, 0x8d2
jr $a0
nop

mod:
bal returnname1
nop
.ascii "ms0:/PSP/SAVEDATA/NPEZ00374DATA00/MOD.BIN\0\0\0"

hbin:
bal returnname2
nop
.ascii "ms0:/PSP/SAVEDATA/NPEZ00374DATA00/H.BIN\0"

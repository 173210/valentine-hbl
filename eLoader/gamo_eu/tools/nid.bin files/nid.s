.set noreorder

/*
This code will load a few utilities and relocate their calls to a crafted module.
VHBL currently loads all utilities at once and these are the ones that don't get loaded due to allocation errors.
This code is game-specific and will not work with other games.
qwikrazor87
*/

b skipfunctions
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

initcolor:
li $t0, 0x40
li $s6, 0xff
move $s7, $t7
color:
lui $t8, 0x4400
lui $t9, 0x4410
sw $t7, 0($t8)
fill:
addiu $t8, $t8, 4
bnel $t8, $t9, fill
sw $t7, 0($t8)
bnez $t0, color
addiu $t0, $t0, -1
beq $s6, $s7, exitgame
nop
jr $ra
nop

exitgame:
jal 0x0888c374
nop

skipfunctions:

/*Load the crafted module into memory*/
b modpath /*Get path offset for arg0*/
nop

loadmod:
/*sceIoOpen, PSP_O_RDONLY | 0777*/
move $a0, $ra
li $a1, 1
jal 0x0888c3d4
li $a2, 511
lui $t0, 0x8c1
sw $v0, 0($t0)

bltz $v0, initcolor
li $t7, 0xff

/*sceIoRead*/
lui $a1, 0x8c8
lui $t0, 0x8c1
lw $a0, 0($t0)
jal 0x0888c3bc
li $a2, 1672

beqz $v0, initcolor
li $t7, 0xff

/*sceIoClose*/
lui $t0, 0x8c1
jal 0x0888c3cc
lw $a0, 0($t0)

bltz $v0, initcolor
li $t7, 0xff

jal 0x0888c3fc /*sceUtilityLoadModule(0x102)*/
li $a0, 0x102

bltz $v0, initcolor
li $t7, 0xff

/*Relocate calls*/

lui $t0, 0x9f5
ori $t0, $t0, 0x917c
lui $t1, 0x8c8
ori $t1, $t1, 0x80
bal relocate /*ThreadManForUser*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x9184
lui $t1, 0x8c8
ori $t1, $t1, 0x2d0
bal relocate /*UtilsForUser*/
li $t3, 6

lui $t0, 0x9f6
ori $t0, $t0, 0xfc28
lui $t1, 0x8c8
bal relocate /*ModuleMgrForUser*/
li $t3, 2

lui $t0, 0x9f6
ori $t0, $t0, 0xfef8

lui $t1, 0x8c8
ori $t1, $t1, 0x28
bal relocate /*sceUtility*/
li $t3, 6

lui $t0, 0x9f6
ori $t0, $t0, 0xff38
lui $t1, 0x8c8
ori $t1, $t1, 0x40
bal relocate /*sceWlanDrv*/
li $t3, 2

lui $t0, 0x9f6
ori $t0, $t0, 0xff40
lui $t1, 0x8c8
ori $t1, $t1, 0x48
bal relocate /*sceWlanDrv_lib*/
li $t3, 4

lui $t0, 0x9f6
ori $t0, $t0, 0xfc30
lui $t1, 0x8c8
ori $t1, $t1, 0x58
bal relocate /*SysMemUserForUser*/
li $t3, 2

lui $t0, 0x9f6
ori $t0, $t0, 0xfc48
lui $t1, 0x8c8
ori $t1, $t1, 0x88
bal relocate /*ThreadManForUser*/
li $t3, 42

lui $t0, 0x9f6
ori $t0, $t0, 0xfcf0
lui $t1, 0x8c8
ori $t1, $t1, 0x2e8
bal relocate /*UtilsForUser*/
li $t3, 12

lui $t0, 0x9f7
ori $t0, $t0, 0xa0cc
lui $t1, 0x8c8
ori $t1, $t1, 0x130
bal relocate /*ThreadManForUser*/
li $t3, 2

jal 0x0888c444 /*sceUtilityUnloadModule(0x102)*/
li $a0, 0x102

bltz $v0, initcolor
li $t7, 0xff

jal 0x0888c3fc /*sceUtilityLoadModule(0x401)*/
li $a0, 0x401

bltz $v0, initcolor
li $t7, 0xff

lui $t0, 0x9f3
ori $t0, $t0, 0x8be0
lui $t1, 0x8c8
ori $t1, $t1, 8
bal relocate /*ModuleMgrForUser*/
li $t3, 2

lui $t0, 0x9f3
ori $t0, $t0, 0x8ec0
lui $t1, 0x8c8
ori $t1, $t1, 0x18
bal relocate /*sceRtc*/
li $t3, 4

lui $t0, 0x9f3
ori $t0, $t0, 0x8be8
lui $t1, 0x8c8
ori $t1, $t1, 0x60
bal relocate /*SysMemUserForUser*/
li $t3, 4

lui $t0, 0x9f3
ori $t0, $t0, 0x8c10
lui $t1, 0x8c8
ori $t1, $t1, 0x138
bal relocate /*ThreadManForUser*/
li $t3, 36

jal 0x0888c444 /*sceUtilityUnloadModule(0x401)*/
li $a0, 0x401

bltz $v0, initcolor
li $t7, 0xff

jal 0x0888c3fc /*sceUtilityLoadModule(0x402)*/
li $a0, 0x402

bltz $v0, initcolor
li $t7, 0xff

lui $t0, 0x9f5
ori $t0, $t0, 0x5f88
lui $t1, 0x8c8
ori $t1, $t1, 0x10
bal relocate /*sceOpenPSID*/
li $t3, 2

lui $t0, 0x9f5
ori $t0, $t0, 0x5cb8
lui $t1, 0x8c8
ori $t1, $t1, 0x70
bal relocate /*SysMemUserForUser*/
li $t3, 4

lui $t0, 0x9f5
ori $t0, $t0, 0x5cd8
lui $t1, 0x8c8
ori $t1, $t1, 0x1c8
bal relocate /*ThreadManForUser*/
li $t3, 66

jal 0x0888c444 /*sceUtilityUnloadModule(0x402)*/
li $a0, 0x402

bltz $v0, initcolor
li $t7, 0xff

/*Load and execute H.BIN*/
lui $t0, 0x889
ori $t0, $t0, 0x64b8
li $t1, 0xc0
jal 0x0888c254 /*sceKernelDcacheWritebackInvalidateAll()*/
sb $t1, 0($t0) /*Modify path pointer to H.BIN*/
lui $ra, 0x889
ori $ra, $ra, 0x64b4
jr $ra /*Jump back to binloader*/
nop

modpath:
bal loadmod
nop
.ascii "ms0:/PSP/SAVEDATA/NPEZ00374DATA00/MOD.BIN"

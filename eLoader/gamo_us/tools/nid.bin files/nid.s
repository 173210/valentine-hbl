.set noreorder

b skip /*Skip the functions for now since they will not be used yet*/
nop

/*Fill screen with red on error and break*/
error:
li $v0, 0xFF
lui $v1, 0x4400
lui $a0, 0x4410
sw $v0, 0($v1)
fill:
addiu $v1, $v1, 4
bnel $v1, $a0, fill
sw $v0, 0($v1)
nop
break
/*End of error function*/

/*Start of relocating function*/
relocate:
lw $t2, 0($t0) /*Source*/
sw $t2, 0($t1) /*Destination*/
addiu $t0, $t0, 4
addiu $t1, $t1, 4
bnez $t3, relocate
addiu $t3, $t3, -1 /*Loop counter*/
jr $ra
nop
/*End of relocating function*/

skip:

/*Load MOD.BIN into memory*/
lui $a0, 0x889		/*filename*/
ori $a0, $a0, 0x65ec
li $a1, 1
li $a2, 31
jal 0x0888c3c8		/* sceIoOpen */
nop

bltz $v0, error		/*If file fails to open then jump to error*/
nop

move $a0, $v0		/* set the return value of the function for arg0 of the next function */

lui $a1, 0x880		/* arg1 is 0x08800c00, load address of the binary file */
ori $a1, $a1, 0xc00
lui $a2, 1		/* arg2, read 0x10000 bytes from the file */
jal 0x0888c3b0		/* sceIoRead */
move $s0, $a0		/* backup $a0 value */

beqz $v0, error		/*If no bytes are read then jump to error*/
nop

jal 0x0888c3c0		/* sceIoClose */
move $a0, $s0		/* restore $a0 value */

bltz $v0, error		/*If file fails to close then jump to error*/
nop
/*End of loading MOD.BIN*/

jal 0x0888c3f0 /*sceUtilityLoadModule(0x102)*/
li $a0, 0x102
bltz $v0, error
nop

/*Relocate calls and NIDs*/
/*Kernel_Library*/
lui $t0, 0x9f5
ori $t0, $t0, 0x97cc
lui $t1, 0x880
ori $t1, $t1, 0x143c
bal relocate
li $t3, 3
lui $t0, 0x9f5
ori $t0, $t0, 0x9154
lui $t1, 0x880
ori $t1, $t1, 0xc00
bal relocate
li $t3, 7

/*ThreadManForUser*/
lui $t0, 0x9f5
ori $t0, $t0, 0x97dc
lui $t1, 0x880
ori $t1, $t1, 0x1574
bal relocate
li $t3, 1
lui $t0, 0x9f5
ori $t0, $t0, 0x9174
lui $t1, 0x880
ori $t1, $t1, 0xe70
bal relocate
li $t3, 3

/*UtilsForUser*/
lui $t0, 0x9f5
ori $t0, $t0, 0x97e4
lui $t1, 0x880
ori $t1, $t1, 0x1704
bal relocate
li $t3, 2
lui $t0, 0x9f5
ori $t0, $t0, 0x9184
lui $t1, 0x880
ori $t1, $t1, 0x1190
bal relocate
li $t3, 5

/*sceWlanDrv*/
lui $t0, 0x9f7
ori $t0, $t0, 0x534
lui $t1, 0x880
ori $t1, $t1, 0x1550
bal relocate
li $t3, 0
lui $t0, 0x9f6
ori $t0, $t0, 0xff38
lui $t1, 0x880
ori $t1, $t1, 0xe28
bal relocate
li $t3, 1

/*sceWlanDrv_lib*/
lui $t0, 0x9f7
ori $t0, $t0, 0x538
lui $t1, 0x880
ori $t1, $t1, 0x1554
bal relocate
li $t3, 1
lui $t0, 0x9f6
ori $t0, $t0, 0xff40
lui $t1, 0x880
ori $t1, $t1, 0xe30
bal relocate
li $t3, 3

/*sceUtility*/
lui $t0, 0x9f7
ori $t0, $t0, 0x514
lui $t1, 0x880
ori $t1, $t1, 0x1530
bal relocate
li $t3, 2
lui $t0, 0x9f6
ori $t0, $t0, 0xfef8
lui $t1, 0x880
ori $t1, $t1, 0xde8
bal relocate
li $t3, 5

/*sceUtility_netparam_internal*/
lui $t0, 0x9f7
ori $t0, $t0, 0x520
lui $t1, 0x880
ori $t1, $t1, 0x153c
bal relocate
li $t3, 4
lui $t0, 0x9f6
ori $t0, $t0, 0xff10
lui $t1, 0x880
ori $t1, $t1, 0xe00
bal relocate
li $t3, 9

/*SysMemUserForUser*/
lui $t0, 0x9f7
ori $t0, $t0, 0x3b0
lui $t1, 0x880
ori $t1, $t1, 0x155c
bal relocate
li $t3, 0
lui $t0, 0x9f6
ori $t0, $t0, 0xfc30
lui $t1, 0x880
ori $t1, $t1, 0xe40
bal relocate
li $t3, 1

/*Kernel_Library*/
lui $t0, 0x9f7
ori $t0, $t0, 0x398
lui $t1, 0x880
ori $t1, $t1, 0x144c
bal relocate
li $t3, 4
lui $t0, 0x9f6
ori $t0, $t0, 0xfc00
lui $t1, 0x880
ori $t1, $t1, 0xc20
bal relocate
li $t3, 9

/*ModuleMgrForUser*/
lui $t0, 0x9f7
ori $t0, $t0, 0x3ac
lui $t1, 0x880
ori $t1, $t1, 0x14cc
bal relocate
li $t3, 0
lui $t0, 0x9f6
ori $t0, $t0, 0xfc28
lui $t1, 0x880
ori $t1, $t1, 0xd20
bal relocate
li $t3, 1

/*ThreadManForUser*/
lui $t0, 0x9f7
ori $t0, $t0, 0x3b4
lui $t1, 0x880
ori $t1, $t1, 0x157c
bal relocate
li $t3, 22
lui $t0, 0x9f6
ori $t0, $t0, 0xfc38
lui $t1, 0x880
ori $t1, $t1, 0xe80
bal relocate
li $t3, 45

/*UtilsForUser*/
lui $t0, 0x9f7
ori $t0, $t0, 0x410
lui $t1, 0x880
ori $t1, $t1, 0x1710
bal relocate
li $t3, 5
lui $t0, 0x9f6
ori $t0, $t0, 0xfcf0
lui $t1, 0x880
ori $t1, $t1, 0x11a8
bal relocate
li $t3, 11

/*Kernel_Library*/
lui $t0, 0x9f7
ori $t0, $t0, 0xa32c
lui $t1, 0x880
ori $t1, $t1, 0x1460
bal relocate
li $t3, 5
lui $t0, 0x9f7
ori $t0, $t0, 0xa09c
lui $t1, 0x880
ori $t1, $t1, 0xc48
bal relocate
li $t3, 11

/*ThreadManForUser*/
lui $t0, 0x9f7
ori $t0, $t0, 0xa344
lui $t1, 0x880
ori $t1, $t1, 0x15d8
bal relocate
li $t3, 1
lui $t0, 0x9f7
ori $t0, $t0, 0xa0cc
lui $t1, 0x880
ori $t1, $t1, 0xf38
bal relocate
li $t3, 3

jal 0x0888c438 /*sceUtilityUnloadModule(0x102)*/
li $a0, 0x102
bltz $v0, error
nop

jal 0x0888c3f0 /*sceUtilityLoadModule(0x400)*/
li $a0, 0x400
bltz $v0, error
nop

/*sceRtc*/
lui $t0, 0x9f2
ori $t0, $t0, 0xdac4
lui $t1, 0x880
ori $t1, $t1, 0x1520
bal relocate
li $t3, 0
lui $t0, 0x9f2
ori $t0, $t0, 0xd800
lui $t1, 0x880
ori $t1, $t1, 0xdc8
bal relocate
li $t3, 1

/*sceReg*/
lui $t0, 0x9f2
ori $t0, $t0, 0xdaac
lui $t1, 0x880
ori $t1, $t1, 0x1508
bal relocate
li $t3, 5
lui $t0, 0x9f2
ori $t0, $t0, 0xd7d0
lui $t1, 0x880
ori $t1, $t1, 0xd98
bal relocate
li $t3, 11

/*sceNpCore*/
lui $t0, 0x9f2
ori $t0, $t0, 0xdaa4
lui $t1, 0x880
ori $t1, $t1, 0x14d8
bal relocate
li $t3, 1
lui $t0, 0x9f2
ori $t0, $t0, 0xd7c0
lui $t1, 0x880
ori $t1, $t1, 0xd38
bal relocate
li $t3, 3

/*Kernel_Library*/
lui $t0, 0x9f2
ori $t0, $t0, 0xda88
lui $t1, 0x880
ori $t1, $t1, 0x1478
bal relocate
li $t3, 0
lui $t0, 0x9f2
ori $t0, $t0, 0xd788
lui $t1, 0x880
ori $t1, $t1, 0xc78
bal relocate
li $t3, 1

/*ThreadManForUser*/
lui $t0, 0x9f2
ori $t0, $t0, 0xda8c
lui $t1, 0x880
ori $t1, $t1, 0x15e0
bal relocate
li $t3, 5
lui $t0, 0x9f2
ori $t0, $t0, 0xd790
lui $t1, 0x880
ori $t1, $t1, 0xf48
bal relocate
li $t3, 11

/*sceNpCore*/
lui $t0, 0x9f3
ori $t0, $t0, 0x27ac
lui $t1, 0x880
ori $t1, $t1, 0x14e0
bal relocate
li $t3, 8
lui $t0, 0x9f3
ori $t0, $t0, 0x2408
lui $t1, 0x880
ori $t1, $t1, 0xd48
bal relocate
li $t3, 17

/*SysMemUserForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x2708
lui $t1, 0x880
ori $t1, $t1, 0x1560
bal relocate
li $t3, 0
lui $t0, 0x9f3
ori $t0, $t0, 0x22c0
lui $t1, 0x880
ori $t1, $t1, 0xe48
bal relocate
li $t3, 1

/*Kernel_Library*/
lui $t0, 0x9f3
ori $t0, $t0, 0x26f0
lui $t1, 0x880
ori $t1, $t1, 0x147c
bal relocate
li $t3, 4
lui $t0, 0x9f3
ori $t0, $t0, 0x2290
lui $t1, 0x880
ori $t1, $t1, 0xc80
bal relocate
li $t3, 9

/*ModuleMgrForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x2704
lui $t1, 0x880
ori $t1, $t1, 0x14d0
bal relocate
li $t3, 0
lui $t0, 0x9f3
ori $t0, $t0, 0x22b8
lui $t1, 0x880
ori $t1, $t1, 0xd28
bal relocate
li $t3, 1

/*ThreadManForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x270c
lui $t1, 0x880
ori $t1, $t1, 0x15f8
bal relocate
li $t3, 12
lui $t0, 0x9f3
ori $t0, $t0, 0x22c8
lui $t1, 0x880
ori $t1, $t1, 0xf78
bal relocate
li $t3, 25

jal 0x0888c438 /*sceUtilityUnloadModule(0x400)*/
li $a0, 0x400
bltz $v0, error
nop

jal 0x0888c3f0 /*sceUtilityLoadModule(0x401)*/
li $a0, 0x401
bltz $v0, error
nop

/*SysMemUserForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x9300
lui $t1, 0x880
ori $t1, $t1, 0x1564
bal relocate
li $t3, 1
lui $t0, 0x9f3
ori $t0, $t0, 0x8be8
lui $t1, 0x880
ori $t1, $t1, 0xe50
bal relocate
li $t3, 3

/*sceRtc*/
lui $t0, 0x9f3
ori $t0, $t0, 0x9468
lui $t1, 0x880
ori $t1, $t1, 0x1524
bal relocate
li $t3, 2
lui $t0, 0x9f3
ori $t0, $t0, 0x8eb8
lui $t1, 0x880
ori $t1, $t1, 0xdd0
bal relocate
li $t3, 5

/*Kernel_Library*/
lui $t0, 0x9f3
ori $t0, $t0, 0x92e0
lui $t1, 0x880
ori $t1, $t1, 0x1490
bal relocate
li $t3, 6
lui $t0, 0x9f3
ori $t0, $t0, 0x8ba8
lui $t1, 0x880
ori $t1, $t1, 0xca8
bal relocate
li $t3, 13

/*ModuleMgrForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x92fc
lui $t1, 0x880
ori $t1, $t1, 0x14d4
bal relocate
li $t3, 0
lui $t0, 0x9f3
ori $t0, $t0, 0x8be0
lui $t1, 0x880
ori $t1, $t1, 0xd30
bal relocate
li $t3, 1

/*ThreadManForUser*/
lui $t0, 0x9f3
ori $t0, $t0, 0x9308
lui $t1, 0x880
ori $t1, $t1, 0x162c
bal relocate
li $t3, 20
lui $t0, 0x9f3
ori $t0, $t0, 0x8bf8
lui $t1, 0x880
ori $t1, $t1, 0xfe0
bal relocate
li $t3, 41

jal 0x0888c438 /*sceUtilityUnloadModule(0x401)*/
li $a0, 0x401
bltz $v0, error
nop

jal 0x0888c3f0 /*sceUtilityLoadModule(0x402)*/
li $a0, 0x402
bltz $v0, error
nop

/*sceOpenPSID*/
lui $t0, 0x9f5
ori $t0, $t0, 0x66e8
lui $t1, 0x880
ori $t1, $t1, 0x1504
bal relocate
li $t3, 0
lui $t0, 0x9f5
ori $t0, $t0, 0x5f88
lui $t1, 0x880
ori $t1, $t1, 0xd90
bal relocate
li $t3, 1

/*SysMemUserForUser*/
lui $t0, 0x9f5
ori $t0, $t0, 0x6580
lui $t1, 0x880
ori $t1, $t1, 0x156c
bal relocate
li $t3, 1
lui $t0, 0x9f5
ori $t0, $t0, 0x5cb8
lui $t1, 0x880
ori $t1, $t1, 0xe60
bal relocate
li $t3, 3

/*Kernel_Library*/
lui $t0, 0x9f5
ori $t0, $t0, 0x6560
lui $t1, 0x880
ori $t1, $t1, 0x14ac
bal relocate
li $t3, 7
lui $t0, 0x9f5
ori $t0, $t0, 0x5c78
lui $t1, 0x880
ori $t1, $t1, 0xce0
bal relocate
li $t3, 15

/*ThreadManForUser*/
lui $t0, 0x9f5
ori $t0, $t0, 0x6590
lui $t1, 0x880
ori $t1, $t1, 0x1680
bal relocate
li $t3, 32
lui $t0, 0x9f5
ori $t0, $t0, 0x5cd8
lui $t1, 0x880
ori $t1, $t1, 0x1088
bal relocate
li $t3, 65

jal 0x0888c438 /*sceUtilityUnloadModule(0x402)*/
li $a0, 0x402
bltz $v0, error
nop

jal 0x088965C0 /*Jump back to savedata and load H.BIN*/
nop

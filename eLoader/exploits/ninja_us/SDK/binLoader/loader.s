
.set noat
.set noreorder


nop


addiu $a0, $ra, 0x71	/* filename. If ou change this value, move it in the savegame */
nop
li $a1, 1
li $a2, 31
jal 0x088F5A90		/* sceIoOpen */
nop

move $a0, $v0		/* set the return value of the function for arg0 of the next function */

lui $a1, 0x08D2 	/* arg1 is 0x08D20000, load address of the binary file */
lui $a2, 1		/* arg2, read 0x10000 bytes from the file */
jal 0x088F5A50      /* sceIoRead */
move $s0, $a0		/* backup $a0 value */

jal 0x088F5A60      /* sceIoClose */
move $a0, $s0		/* restore $a0 value */

lui $a0, 0x08D2
lui $a1, 0x1
jal 0x088F5C50		/* sceKernelDcacheWritebackInvalidateAll */
nop

nop
nop
li $a0, 0x08D20000
jr $a0
nop

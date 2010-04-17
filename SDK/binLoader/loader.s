/* by Wololo (adapted from Gripshift loader by Matiaz) */

.set noat
.set noreorder


nop


addiu $a0, $ra, 0x60	/* filename. If ou change this value, move it in the savegame */
nop
li $a1, 1
li $a2, 31
jal 0x08A885C8		/* sceIoOpen */
nop

move $a0, $v0		/* set the return value of the function for arg0 of the next function */

lui $a1, 0x08D2 	/* arg1 is 0x08D20000, load address of the binary file */
lui $a2, 1		/* arg2, read 0x10000 bytes from the file */
jal 0x08A88578		/* sceIoRead */
nop

jal 0x08A88590		/* sceIoClose */
nop

lui $a0, 0x08D2
lui $a1, 0x1
jal 0x08A887C0		/* sceKernelDcacheWritebackInvalidateAll */
nop

nop
nop
li $a0, 0x08D20000
jr $a0
nop


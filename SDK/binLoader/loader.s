# by 173210 and Wololo (adapted from Gripshift loader by Matiaz)
 
.set noat
.set noreorder
 
 
addiu $a0, $ra, 0x60	# filename. If you change this value, move it in the savegame
li $a1, 1
jal 0x08A885C8		# sceIoOpen
li $a2, 31
 
move $s0, $v0		# backup $v0 value
move $a0, $v0		# set the return value of the function for arg0 of the next function
lui $a1, 0x08D2 	# arg1 is 0x08D20000, load address of the binary file
jal 0x08A88578		# sceIoRead
lui $a2, 1		# arg2, read 0x10000 bytes from the file
 
jal 0x08A88590		# sceIoClose
move $a0, $s0		# restore $v0 value
 
j 0x08A887C0		# sceKernelDcacheWritebackInvalidateAll
lui $ra, 0x08D2 	# return address is 0x08D20000, load address of the binary file

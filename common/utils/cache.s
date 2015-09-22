	.text
	.align	2
	.set	noat
	.set	nomips16
	.set	noreorder

	.globl	synci
	.ent	synci
	.type	synci, @function
synci:
	li	$at, 0xFFFFFFC0
	and	$a0, $a0, $at
loop:
	addiu	$a0, $a0, 64
	cache	0x1A, -64($a0)
	sltu	$at, $a0, $a1
	bnez	$at, loop
	cache	0x8, -64($a0)

	jr	$ra
	nop

	.end	synci
	.size	synci, .-synci

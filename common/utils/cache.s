	.text
	.align	2
	.set	nomips16
	.set	noreorder

	.globl	hblIcacheFillRange
	.ent	hblIcacheFillRange
	.type	hblIcacheFillRange, @function
hblIcacheFillRange:
	addiu	$a0, $a0, 64
	sltu	$a2, $a0, $a1
	bnez	$a2, hblIcacheFillRange
	cache	0x14, -64($a0)

	jr	$ra
	nop

	.end	hblIcacheFillRange
	.size	hblIcacheFillRange, .-hblIcacheFillRange

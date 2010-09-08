	.file	1 "main.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"ms0:/umem.dump\000"
	.align	2
$LC1:
	.ascii	"ms0:/kmem.dump\000"
	.section	.text.start,"ax",@progbits
	.align	2
	.globl	_start
	.set	nomips16
	.ent	_start
_start:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$4,%hi($LC0)
	addiu	$sp,$sp,-8
	addiu	$4,$4,%lo($LC0)
	li	$5,514			# 0x202
	sw	$16,0($sp)
	sw	$31,4($sp)
	jal	sceIoOpen
	li	$6,511			# 0x1ff

	bltz	$2,$L2
	move	$16,$2

	move	$4,$2
	li	$5,142606336			# 0x8800000
	jal	sceIoWrite
	li	$6,25165824			# 0x1800000

	jal	sceIoClose
	move	$4,$16

$L2:
	lui	$4,%hi($LC1)
	addiu	$4,$4,%lo($LC1)
	li	$5,514			# 0x202
	jal	sceIoOpen
	li	$6,511			# 0x1ff

	bltz	$2,$L3
	move	$16,$2

	move	$4,$2
	li	$5,134217728			# 0x8000000
	jal	sceIoWrite
	li	$6,4194304			# 0x400000

	jal	sceIoClose
	move	$4,$16

$L3:
	li	$4,65536			# 0x10000
	jal	sceKernelDelayThread
	ori	$4,$4,0x86a0

	lw	$31,4($sp)
	lw	$16,0($sp)
	j	sceKernelExitGame
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	_start
	.size	_start, .-_start
	.ident	"GCC: (GNU) 4.3.3"

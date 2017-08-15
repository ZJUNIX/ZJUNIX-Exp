.globl exception
.globl start
.extern init_kernel

.set noreorder
.set noat
.align 2

exception:
	
.org 0x1000
start:
	lui $sp, 0x8100
	la $gp, _gp
	j init_kernel
	nop

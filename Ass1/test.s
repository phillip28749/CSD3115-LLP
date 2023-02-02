	.file	"test.c"
	.text
	.globl	arith
	.type	arith, @function
arith:
.LFB23:
	.cfi_startproc
	endbr64
	leaq	(%rdi,%rsi), %rax
	addq	%rdx, %rax
	leaq	(%rsi,%rsi,2), %rdx
	salq	$4, %rdx
	leaq	4(%rdi,%rdx), %rdx
	imulq	%rdx, %rax
	ret
	.cfi_endproc
.LFE23:
	.size	arith, .-arith
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:

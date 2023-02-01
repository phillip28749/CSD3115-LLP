	.globl sq_root_compute_array
	.globl sq_root_compute_varargs

	.text
sq_root_compute_array:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $48, %rsp
        movl    %edi, -36(%rbp)
        movq    %rsi, -48(%rbp)
        movl    $0, -16(%rbp)
        movl    -16(%rbp), %eax
        movl    %eax, %esi
        movl    $format, %edi
        call    printf
        movq    %rbp, %rsp
        popq    %rbp
        ret



sq_root_compute_varargs:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $120, %rsp
        movl    %edi, -108(%rbp)
        movq    %rsi, -48(%rbp)
        movq    %rdx, -40(%rbp)
        movq    %rcx, -32(%rbp)
        movq    %r8, -24(%rbp)
        movq    %r9, -16(%rbp)
        movl    %eax, %esi
        movl    $format, %edi
        call    printf
        movq    %rbp, %rsp
        popq    %rbp		
        ret

	.data
format:
        .asciz "Square root of %u is %u \n"


	.globl sq_root_compute_array
	.globl sq_root_compute_varargs

	.text
#sq_root_compute_array:
#        pushq   %rbp
#        movq    %rsp, %rbp
#        subq    $48, %rsp
#        movl    %edi, -36(%rbp)
#        movq    %rsi, -48(%rbp)
#        movl    $0, -16(%rbp)
#        movl    -16(%rbp), %eax
#        movl    %eax, %esi
#        movl    $format, %edi
#        call    printf
#        movq    %rbp, %rsp
#        popq    %rbp
#        ret
                     
sq_root_compute:
	subq	$8, %rsp			# allocate 8 byte of the stack		
	movl	%edi, %edx			# store val in edi register
	movl	%edi, %ecx			# store val in ecx register
	movl	$1, %eax			# i = 1	
	movl	$0, %r8d			# root = 0
.loop: 
	cmpl	%eax, %ecx			# compare i and val
	jb	.print					# jump if i > val
	addl	$1, %r8d			# ++root
	subl	%eax, %ecx			# val -= i
	addl	$2, %eax			# i + 2
	cmpl	%eax, %ecx			# val - (i + 2)
	jnb	.loop					# if(i+2 > val)
.print: 
	movl	%r8d, %ecx			# moving the value stored in root to %ecx.
	leaq	format, %rsi		# load format string into %rsi
	movl	$1, %edi			# mov 1 to %edi		
	movl	$0, %eax			# mov 0 to %eax
	call	__printf_chk@PLT	# calling printf
	addq	$8, %rsp			# Adding 8 to the stack pointer %rsp.
	ret

sq_root_compute_array:
	pushq	%r12				# Push r12 onto the stack for safekeeping
	pushq	%rbp				# Push rbp onto the stack for safekeeping
	pushq	%rbx				# Push rbx onto the stack for safekeeping
	movl	%edi, %ebp			# Move the value of num_of_elements into ebp
	movq	%rsi, %r12			# Move the pointer to the first element of the array into r12
	movl	$0, %ebx			# i = 0
	jmp	.arrdone
.arrloop:
	movslq	%ebx, %rax			# Move the value of ebx into rax
	movl	(%r12,%rax,4), %edi	# Move the value of the current array element into edi
	call	sq_root_compute		# Call the sq_root_compute function
	addl	$1, %ebx			# ++i
.arrdone:
	cmpl	%ebp, %ebx			# Compare the value of ebp to ebx
	jl	.arrloop				# If ebx is less than ebp, jump to label L8
	popq	%rbx				# Pop the value of rbx from the stack
	popq	%rbp				# Pop the value of rbp from the stack
	popq	%r12				# Pop the value of r12 from the stack
	ret



#sq_root_compute_varargs:
#        pushq   %rbp
#        movq    %rsp, %rbp
#        subq    $120, %rsp
#        movl    %edi, -108(%rbp)
#        movq    %rsi, -48(%rbp)
#        movq    %rdx, -40(%rbp)
#        movq    %rcx, -32(%rbp)
#        movq    %r8, -24(%rbp)
#        movq    %r9, -16(%rbp)
#        movl    %eax, %esi
#        movl    $format, %edi
#        call    printf
#        movq    %rbp, %rsp
#        popq    %rbp		
#        ret
#
#	.data

sq_root_compute_varargs:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $120, %rsp
        # move 6 registers stored args onto stack
        movl    %edi, -56(%rbp)
        movq    %rsi, -48(%rbp)
        movq    %rdx, -40(%rbp)
        movq    %rcx, -32(%rbp)
        movq    %r8, -24(%rbp)
        movq    %r9, -16(%rbp)
		xorl %ecx, %ecx
		xorl %edx, %edx
		movl $1, %edx
        movl    -56(%rbp,%rcx,8),%edi     # v
		cmpl $0, %edi
		jne .varargs_inner_loop
		
.varargs_outer_loop:
        incl    %ecx            # increase arg counter
        xorq    %rax,%rax       # root = 0
        movl    $1,%edx         # sub = 1
        movl    -56(%rbp,%rcx,8),%edi           # v
        cmpl    $6, %ecx                        # >= 6 registers stored arg
        cmovge  -32(%rbp, %rcx, 8), %edi   
        testl   %edi,%edi       
        jnz     .varargs_inner_loop
		movq    %rbp, %rsp
        popq    %rbp		
        ret    
.varargs_inner_loop:
        subl    %edx, %edi      # v -= sub
        jb      .varargs_print
        addl    $2, %edx        # sub += 2
        incl    %eax            # ++root
        jmp     .varargs_inner_loop
.varargs_print:
        pushq   %rcx                    # store 
        pushq   %rax                    # store rax
        subq    $8,%rsp                 # align stack for func call
        movl    -56(%rbp,%rcx,8),%esi   # v
        cmpl    $6, %ecx                # >= 6 registers stored arg
        cmovge  -32(%rbp, %rcx, 8), %esi      
        movl    %eax,%edx               # printf second agr
        movl    $format, %edi           # printf format
        xorq     %rax, %rax             # RAX = 0 since no vector registers used for calling printf
        call    printf
        # restore caller registers
        addq    $8,%rsp                 # restore rsp
        popq    %rax                    
        popq    %rcx                     
        jmp     .varargs_outer_loop

		
        .data
format:
        .asciz "Square root of %u is %u \n"


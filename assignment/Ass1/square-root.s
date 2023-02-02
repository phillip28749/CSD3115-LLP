/* Start Header

*****************************************************************/

/*!

\file square-root.s

\author Chen Yen Hsun, c.yenhsun, 2002761

\par email: c.yenhsun@digipen.edu

\date Jan 29, 2023

\brief
Computing nearest sqaure roots using assembly code

Copyright (C) 2023 DigiPen Institute of Technology.

Reproduction or disclosure of this file or its contents without the

prior written consent of DigiPen Institute of Technology is prohibited.

*/

/* End Header

*******************************************************************/
	.globl sq_root_compute_array
	.globl sq_root_compute_varargs

	.text
            
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

sq_root_compute_varargs:
        # Save the base pointer
        pushq   %rbp
        movq    %rsp, %rbp
        
        # Allocate stack space
        subq    $120, %rsp
        
        # Store the 6 register stored arguments onto the stack
        movl    %edi, -56(%rbp)
        movq    %rsi, -48(%rbp)
        movq    %rdx, -40(%rbp)
        movq    %rcx, -32(%rbp)
        movq    %r8, -24(%rbp)
        movq    %r9, -16(%rbp)
        
        # Initialize the argument counter and sub value
        xorq    %rcx,%rcx       # arg counter = 0
        movl    $1,%edx         # sub = 1
        
        # Load the first argument into EDI
        movl    -56(%rbp,%rcx,8),%edi     # v
        testl   %edi,%edi       
        
        # If EDI is not zero, jump to inner loop
        jnz     .sq_root_compute_varargs_inner_loop

.sq_root_compute_varargs_outer_loop:
        # Increase the argument counter
        incl    %ecx            # increase arg counter
        
        # Initialize the root value
        xorq    %rax,%rax       # root = 0
        movl    $1,%edx         # sub = 1
        
        # Load the next argument into EDI
        movl    -56(%rbp,%rcx,8),%edi           # v
        
        # If the argument counter >= 6, use the argument from the stack
        cmpl    $6, %ecx                        # >= 6 registers stored arg
        cmovge  -32(%rbp, %rcx, 8), %edi   
        
        # Test if EDI is not zero
        testl   %edi,%edi       
        
        # If EDI is not zero, jump to inner loop
        jnz     .sq_root_compute_varargs_inner_loop
		
		# No more args to process
		# Restore the stack pointer
		movq    %rbp, %rsp
        popq    %rbp		
        
        # Return
        ret    

.sq_root_compute_varargs_inner_loop:
        # Decrement EDI by sub value
        subl    %edx, %edi      # v -= sub
        
        # If EDI is negative, jump to print result
        jb      .sq_root_compute_varargs_print
        
        # Increment sub value by 2
        addl    $2, %edx        # sub += 2
        
        # Increment root value
        incl    %eax            # ++root
        
        # Jump back to inner loop
        jmp     .sq_root_compute_varargs_inner_loop

.sq_root_compute_varargs_print:

		# Store RCX and RAX
        pushq   %rcx
        pushq   %rax
        subq    $8, %rsp
        movl    -56(%rbp, %rcx, 8), %esi
        cmpl    $6, %ecx
        jge     .read_from_stack
        movl    -56(%rbp, %rcx, 8), %esi
        jmp     .call_printf
.read_from_stack:
        movl    -32(%rbp, %rcx, 8), %esi
.call_printf:

		#Pass in two variable and print the statement
        movl    %eax, %edx
        movl    $format, %edi
        xorq    %rax, %rax
        call    printf
        addq    $8, %rsp
        popq    %rax
        popq    %rcx
        jmp     .sq_root_compute_varargs_outer_loop
.no_more_args:

		#Return if no more argument
        movq    %rbp, %rsp
        popq    %rbp
        ret

		
        .data
format:
        .asciz "Square root of %u is %u \n"


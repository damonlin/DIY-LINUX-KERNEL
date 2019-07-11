
# the stack after trap occurred 
# --------------------------------------------------------------
# ↓ ss                          :  by CPU
# ↓ esp                         :  by CPU
# ↓ eflag                       :  by CPU
# ↓ cs                          :  by CPU
# ↓ eip                         :  by CPU
# ↓ handler address             :  by (no_)error_code
# ↓ ebx                         :  by (no_)error_code
# ↓ ecx                         :  by (no_)error_code       
# ↓ edx                         :  by (no_)error_code
# ↓ edi                         :  by (no_)error_code
# ↓ esi                         :  by (no_)error_code
# ↓ ebp                         :  by (no_)error_code
# ↓ ds                          :  by (no_)error_code
# ↓ es                          :  by (no_)error_code
# ↓ fs                          :  by (no_)error_code
# ↓ error code                  :  by (no_)error_code
# ↓ esp                         :  by (no_)error_code, point to eip

.global divide_error,nmi,invalid_op,coprocessor_segment_overrun,reserved
.global invalid_TSS,segment_not_present,stack_segment,general_protection

divide_error:
	pushl   $do_divide_error

no_error_code:
        xchgl 	%eax,(%esp)			# &handler <-> %eax
	pushl 	%ebx
	pushl   %ecx
	pushl   %edx
	pushl   %edi
	pushl   %esi
	pushl   %ebp
	push    %ds
	push    %es
	push    %fs

	pushl   $0x0			        # pass as parameter 2 : error code
	lea     44(%esp),%edx		        # get the address of EIP on stack
	pushl   %edx				# pass as parameter

	movl    $0x10,%edx                      # Now we are at Kernel, set to Kernel DS
	mov     %dx,%ds
       
	call    *%eax				# call handler
	addl    $8,%esp                         # pop error code and the address of EIP on stack

	pop     %fs
	pop     %es
	pop     %ds
	popl    %ebp
	popl    %esi
        popl    %edi
	popl    %edx
	popl    %ecx
	popl    %ebx
	popl    %eax
	iret

nmi:
	pushl 	$do_nmi
	jmp 	no_error_code

overflow:
	pushl 	$do_overflow
	jmp 	no_error_code

invalid_op:
	pushl 	$do_invalid_op
	jmp 	no_error_code

coprocessor_segment_overrun:
	pushl 	$do_coprocessor_segment_overrun
	jmp 	no_error_code

reserved:
	pushl 	$do_reserved
	jmp 	no_error_code

error_code:
        xchgl 	%eax,4(%esp)			# error code <-> %eax
	xchgl 	%ebx,(%esp)			# &handler <-> %ebx
	pushl   %ecx
	pushl   %edx
	pushl   %edi
	pushl   %esi
	pushl   %ebp
	push    %ds
	push    %es
	push    %fs

	pushl   %eax			        # pass as parameter 2 : error code
	lea     44(%esp),%eax		        # get the address of EIP on stack
	pushl   %eax				# pass as parameter 1

	movl    $0x10,%eax                      # Now we are at Kernel, set to Kernel DS
	mov     %ax,%ds
       
	call    *%ebx				# call handler
	addl    $8,%esp                         # pop error code and the address of EIP on stack

	pop     %fs
	pop     %es
	pop     %ds
	popl    %ebp
	popl    %esi
        popl    %edi
	popl    %edx
	popl    %ecx
	popl    %ebx
	popl    %eax
	iret

invalid_TSS:
	pushl 	$do_invalid_TSS
	jmp 	error_code

segment_not_present:
	pushl 	$do_segment_not_present
	jmp 	error_code

stack_segment:
	pushl 	$do_stack_segment
	jmp 	error_code

general_protection:
	pushl 	$do_general_protection
	jmp 	error_code
              
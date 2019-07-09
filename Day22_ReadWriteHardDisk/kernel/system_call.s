.global system_call, sys_fork
.global hd_interrupt

.align 2
bad_sys_call:
	movl $-1,%eax
	iret
.align 2
system_call:
	cmpl 	$nr_system_calls-1,%eax
	ja 	bad_sys_call
        push    %ds                     # 進入中斷需要保留 user 的 DS, CS 已經指向內核了
        push    %es
        push    %fs      
        pushl   %edx                    # parameter 3
        pushl   %ecx                    # parameter 2
        pushl   %ebx                    # parameter 1

        movl    $0x10, %edx             # 然後設定內核的 DS, 因為 ISR 在內核
        mov     %dx, %ds

        call    *sys_call_table(,%eax,4)

        popl    %ebx
        popl    %ecx
        popl    %edx
        pop     %fs
	pop     %es
	pop     %ds    
        iret

.align 2
sys_fork:	
        call    find_empty_process
        testl   %eax,%eax               # set SF to 1 if eax < 0 (negative)
	js      1f                      # jump if SF == 1
        
        push    %gs
	pushl   %esi
	pushl   %edi
	pushl   %ebp
	pushl   %eax

        call    copy_process

        addl    $20,%esp
1:	ret

hd_interrupt:
	push    %ds                     # 進入中斷需要保留 user 的 DS, CS 已經指向內核了
        push    %es
        push    %fs      
        pushl   %edx                    
        pushl   %ecx                    
        pushl   %ebx                    
	
	movl 	$0x10,%eax		# 然後設定內核的 DS, 因為 ISR 在內核
	mov 	%ax,%ds
	
	movb 	$0x20,%al
	outb 	%al,$0xA0		# EOI to interrupt controller #1
	jmp 	1f			# give port chance to breathe
1:	jmp 	1f
1:	xorl 	%edx,%edx
	xchgl 	do_hd,%edx
	testl 	%edx,%edx
	jne 	1f
	movl 	$unexpected_hd_interrupt,%edx
1:	outb 	%al,$0x20
	call 	*%edx		        # "interesting" way of handling intr.

	popl    %ebx
        popl    %ecx
        popl    %edx
        pop     %fs
	pop     %es
	pop     %ds    
        iret

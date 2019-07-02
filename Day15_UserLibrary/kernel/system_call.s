.global system_call

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

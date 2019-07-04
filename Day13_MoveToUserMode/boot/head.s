.code32
.global start
.global timer_interrupt, idt, gdt

.text
start:
	# 設定 Data segment selector(0x10)
        mov	$0x10, %ax
        mov	%ax, %ds
        
        # load stack_start into SS:ESP
        lss     stack_start, %esp

        # Setup IDT for default's handler
        call    setup_idt

        # setup GDT again, because the old GDT will be replaced !!!
        call    setup_gdt

        # reload segments for safety after resetting GDT
        mov	$0x10, %ax
        mov	%ax, %ds        
        lss     stack_start, %esp

        call    main  
        jmp     .

setup_idt:   
	lea     default_int_handler,%edx
	movl    $0x00080000,%eax
	movw    %dx,%ax		        # selector = 0x0008 = cs
	movw    $0x8E00,%dx	        # interrupt gate - dpl=0, present

	lea     idt,%edi
	mov     $256,%ecx
rp_sidt:
	movl    %eax,(%edi)
	movl    %edx,4(%edi)
	addl    $8,%edi
	dec     %ecx
	jne     rp_sidt
	lidt    idt_descr
	ret

setup_gdt:
	lgdt    gdt_descr
	ret

.align 2
default_int_handler:
        push    %ds                    # Save user's DS
        pushl   %eax
        movl    $0x10, %eax            # Now we are at Kernel, set to Kernel DS
        mov     %eax, %ds

        # shwo message
        pushl   $default_interrupt_msg
        call    printk
        popl    %eax

        popl    %eax
        pop     %ds
        iret

default_interrupt_msg:
	.asciz "Interrupt Called !!!"

.align 2
timer_interrupt:
        push    %ds                    # Save user's DS
        pushl   %eax
        movl    $0x10, %eax            # Now we are at Kernel, set to Kernel DS
        mov     %eax, %ds

        # send EOI to reopen interrupt
        movb    $0x20, %al
        outb    %al, $0x20

        call    do_timer      

        popl    %eax
        pop     %ds   
        iret

        # interrupt descriptor :
        # 47 ---------------------- 16  15 ---------0
        # |    base address          |   |   limit  |
        # -------------------------------------------
idt_descr:
	.word   256*8-1		        # limit : idt contains 256 entries, each interrupt occupies 8 bytes
	.long   idt                     # base address

        # interrupt entry :
        # 31 ---------------------- 16  15 ---------------------- 0
        # |    handler address       |   |   flag                 |
        # 31 ---------------------- 16  15 ---------------------  0
        # |    segment selector      |   |   handler address      |
        # ---------------------------------------------------------
idt:
                                        # command : .fill repeat , size , value
        .fill   256,8,0                 # 256 interupt, fill in with zero

        # GDT descriptor :
        # 47 ---------------------- 16  15 ---------0
        # |    base address          |   |   limit  |
        # -------------------------------------------
gdt_descr:
	.word 256*8-1
	.long gdt

        # GDT/LDT entry :
        # 31 ----------------24 23------------8 7--------------0
        # |    base address   | | flag        | | base address |
        # 31 -----------------------16 15----------------------0
        # |    base address          | |   limit               |
        # ------------------------------------------------------
gdt:	
        .quad   0x0000000000000000	# NULL descriptor
	.quad   0x00c09a0000000fff	# 16Mb (CS)
	.quad   0x00c0920000000fff	# 16Mb (DS, SS) 
	.quad   0x0000000000000000	# TEMPORARY - don't use
	.fill   252,8,0			# space for LDT's and TSS's etc
        

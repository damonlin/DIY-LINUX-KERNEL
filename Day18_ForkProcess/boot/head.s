.code32
.global start
.global timer_interrupt, idt, gdt, pg_dir

.text
pg_dir: # Page Dir(1024 entry * 4B = 4KB) will be here(at 0x00), and replace the code
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
        mov     %ax, %es                # must set es properly, it'll be used in setup_paging           
        lss     stack_start, %esp

        jmp	after_page_tables

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

.org 0x1000				# first page table at 0x1000(1024 entry * 4B = 4KB = 0x1000 bytes)
pg0:

.org 0x2000				# second page table at 0x2000(1024 entry * 4B = 4KB = 0x1000 bytes)
pg1:

.org 0x3000				# third page table at 0x3000(1024 entry * 4B = 4KB = 0x1000 bytes)
pg2:

.org 0x4000				# forth page table at 0x4000(1024 entry * 4B = 4KB = 0x1000 bytes)
pg3:

.org 0x5000

after_page_tables:	
	pushl 	$L6			# return address for main, if it decides to.
	pushl 	$main                   # return address for setup_paging
	jmp 	setup_paging
L6:
	jmp     L6

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

        call    do_timer      

        # send EOI to reopen interrupt
        movb    $0x20, %al
        outb    %al, $0x20

        popl    %eax
        pop     %ds   
        iret

.align 2
setup_paging:	
	movl    $1024*5,%ecx		# 5 pages - pg_dir + 4 page tables
	xorl    %eax,%eax
	xorl    %edi,%edi		# pg_dir is at 0x0000
	cld                   

        # first fill in Page Directory for first 4 entry
	movl    $pg0+7,pg_dir		# set present bit/user r/w
	movl    $pg1+7,pg_dir+4		# --------- " " ---------
	movl    $pg2+7,pg_dir+8		# --------- " " ---------
	movl    $pg3+7,pg_dir+12	# --------- " " ---------

        # now start to fill in first 4 page table, save EAX to  ES:EDI
	movl    $pg3+4092,%edi
	movl    $0xfff007,%eax		# 16Mb - 4096 + 7 (r/w user,p)
	std
1:	stosl			        # fill pages backwards - more efficient :-)
	subl    $0x1000,%eax
	jge     1b
	cld
	xorl    %eax,%eax		# pg_dir is at 0x0000
	movl    %eax,%cr3		# cr3 - page directory start
	movl    %cr0,%eax
	orl     $0x80000000,%eax
	movl    %eax,%cr0		# set paging (PG) bit	
        ret

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
        

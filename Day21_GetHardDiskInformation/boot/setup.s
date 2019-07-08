.code16
.global start
.text	

.equ INITSEG, 0x9000
.equ SETUPSEG, 0x9020

	ljmp 	$SETUPSEG, $start
start:

	# Read cursor and save to 0x90000, must be done before moving system
	# address 0x00 still contain BIOS now
	mov     $INITSEG, %ax
	mov     %ax, %ds
	mov     $0x03, %ah
	xor     %bh, %bh
	int     $0x10
	mov	$0x0, %bx
	mov     %dx, %ds:(%bx) 	# DH = Row(0x90001), DL = Column(0x90000)

	# Get hd0 data(ds:si) and save to 0x90080(es:di)
	mov	$0x0000, %ax
	mov	%ax, %ds
	lds	%ds:4*0x41, %si	# get int 0x41's value(= hd0 parameter)
	mov	$INITSEG, %ax
	mov	%ax, %es
	mov	$0x0080, %di	# es:di ==> destination(0x90080)
	mov	$0x10, %cx
	rep
	movsb

	mov 	%cs, %ax
	mov 	%ax, %ds	
	
	cli

    	mov     $0x0000, %ax
    	cld

	# start to load kernel to 0x0000 because we don't need functions of BIOS	
do_move:			
	mov     %ax, %es    	# es:di ==> destination
	add     $0x1000, %ax
	cmpw    $0x9000, %ax
	jz      end_move 
	mov     %ax, %ds
	sub     %di, %di
	sub     %si, %si
	mov     $0x8000, %cx
	rep
	movsw
	jmp     do_move

end_move:

	# Reset DS after moving
	mov %cs, %ax
	mov %ax, %ds

	# Enable A20 line
	xor     %ax,   %ax
	in      $0x92, %al
	or      $2,    %al
	out    	%al,   $0x92
		
	# Load the GDT base address and limit from memory into the GDTR register
	lgdt   	gdt_48

	# Enable protect mode
	movl    %cr0, %eax
	orl     $1,   %eax
	movl    %eax, %cr0
	
	# 已經進入保護模式了, 所以 CPU 會依據 GDT 表來運作
	# 0x08 表示 GDT 表中的 index 1, 也就是 GDT_DESC_CODE32
	# 0b0000 0000 0000 1000, bit0=0, bit1=0, bit2=0, bit3~15 = 1 (index = 1)
	ljmp    $0x08, $0

gdt:
GDT_DESC_DUMMY:					# dummy
	.word	0,0,0,0		

GDT_DESC_CODE32:				# for 32bit code
	.word	0x07FF				# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000				# base address=0
	.word	0x9A00				# code read/exec
	.word	0x00C0				# granularity=4096, 386

GDT_DESC_DATA:					# for data
	.word	0xFFFF				# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000				# base address=0x0000
	.word	0x9200				# data read/write
	.word	0x00C0				# granularity=4096, 386

gdt_48:	
	.word	0x800				# gdt limit=2048, 256 GDT entries
	.word   512+gdt, 0x9			# gdt base = 0X9xxxx, 

.code16
.global start
.text

.equ SEUPSEG, 0x9020

.macro InitSegDescriptor OFFSET GDT_SEG_ADDR
	xor     %ax, %ax
	mov     %cs, %ax
	shl     $4, %eax
	addl    $(\OFFSET), %eax
	movw    %ax, (\GDT_SEG_ADDR + 2)
	shr     $16, %eax
	movb    %al, (\GDT_SEG_ADDR + 4)
	movb    %ah, (\GDT_SEG_ADDR + 7)
.endm

	ljmp 	$SEUPSEG, $start
start:
	mov 	%cs, %ax
	mov 	%ax, %ds	

	# 必須先將 32 位元程式碼的起始位址填入 GDT_DESC_CODE32 的 base address
	# 這樣到執行 ljmp $0x08, $0, 才會真正從 PE_CODE32 開始執行
	InitSegDescriptor PE_CODE32, GDT_DESC_CODE32
	
	cli

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

# protected-mode OS code, here we just print some message
PE_CODE32:
.code32		
	mov     $0x10, %ax			# Load Video segment selector
	mov     %ax, %gs

	movb   	$0xC, %ah   			# 0000: Black Back 1100: Red Front
	xor    	%esi, %esi
	xor    	%edi, %edi
	movl   	$(os_msg), %esi
	movl   	$((80 * 10 + 0) * 2), %edi
	cld


# Display a string from %esi(string offset) to %edi(video segment).
DisplayOneByte: 
	lodsb 					# Load a byte from source 
	test    %al, %al
	jz      DisplayEnd
	mov     %ax, %gs:(%edi)
	add     $2, %edi
	jmp     DisplayOneByte

DisplayEnd:
    	# Stop here, infinite loop.
    	jmp     .

os_msg: .asciz "Enter Protected Mode!"

gdt:
GDT_DESC_DUMMY:					# dummy
	.word	0,0,0,0		

GDT_DESC_CODE32:				# for 32bit code
	.word	0x07FF				# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000				# base address=0
	.word	0x9A00				# code read/exec
	.word	0x00C0				# granularity=4096, 386

GDT_DESC_VIDEO:					# for video ram
	.word	0xFFFF				# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x8000				# base address=0xB8000, we fill 16bit(0x8000) LSB first
	.word	0x920B				# data read/write, and we we fill 4bit(0xB) LSB later
	.word	0x00C0				# granularity=4096, 386

gdt_48:	
	.word	0x800				# gdt limit=2048, 256 GDT entries
	.word   512+gdt, 0x9			# gdt base = 0X9xxxx, 

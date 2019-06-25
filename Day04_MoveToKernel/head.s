.code32
.globl start
.text

start:
	# setup Data segment selector(0x10)
        mov	$0x10, %ax
        mov	%ax, %ds
        mov	%ax, %ss   

show_msg:
        # setup Video segment selector(0x18)
        mov     $0x18, %ax
        mov     %ax, %gs

        movb    $0xC, %ah   # 0000: Black Back 1100: Red Front
        xor     %esi, %esi
        xor     %edi, %edi
        movl    $(kerenl_msg), %esi
        movl    $((80 * 10 + 0) * 2), %edi
        cld     # Clear DF flag.


# Display a string from %esi(string offset) to %edi(video segment).
DisplayOneByte: 
        lodsb # Load a byte from source 
        test    %al, %al
        jz      DisplayEnd
        mov     %ax, %gs:(%edi)
        add     $2, %edi
        jmp     DisplayOneByte

DisplayEnd:
        # Stop here, infinite loop.
        jmp . 

kerenl_msg: .asciz "Enter Kernel!"

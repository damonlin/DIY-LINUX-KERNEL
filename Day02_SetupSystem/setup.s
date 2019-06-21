.code16
.global start
.text

.equ SYSSEG, 0x9020

	ljmp $SYSSEG, $start
start:
        # Print message
	mov	$0x03, %ah		# read cursor pos
	xor	%bh, %bh
	int	$0x10
	
	mov	$21, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)	
	mov     $msg, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10	

msg:    .byte 13,10,13,10
	.ascii "Setting up system !!!"        

fin:
    hlt
    jmp     fin
    
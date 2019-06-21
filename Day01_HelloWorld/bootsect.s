.code16
.global start
.text
.equ BOOTSEG, 0x07c0		# original address of boot-sector

	ljmp    $BOOTSEG, $start
start:
	
	mov	%cs, %ax
	mov	%ax, %ds
	mov	%ax, %es

# Print message
	mov	$0x03, %ah		# read cursor pos
	xor	%bh, %bh
	int	$0x10
	
	mov	$24, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)	
	mov     $msg, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

	

msg:
	.ascii "Hello World !!!"

	.org 510
boot_flag:
	.word 0xAA55

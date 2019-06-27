.code16
.global start
.text

.equ BOOTSEG, 0x07c0			# original address of boot-sector
.equ SETUPSEG, 0x9020			# setup starts here
.equ SETUPSIZE, 4			# nr of setup-sectors
.equ KERNELSEG, 0x1000			# system loaded at 0x10000 (65536).
.equ KERNELSIZE, 0x3000
.equ ENDSEG, KERNELSEG + KERNELSIZE	# where to stop loading

	ljmp    $BOOTSEG, $start
start:
	
	mov	%cs, %ax		# 目前程式碼和資料都放在同段
	mov	%ax, %ds		# 所以設置 ds 和 es 跟 cs 一樣
	mov	%ax, %es

load_setup:                         	# 將 setup 讀到 SETUPSEG(0x9020) 裡
	mov     $0000, %dx          	# dh: 磁頭0, dl: 0 軟碟
	mov     $0002, %cx          	# ch: 柱面0, cl: 扇區2(setup 位於扇區2)

	mov     $SETUPSEG, %ax        
	mov     %ax, %es            	# 設置讀入緩衝區位置( es:bx )
	xor     %bx, %bx

	mov     $0x02, %ah          	# ah: 讀取扇區功能
	mov     $SETUPSIZE, %al        	# al: 讀取扇區的數量
	int     $0x13	
	jnc     ok_load             	# 讀取失敗, 會設置 CF 為 1, 所以判斷 CF 旗標	

	jmp     load_setup

ok_load:       

	# Get disk drive parameters, specifically number of sectors and track
	mov	$0x00, %dl
	mov	$0x0800, %ax		# AH=8 is get drive parameters
	int	$0x13
	mov	$0x00, %ch
	mov	%cx, %cs:sectors+0	# store number of sectors in %cs:sectors
	 
	# start to load kernel to $KERNELSEG(0x10000) temporarily
	# Because we still need functions of BIOS
	mov	$KERNELSEG, %ax
	mov	%ax, %es
	call	load_kernel

    	ljmp    $SETUPSEG,$0x0000	# 跳到 Setup 的程式碼, 進行 system setup

load_kernel:				# 將 kernel 讀到 KERNELSEG(0x10000) 裡
	mov	%es, %ax
	test	$0x0fff, %ax
die:	jne 	die			# es must be at 64kB boundary
	xor 	%bx, %bx		# bx is starting address within segment
rp_read:
	mov 	%es, %ax
 	cmp 	$ENDSEG, %ax		# have we loaded all yet?
	jb	ok1_read
	ret
ok1_read:	
	mov	%cs:sectors+0, %ax
	sub	sread, %ax
	mov	%ax, %cx
	shl	$9, %cx
	add	%bx, %cx
	jnc 	ok2_read
	je 	ok2_read
	xor 	%ax, %ax
	sub 	%bx, %ax
	shr 	$9, %ax
ok2_read:
	call 	read_track
	mov 	%ax, %cx
	add 	sread, %ax
	cmp 	%cs:sectors+0, %ax
	jne 	ok3_read
	mov 	$1, %ax
	sub 	head, %ax
	jne 	ok4_read
	incw    track 
ok4_read:
	mov	%ax, head
	xor	%ax, %ax
ok3_read:
	mov	%ax, sread
	shl	$9, %cx
	add	%cx, %bx
	jnc	rp_read
	mov	%es, %ax
	add	$0x1000, %ax
	mov	%ax, %es
	xor	%bx, %bx
	jmp	rp_read

read_track:
	push	%ax
	push	%bx
	push	%cx
	push	%dx
	mov	track, %dx
	mov	sread, %cx
	inc	%cx
	mov	%dl, %ch
	mov	head, %dx
	mov	%dl, %dh
	mov	$0, %dl
	and	$0x0100, %dx
	mov	$2, %ah
	int	$0x13
	jc	bad_rt
	pop	%dx
	pop	%cx
	pop	%bx
	pop	%ax
	ret
bad_rt:	mov	$0, %ax
	mov	$0, %dx
	int	$0x13
	pop	%dx
	pop	%cx
	pop	%bx
	pop	%ax
	jmp	read_track


sread:	.word 1+ SETUPSIZE		# sectors read of current track
head:	.word 0				# current head
track:	.word 0				# current track

sectors:
	.word 0

disksizes:	 
	.byte 36, 18, 15, 9

	.org 510
boot_flag:
	.word 0xAA55

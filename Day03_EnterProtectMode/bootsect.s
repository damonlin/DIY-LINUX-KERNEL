.code16
.global start
.text

.equ BOOTSEG, 0x07c0		   	# original address of boot-sector
.equ SETUPSEG, 0x9020
.equ SETUPLEN,4

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
	mov     $SETUPLEN, %al        	# al: 讀取扇區的數量
	int     $0x13
	mov     $BootMessage, %si   	# si 指向字串
	jnc     ok_load             	# 讀取失敗, 會設置 CF 為 1, 所以判斷 CF 旗標	

	jmp     load_setup

ok_load:                            

	# Print message			    
	movb    (%si), %al          	# 這裡讀取每一個字元來輸出, 這樣就不用特別計算字串的長度
	cmpb    $0, %al
	je      fin
	
	movb    $0x0e, %ah  	    	# ah = 0x0e 輸出一個字元 , al = 要輸出的字元
	int     $0x10
	
	add     $1, %si     	    	# 指向下一個字元
	jmp     ok_load

fin:
    	ljmp     $SETUPSEG,$0x0000   	# 跳到 Setup 的程式碼, 進行 system setup

BootMessage: 
	.asciz "Start to Load System!!"  # string end with '\0'

	.org 510
boot_flag:
	.word 0xAA55

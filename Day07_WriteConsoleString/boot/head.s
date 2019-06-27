.code32
.global start

.text
start:
	# 設定 Data segment selector(0x10)
        mov	$0x10, %ax
        mov	%ax, %ds
        
        # load stack_start into SS:ESP
        lss     stack_start, %esp

        call    main 
        jmp .

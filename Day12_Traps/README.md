# 第十二天

當 CPU 執行時遇到異常時，便會產生 Trap，把目前正在執行的程序暫停，並開始執行異常處理程序。
我們先設置幾個常見的異常，並且顯示相對應的資訊，方便除錯。

## 程式流程

1. asm.s : 

* 實做常見的異常處理函式，如溢位、除零等等。

* 當異常發生時，CPU 會自動地依序將當下的 SS、ESP、EFlag、CS、EIP 值放到堆疊中
```
the stack after trap occurred 
---------------
↓ ss           
↓ esp          
↓ eflag        
↓ cs           
↓ eip          
```

* 因此可以將 EIP 在 Stack 的位置當參數傳給其他函式，如 `die()`可以取得暫存器值。
```
pushl   %eax          # pass as parameter 2 : error code
lea     44(%esp),%edx # get the address of EIP on stack
pushl   %edx          # pass as parameter 1
```

* 有些異常發生時，CPU還會多放異常碼到堆疊上。
```
the stack after trap occurred 
---------------
↓ ss           
↓ esp          
↓ eflag        
↓ cs           
↓ eip          
↓ error code   
```

兩者主要差別是多了從 Stack 取得錯誤碼的步驟，因此需要分開處理。
```
no_error_code:
        xchgl 	%eax,(%esp)			# &handler <-> %eax
```
```
error_code:
        xchgl 	%eax,4(%esp)			# error code <-> %eax
	xchgl 	%ebx,(%esp)			# &handler <-> %ebx
```

2. trap.c : 

* 註冊異常處理函式。
```
trap_init()
```

* 顯示當下異常的暫存器資訊。
```
die(char * str,long esp_ptr,long nr)
```

3. main.c :
* 簡單的測試除零異常。
```
int a = 1, b = 0;
a = a/b;
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

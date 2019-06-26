# 第六天

因為不論是呼叫函式或是傳遞參數都須透過 Stack, 在增加新功能之前, 先設定 Kernel Stack.

## 程式流程

1. main.c : 配置 kernel_stack 共 4096 Bytes 來當作 Kernel Stack.
* Stack 的增加是由高位置往下生長, 因此 esp 是指向 kernel_stack 的尾端 
```
&kernel_stack[1024]
```
* Stack 位於 data segment, 所以 SS 設置 0x10

2. head.s : 透過 lss 指令, 載入到 SS:ESP
```
lss        stack_start
```

## 測試

1. head.s : 把 0x1 和 0x2 依序 push 到 Stack 裡
```
push    $1
push    $2
```

1. main.c : 在 main(int a, int b) 裡將參數取出, 並且判斷參數是否正確並顯示字串.
>注意：參數的取得是由右到左依序從 Stack 取出, 因此 b 是 0x1 而且 a 是 0x2
```
if( a == 2 && b == 1)
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

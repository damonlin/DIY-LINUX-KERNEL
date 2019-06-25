# 第六天
===

因為不論是呼叫函式或是傳遞參數都須透過 Stack, 在增加新功能之前, 先設定 Kernel Stack.

## 程式流程

1. main.c : 配置 kernel_stack 共 4096 Bytes 來當作 Kernel Stack.
> * Stack 的增加是由高位置往下生長, 因此 esp 是指向 kernel_stack 的尾端 *** &kernel_stack[1024] ***
>
> * Stack 位於 data segment, 所以 SS 設置 0x10

2. head.s : 透過 *** lss stack_start  ***, 載入到 SS:ESP

## 編譯
```bash
make
```

## 執行
```bash
make start
```

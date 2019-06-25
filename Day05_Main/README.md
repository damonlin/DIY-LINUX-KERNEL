# 第五天
===

終於可以使用 C 語言了. 新增 main() 來實做 Kernel 功能.
並且整理一下程式目錄, 把開機有關的程式碼移到 boot/ 底下, main.c 則放在 init/
同時修改 Makefile, 將產生的 kernel image 放到 system/

## 程式流程

1. head.s : 透過 *** call main ***, 來與 C 語言 main() 連結
2. main.c : 在 main() 裡呼叫 show_msg() 來印出字串

## 編譯
```bash
make
```

## 執行
```bash
make start
```

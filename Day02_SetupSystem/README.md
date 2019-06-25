# 第二天
===

新增 setup.s 檔案, 設定系統功能如保護模式GDT, IDT 等功能都會在這裡實現,現在只是印出字串

1. setup.s: 印出字串
2. Makefile:
    
    * ***@dd if=setup seek=1 bs=512 count=4***： 將 setup 執行檔複製到 Image 第二個磁區開始(第一磁區是 bootsect), 佔了四個磁區大小    

## 程式流程
BIOS 載入開機磁區後, bootsect 會先將 setup 的程式碼搬到 0x90200 記憶體位址上, 這樣做的意義是避免覆蓋到 BIOS 中斷
因為在設定系統仍然需要使用到 BIOS 中斷. 完成搬送就執行 `"jmp $SYSSEG,$0x0000"` 跳轉到 setup 繼續執行.

## 編譯
```bash
make
```

## 執行
```bash
make start
```

#第一天
===

開發作業系統的第一天, 先寫一個簡單的 hello world, 因為作業系統一開始什麼功能都沒有, 只能利用 BIOS 的功能來印出 hello world 字串包含兩個檔案, 分別是 bootsect.s 和 Makefile

1. bootsect.s: 組合語言用的是 AT&T 的格式
2. Makefile:

    * `"LDFLAGS	+= -m elf_i386"` ： 由於是在 64 位元下開發 32 位元的OS, 因此編譯和連結需要加入 32 位元的參數
    * `"dd if=bootsect bs=512 count=1 of=Image"`： 製作 512 Bytes 的開機映像檔, 產生 Image
    * `"objcopy"`： Linker 會把一些資訊加到執行檔,這對於自製 OS 沒有意義, 需要移除

## X86 啟動流程
BIOS 會將可啟動設備(這裡是軟碟)的第一個扇區(大小是 512 bytes)讀入記憶體位址 0x7C00 處
接著就開始從 0x7C00 執行, 因此第一隻程式就要寫在第一扇區裡面,可以參考 
[為什麼在 x86，MBR 會被載入到 0x7C00](https://gist.github.com/letoh/2790559)

## 編譯
```bash
make
```

## 執行
``` bash
make start
```

# 第四天
===

新增 Kernel 程式碼, head.s 代表 Kernel 的開頭設定


## 程式流程

1. bootsect.s : 暫時先從磁碟裡將 setup 讀到 0x90200 記憶體位址, 接著在讀入 kernel 到 0x10000
                之所以不直接讀到記憶體 0x00 位址, 是因為還需要使用到 BIOS 的功能, 不能覆蓋 BIOS

2. setup.s : 因為已經用不到 BIOS, 可以將 kernel 搬到記憶體 0x00 位址, 接著設定保護模式, 然後跳到 kernel

3. head.s : 簡單的印出字串, 證明已經進入 kernel

4. Makefile:
    
    * ***@dd if=kernel seek=5 bs=512 count=2883 of=Image 2>&1 >/dev/null***： 將 kernel 執行檔複製到 Image 第六個磁區開始

## 編譯
```bash
make
```

## 執行
```bash
make start
```

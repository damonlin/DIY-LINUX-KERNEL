# 第三天
===

今天來設定系統的保護模式. 因為保護模式需要依靠 GDT 來運作, 所以設置了臨時的 GDT.
若成功進入保護模式, 即可看到字串的顯示   

## 程式流程

首先設置臨時 GDT 表, 包含了 32位元程式碼 segment 和 VIDEO segment.

進入保護模式流程：
1. 開啟 A20 Line
2. 利用 **lgdt**, 將 GDT 的起始位址載入至 GDTR 暫存器
3. 設定 **cr0** 暫存器的 bit 0 以便進入保護模式
4. 進入 32位元程式碼, 利用 Video segment selector(0x10) 來印出字串

## 編譯
```bash
make
```

## 執行
```bash
make start
```

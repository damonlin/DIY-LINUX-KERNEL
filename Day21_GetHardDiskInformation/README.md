# 第二十一天

作業系統重要的功能之一就是讀取硬碟上的檔案，一開始先來實做讀取硬碟的資訊。

## 程式流程

1. setup.s : 新增系統呼叫 `sys_setup`，用來讀取和設定硬碟

* 設定資料來源位址。第一個硬碟參數表的地址在 BIOS 中斷 0x41。第二個硬碟參數表在 0x46，現在只讀取第一個硬碟。 
```
mov	$0x0000, %ax
mov	%ax, %ds
lds	%ds:4*0x41, %si
```
> 只能在 `setup.s`取得硬碟資訊，因為之後，BIOS 將會被覆蓋掉。

* 設定目標位址 0x90080。
```
mov	$INITSEG, %ax
mov	%ax, %es
mov	$0x0080, %di
```

* 參數一共有 16 Bytes。
```
mov	$0x10, %cx
```

* 然後將參數拷貝到記憶體位址 0x90080，供以後使用。
```
rep
movsb
```

2. hd.c :

* 新增系統呼叫，來取得並顯示硬碟的資訊。
```
sys_setup
```

3. Makefile :

* 新增測試用的硬碟映像檔。
```
HDA_IMG = ../root.img
```

```
@qemu-system-x86_64 -m 16M -boot a -fda Image -hda $(HDA_IMG)
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

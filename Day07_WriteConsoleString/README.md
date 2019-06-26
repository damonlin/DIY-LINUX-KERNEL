# 第七天

為了方便變除錯, 需要實做一個可以輸出字串的功能

## 程式流程

1. setup.s :  
> * 透過 BIOS int 0x10 來讀取目前游標位址, 並且紀錄在記憶體位址 0x90000.
>
> * 移除 GDT 的 Video Segment, 因為不再使用.

2. console.c : 

* 讀取目前游標位址(0x90000), 再開始顯示字串, 否則會覆蓋到原有的字串.
```
#define ORIG_X (*(unsigned char *)0x90000)
#define ORIG_Y (*(unsigned char *)0x90001)
```

* Video RAM 是從記憶體位址 0xB8000 開始.
```
video_mem_start = 0xB8000; 
```
* 透過寫入 Video RAM 來顯示字串.
```
*((unsigned short*)video_mem_start + index) = c | (attr << 8); 
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

# 第八天

字串顯示感覺怪怪的. 原來是游標的位置沒有跟著移動. 接著來實做游標的移動吧！

## 程式流程

1. io.h :

* 對硬體 IO Port 讀寫, 提供了四種函式
```
outb() , inb() , outb_p() , inb_p()
```
其中 outb_p() , inb_p() 利用了 jmp 指令來做時間延遲的效果.

2. console.c : 

* 根據更新後的 x 與 y 位址, 寫入 VGA 暫存器, 來達到做游標移動的效果.
```
static unsigned short   video_port_reg	= 0x3d4;
static unsigned short	video_port_val	= 0x3d5;
```

```
set_cursor()
```

> 詳細設定可以參考 [VGA Hardware](https://wiki.osdev.org/VGA_Hardware)

## 編譯
```bash
make
```

## 執行
```bash
make start
```

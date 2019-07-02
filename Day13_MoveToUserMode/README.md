# 第十三天

Kernel 在初始化之後，便會切換到 User Mode（任務 0），也就是從特級權 0 到特級權 3。     

## 程式流程

1. head.s :

* 因為在 setup.s 裡舊的 GDT 所在的記憶體位置會被 Kernel 所覆蓋掉，所以需要重新載入 GDT。
```
call    setup_gdt
```

2. sched.h : 

* 宣告和設定 Task 0 的相關數值。
```
#define INIT_TASK
```

2. sched.c : 

* 設置 Task 0。
```
void sched_init()
```
* 並在新的 GDT 裡加入 Task 0 的 TSS 和 LDT 的描述符。
```
set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));
```
* 計算 TSS 和 LDT 在 GDT 裡的位址，並載入到對應的暫存器。
```
ltr(0);
lldt(0);
```

|15    ~    4|3 |2  ~ 0|
  |---|----|----|
  |index |GDT/LDT|RPL|

```
當使用 segment selector
- 0x08、0x10： CPU 會判斷 bit 3，這是屬於 GDT，從 GDTR 取得 GDT 位址，並且根據 index 來得到對應的段。
- 0x0f、0x17： CPU 會判斷 bit 3，這是屬於 LDT，從 LDTR 取得 LDT 在 GDT 裡的偏移位址，然後得到 LDT 的位址。
               再根據 index 來得到對應的段。
```

3. main.c :

* 進入 User Mode，利用 `iret` 來模擬中斷呼叫返回原進程，將特級權從 0 切換到 3。然後 CPU 就會從 Task 0 開始執行。
```
move_to_user_mode();
```

## 測試
* 當進入 User Mode 之後，將無法使用 printk()，因為要顯示的字串是位於 User Mode。
所以使用 printk() 產生錯誤代表進入了 User Mode。

## 編譯
```bash
make
```

## 執行
```bash
make start
```

# 第十一天

實做 Timer，其週期是 10 毫秒。另外設置 jiffies 變數，每產生時間中斷，就增加一，可以用來確定當前時間。

## 程式流程

1. head.s : 

* 實做 Timer 中斷處理函式。
```
timer_interrupt()
```

2. sched.c : 

* 初始化 Timer 晶片(8254)，並且註冊 Timer 中斷。
```
timer_init()
```

3. main.c : 

* 因為在 setup.s 裡已經關閉中斷，因此需要重新打開中斷，Timer 才會生效。
```
sti()
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

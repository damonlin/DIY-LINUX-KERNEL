# 第十九天

實做簡單的排程器，讓兩個進程得到執行。

## 程式流程

1. sched.c :

* 排程器主要依靠 `timer` 來執行。
```
void do_timer(){
        schedule();
}
```

* 因為目前只有兩個進程，所以簡單的使用變數來改變要執行的進程。
```
int pid = !current->pid;
```

* 選出要執行的進程，就跳去執行它。
```
switch_to(pid);
```

2. sched.h :

* `switch_to` 實做切換進程的功能，參數是要切換的 `pid`。
```
#define switch_to(n)
```

* 首先比較目前在執行的進程，和要切換的進程(保存在`ecx`)是否相同。
```
cmpl %%ecx,current
```

* 若是切換到不同的進程，就將新進程的 `TSS selector`存入 __tmp.b 中。
```
movw %%dx,%1
```

* 接著交換新舊進程。現在 `current` 指向新的進程。
```
xchgl %%ecx,current
```

* 執行長跳轉到 `*&__tmp`,也就是 `jmp TSS selcetor:offset`。
造成進程切換。TR 的內容也會在進程切換時，自動地改變被改變。
```
ljmp %0
```

## 測試

* 結果成功顯示 `This is child process` 和 `This is parent process`。

## 編譯
```bash
make
```

## 執行
```bash
make start
```

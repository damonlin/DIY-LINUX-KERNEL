# 第十八天

實做系統呼叫 fork()，用來建立新的進程。


## 程式流程

1. system_call.s :

* 定義 fork 系統呼叫
```
sys_fork:
```

* 找到適合 pid
```
call    find_empty_process
```

* 放對應的暫存器的直到堆疊中當 `copy_process` 的參數。其他如 ss、esp等暫存器，在發生系統呼叫時已經先被放堆疊中了。
```
push    %gs
pushl   %es
pushl   %edi
pushl   %ebp
pushl   %eax
```

2. fork.c :

* 利用 fork 來建立進程，其實是複製原來進程的代碼和資料。
```
copy_process()
```

* 先配置一個空閒頁來存放新進程的資料。
```
p = (struct task_struct *) get_free_page();
```

* 註冊新進程到進程表。
```
task[nr] = p;
```

* 接著複製原進程的資料到新進程。
```
*p = *current;
```
> 注意！為了簡化，我們暫時複製 LDT，代表新進程可以操作的記憶體範圍與原進程是相同的(0~640K)。
而 0 ~ 640K 所屬的 page table 已經先被設定好了，所以不會造成 page fault。
之後會修改個別獨立的 LDT！

* 接著根據原進程環境設置新進程的任務狀態段(TSS)中個暫存器。

* 每個進程都有其自己的 kernel stack，每當進程透過中斷，進入到 `kernel mode`，會切換到該 stack 並使用。
新進程的 kernel esp 被設置為進程結構的頂端，kernel ss 則設置為 kernel DS(0x10)。
```
p->tss.esp0 = PAGE_SIZE + (long) p;   
p->tss.ss0 = 0x10;
```

3. sys.h :

* 將 `sys_fork` 註冊到系統呼叫表格。
```
fn_ptr sys_call_table[] = {sys_write, sys_fork};
```

* 另外定義 `mem_map`，來對頁做管理，紀錄是否使用或是共享。
```
static unsigned char mem_map [ PAGING_PAGES ] = {0,};
```

4. main.c :

* 建立新的進程
```
if(!(pid=fork()))
```

## 測試

* 理論上應該會列印出 `This is child process`，但卻只有`This is parent process`。
原來是沒有切換到新進程去執行。接著將會實做一個簡單的排程器，來切換新舊進程。


## 編譯
```bash
make
```

## 執行
```bash
make start
```

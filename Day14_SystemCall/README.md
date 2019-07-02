# 第十四天

既然不能直接使用 printk()來顯示字串，那有其他辦法嗎？
答案是使用 system call。當 User mode 底下要執行 Kernel 的程式碼，必須要透過 system call。

## 程式流程

1. sys.h :

* 實做 sys_write()，並且填入 `sys_call_table`。
```
fn_ptr sys_call_table[] = {sys_write};
```

2. system_call.s :

* 實做 system_call()。在 `sys_call_table` 裡找尋適當的系統呼叫函式，並且執行。
```
call    *sys_call_table(,%eax,4)
```

3. sched.c : 

* 註冊 system_call() 到中斷編號 `0x80`。
```
set_system_gate(0x80, &system_call);
```

4. main.c :

* 呼叫 system call 的編號 0，也就是 sys_write()。
```
__asm__ ("int $0x80"::"a" (0));
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

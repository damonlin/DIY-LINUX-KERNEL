# 第二十天

改進排程器，支援多個進程。

## 程式流程

1. sched.c :

* 修改 `schedule()`，從進程表中依序檢查 task NR_TASKS-1 到 task 1, 選出最大時間片段(counter)的 process, 然後執行。
```
while (--i) {
	if (!*--p)
		continue;			
	if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
		c = (*p)->counter, next = i;
	}
}
```

* 如果 c == -1, 代表沒有其他的 process, 就執行 process 0。
  如果 c > 0,代表已經選中某一個 process。
			
```
if (c) break;
```

* 如果 c == 0, 代表有其他 process, 但是他們都已執行完時間片段(counter), 需要重新計算 counter。
```
for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
	if (*p)
		(*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
```

* do_timer()：如果目前正在執行的進程，它的時間片段(counter) 大於零，代表系統給它的執行時間片段(counter)還沒結束，就繼續執行。
```
if ((--current->counter)>0) 
                return;
```

## 測試

* 結果成功顯示 `This is process2` 和 `This is process3`。

## 編譯
```bash
make
```

## 執行
```bash
make start
```

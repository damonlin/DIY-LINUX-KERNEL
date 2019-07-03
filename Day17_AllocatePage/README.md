# 第十七天

管理實體記憶體，包括配置、釋放或是共享。


## 程式流程

1. main.c :

* 配置記憶體。因為 kernel 的程式碼和資料都在 0~1MB 裡，1~3MB 用來做 cache，所以真正可以管理的記憶體從 4MB ~ 16MB。
```
mem_init(4*1024*1024, 16*1024*1024);
```

2. memory.c :

* 將可配置的記憶體依 PAGE_SIZE(4KB) 劃分，共有 `PAGING_PAGES` 個頁可以使用。
```
#define PAGING_PAGES    (PAGING_MEMORY>>12)
```

* 另外定義 `mem_map`，來對頁做管理，紀錄是否使用或是共享。
```
static unsigned char mem_map [ PAGING_PAGES ] = {0,};
```

* 當要配置新的頁，呼叫
```
get_free_page()
```

* 當要釋放頁，呼叫
```
free_page()
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

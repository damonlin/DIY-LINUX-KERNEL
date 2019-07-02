# 第十五天

既然系統呼叫的種類很多，需要設計一套系統呼叫的函式庫，來快速實做各種類型的系統呼叫。

## 程式流程

1. unistd.h :

* 提供了從參數 0 個到參數 3 個的巨集，來簡化實做系統呼叫。
```
#define _syscall0(type,name)
#define _syscall1(type,name,atype,a)
#define _syscall2(type,name,atype,a,btype,b)
#define _syscall3(type,name,atype,a,btype,b,ctype,c)
```

* 呼叫中斷 0x80。
```
__asm__ volatile ("int $0x80"
```

* 並且將參數傳進去。
```
"0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b)),"d" ((long)(c)));
```

2. 建立 lib 目錄 : 

* 目錄裡面專門放系統呼叫的函式庫，這裡先新增一個 write。

3. write.c : 

* 宣告 write() 函式，因為參數為 1 個，選擇 `_syscall1`。
```
_syscall1(int, write, const char *, buf)
```

* 
4. main.c :

* 使用函式庫來印出字串。
```
write("Entering User ...\n"); 
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

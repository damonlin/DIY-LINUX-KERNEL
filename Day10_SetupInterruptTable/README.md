# 第十天

新增中斷表. 透過中斷表, 將每個異常或是中斷與它的處理函式連結起來. 當發生異常或是中斷,
CPU 就會在中斷表裡尋找對應的處理函式, 並且執行.

## 程式流程

1. head.s : 

* 建立中斷表
```
idt:
        .fill    256,8,0
```

* 填入預設的中斷處理函式
```
call    setup_idt
```

2. main.c : 

* 測試中斷產生, 是否會成功執行處裡預設的處理函式, 並顯示顯示字串
```
__asm__ ("int $0x80");
```

## 編譯
```bash
make
```

## 執行
```bash
make start
```

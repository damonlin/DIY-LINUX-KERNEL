# 第九天

實做 printk. 新增可以格式化字串.

## 程式流程

1. vsprintf.c : 

* 根據參數 format 字串轉換並格式化數據

2. printk.c : 

* 主要用於 Kernel 輸出字串, 先利用 `vsprintf()` 格式化字串, 再交給 `con_write()` 輸出.

## 編譯
```bash
make
```

## 執行
```bash
make start
```

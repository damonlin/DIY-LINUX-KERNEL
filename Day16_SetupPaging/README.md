# 第十六天

進入記憶體管理的部份，首先先開啟分頁功能，並且設定 Kernel 的 Page table。從這裡開始，記憶體的讀寫都由分頁管理。

## 程式流程

1. head.s :

* 我們設計將 page directory 放在記憶體 0x0000 的位址，接著依序存放 4 個 page tables。
每一個 Page 大小為 4KB，一個 page table 可以有 1K 個 Pages，所以一個 page table 大小是 1MB(0x1000)。
目前實體記憶體為 16MB，所以需要配置 4 個 page tables，來直接對應實體記憶體。

```
pg_dir:

.org 0x1000
pg0:

.org 0x2000
pg1:

.org 0x3000
pg2:

.org 0x4000
pg3:

.org 0x5000
```

* 但如此一來位於 0x0000 ~ 0x5000 的程式碼會被覆蓋掉，因此需要把後續的程式碼放到 0x5000 並執行程式。
```
jmp	after_page_tables
```

* 開始設定 paging 功能。
```
setup_paging:
```

* 先設定位於 0x0000 的 page directory。也就是將前四個 page tables 的位址填入到 page directory，並設定相關屬性。
````
movl    $pg0+7,pg_dir
movl    $pg1+7,pg_dir+4	
movl    $pg2+7,pg_dir+8	
movl    $pg3+7,pg_dir+12
````

* 再來設定前四個 page tables。將實體記憶體 0 ～ 16MB，以 4KB 為單位，將記憶體位址依序填入到 page talbe。

* 最後設定 `cr0` 裡的 paging bit，整個分頁功能正式啟動。
```
orl     $0x80000000,%eax
movl    %eax,%cr0
```

> 還記得在 GDT 裡設定 Kerenl 的 segment 範圍為 16 MB，也就是 kernel 存取記憶體的範圍是 0 ~ 16 MB。
開啟分頁功能之後，因為已經設定好 16MB 的 page table，所以 kernel 在存取記憶體並不會產生 `page fault`。
因為記憶體位址都可以在 page table 裡尋找的到。

## 編譯
```bash
make
```

## 執行
```bash
make start
```

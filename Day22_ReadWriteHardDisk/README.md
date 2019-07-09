# 第二十二天

實做硬碟的讀寫功能。

對硬碟設備上的數據讀寫，是通過硬碟中斷處理函式來進行。

## 程式流程

1. system_call.s :

* 實做硬碟中斷處理函式。
```
hd_interrupt:
```

* `do_hd` 是函式指標，呼叫它來真正處理資料。它可以被替換讀取的中斷或是寫入中斷，增加實做彈性。
```
xchgl 	do_hd,%edx
call 	*%edx
```

2. hd.c :

* 註冊硬碟中斷處理函式。
```
set_intr_gate(0x2E,&hd_interrupt);
```

3. ll_rw_blk.c : 

* 處理低階的讀寫功能。 Kernel 可以根據設備編號(`dev`)、操作功能(`rw`)和欲讀寫邏輯塊(`blocknr`)的參數來建立請求。
把讀到資料放到 `buffer` 裡，或是將 `buffer` 的資料寫入。
```
void ll_rw_block(int dev, int rw, char* buffer, int blocknr)
```

* 硬碟的讀寫，必須等待一個請求完成，才能執行下一個請求。因此需要建立請求列表來依序處理硬碟的讀寫。
```
struct request request[NR_REQUEST];
```

* 當 Kernel 發出需求，就會建立一個請求並且加入請求列表。
```
static void make_request(int dev, int rw, char* buffer, int blocknr)
static void add_request(struct blk_dev_struct * dev, struct request * req)
```

* 這裡注意， Kernel 每次讀寫的數據以 1024 Bytes 為單位，而硬碟裝置則是以 512 Bytes 為單位。
因此實際上 Kernel 每次讀寫需要讀寫兩個磁區，才能完成。
```
req->nr_sectors = 2;
```

* 建立完請求，就真正對硬碟下指令。這也是函式指標，不同裝置有不同的處理方式，這裡是指向 `do_hd_request`。
```
(dev->request_fn)();
```

4. hd.c :

* 首先透過欲讀寫邏輯塊(`blocknr`)的編號，來換算真正在硬碟上的磁區。
```
__asm__("divl %4":"=a" (block),"=d" (sec):"0" (block),"1" (0),
		"r" (hd_info[dev].sect));
__asm__("divl %4":"=a" (cyl),"=d" (head):"0" (block),"1" (0),
		"r" (hd_info[dev].head));
```

* 根據指令，來下達寫入的硬碟動作。
```
if (CURRENT->cmd == WRITE)
```

* 前面提到的 `do_hd`，因為是寫的動作，因此會被取代為 `write_intr`。
```
hd_out(dev,nsect,sec,head,cyl,WIN_WRITE,&write_intr);
```

* 根據指令，來下達讀取的硬碟動作。
```
else if (CURRENT->cmd == READ)
```

* 前面提到的 `do_hd`，因為是讀的動作，因此會被取代為 `read_intr`。
當硬碟的讀寫動作做完，就會發出中斷，進而進入 `write_intr` 或是 `read_intr`。
```
hd_out(dev,nsect,sec,head,cyl,WIN_READ,&read_intr);
```

* 判斷是否還有其他的磁區需要處理
```
if (--CURRENT->nr_sectors)
```

* 接著就是結束掉請求。
```
end_request()
```

* 讀取硬碟的引導扇區，也就是磁區 0 的第 510 byte 和 511 byte。若讀出來的值是 0xAA55，代表該硬碟分區表是正常的。
```
ll_rw_block(0x300, READ, buffer, 0);
```
```
if (buffer[510] != 0x55 || (unsigned char) buffer[511] != 0xAA) 
```

## 測試

若是讀取成功，則顯示字串`Partition table ok.`

## 編譯
```bash
make
```

## 執行
```bash
make start
```

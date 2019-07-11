# 第二十三天

要是每次讀寫資料都直接存取硬碟，會造成效能低落。因此，需要一種快取機制，將曾經讀寫過的資料存放在記憶體 buffer 中，
下次需要使用的話，直接從記憶體 buffer 中存取，而不是從硬碟。

## 程式流程

1. main.c : 

* 在 link 過程中, linker 會使用變數來紀錄程序的每一個段的邏輯位址。因此在程序中可以通過訪問這幾個外部變數來獲取程序段的位址。
整個 kernel 鏈結完之後，會產生 `end` 的變數，代表代表 kernel 之後的第一個記憶體位址。
我們可以把它當作是 kernel 載入記憶體後的最後位址。也代表 buffer 的起始位址。
```
struct buffer_head * start_buffer = (struct buffer_head *) &end;
```

* 16MB 中的 `end ~ 4MB` 當作 cache buffer 使用
```
buffer_memory_end = 4*1024*1024; 
```

2. buffer.c : 

* 初始化 buffer。
```
void buffer_init(long buffer_end)
```

* 將 buffer 根據 `BLOCK_SIZE`(1024Bytes) 大小，劃分 `NR_BUFFERS` 個 buffer 塊。
並建立 `buffer_head` 結構來管理這些 buffer。
```
while ( (b -= BLOCK_SIZE) >= ((void *) (h+1)) ) 
```
>    start_buffer                                                          buffer_memory_end
  	      	|----|----|----|----|----|----|--------|--------|--------|
		| h1   h2   h3    .........       b3        b2      b1   |
		|----|----|----|----|----|----|--------|--------|--------|
  		 
* 將每一個 `buffer_head`把它們鍊結起來，形成 double linked list。
```
h->b_prev_free = h-1;
h->b_next_free = h+1;
```

* 雖然有了 list，但是要在 list 搜尋需要 O(n)，因此為了加快搜尋速度，另增加了 `hash_table`。
```
for (i=0;i<NR_HASH;i++)
	hash_table[i]=NULL;
```

* 透過裝置編號和磁區編號，就可以在 hash 新增、刪除和搜尋。
```
buffer_head * find_buffer(int dev, int block)
void insert_into_queues(struct buffer_head * bh)
void remove_from_queues(struct buffer_head * bh)
```

* `buffer_head` 結構用來管理讀寫的資料，包含裝置編號、讀寫的磁區、以及存放的資料。
```
struct buffer_head {
	char * b_data;			/* pointer to data block (1024 bytes) */
	unsigned long b_blocknr;	/* block number */
	unsigned short b_dev;
```

* 之後讀寫資料都是透過 `buffer_head` 來管理，所以新增以 `buffer_head` 結構的讀取函式，用來封裝底層的 `ll_rw_block()`。
```
struct buffer_head * bread(int dev,int block)
```

* 透過 `bread()` 讀取資料，會先尋找 buffer 是否已經存在。若是存在並且是最新狀態下，就直接回傳，而不需要硬碟的讀取。
```
if (!(bh=getblk(dev,block)))
	panic("bread: getblk returned NULL\n");
	        
if (bh->b_uptodate)
	return bh;
```

3. hd.c : 

* 透過 `bread()` 來讀取硬碟的引導扇區。
```
if (!(bh = bread(0x300,0))) 
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

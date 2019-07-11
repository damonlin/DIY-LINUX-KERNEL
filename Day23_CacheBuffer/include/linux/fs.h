#ifndef __FS_H__
#define __FS_H__

#define READ 0
#define WRITE 1

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff)

#define NR_HASH 307
#define BLOCK_SIZE 1024

struct buffer_head {
	char * b_data;			/* pointer to data block (1024 bytes) */
	unsigned long b_blocknr;	/* block number */
	unsigned short b_dev;		/* device (0 = free) */
	unsigned char b_uptodate;
	unsigned char b_dirt;		/* 0-clean,1-dirty */
	unsigned char b_count;		/* users using this block */
	unsigned char b_lock;		/* 0 - ok, 1 -locked */
	struct task_struct * b_wait;
	struct buffer_head * b_prev;
	struct buffer_head * b_next;
	struct buffer_head * b_prev_free;
	struct buffer_head * b_next_free;
};

extern void ll_rw_block(int rw, struct buffer_head * bh);
extern struct buffer_head * bread(int dev,int block);
extern void brelse(struct buffer_head * buf);

#endif
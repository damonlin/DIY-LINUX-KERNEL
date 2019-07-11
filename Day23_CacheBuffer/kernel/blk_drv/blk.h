#ifndef __BLK_H__
#define __BLK_H__

#include <linux/sched.h>

#define NR_BLK_DEV	7
#define NR_REQUEST	32

struct request {
	int dev;		             /* -1 if no request */
	int cmd;		         /* READ or WRITE */
	int errors;
	unsigned long sector;
	unsigned long nr_sectors;
	char * buffer;
	struct task_struct * waiting;
	struct buffer_head * bh;
	struct request * next;
	unsigned char b_lock;	/* 0 - ok, 1 -locked */
};

static inline void lock_request(struct request * req)
{
	cli();
	while (req->b_lock)
		sleep_on(&req->waiting);
	req->b_lock=1;
	sti();
}

static inline void unlock_request(struct request * req)
{
	if (!req->b_lock)
		printk("free buffer being unlocked\n");
	req->b_lock=0;
	wake_up(&req->waiting);
}

static inline void wait_on_request(struct request * req)
{
	cli();
	while (req->b_lock)
		sleep_on(&req->waiting);
	sti();
}

/*
 * This is used in the elevator algorithm: Note that
 * reads always go before writes. This is natural: reads
 * are much more time-critical than writes.
 */
#define IN_ORDER(s1,s2) \
((s1)->cmd<(s2)->cmd || (s1)->cmd==(s2)->cmd && \
((s1)->dev < (s2)->dev || ((s1)->dev == (s2)->dev && \
(s1)->sector < (s2)->sector)))

struct blk_dev_struct {
	void (*request_fn)(void);
	struct request * current_request;
};

extern struct blk_dev_struct blk_dev[NR_BLK_DEV];
extern struct request request[NR_REQUEST];
extern struct task_struct * wait_for_request;

#ifdef MAJOR_NR
#define DEVICE_NAME "harddisk"
#define DEVICE_INTR do_hd
#define DEVICE_REQUEST do_hd_request
#define DEVICE_NR(device) (MINOR(device)/5)
#define DEVICE_ON(device)
#define DEVICE_OFF(device)

#define CURRENT (blk_dev[MAJOR_NR].current_request)
#define CURRENT_DEV DEVICE_NR(CURRENT->dev)

#ifdef DEVICE_INTR
void (*DEVICE_INTR)(void) = NULL;
#endif

static inline void unlock_buffer(struct buffer_head * bh)
{
	if (!bh->b_lock)
		printk(DEVICE_NAME ": free buffer being unlocked\n");
	bh->b_lock=0;
	wake_up(&bh->b_wait);
}

extern inline void end_request(int uptodate)
{
	if (CURRENT->bh) 
	{		
		CURRENT->bh->b_uptodate = uptodate;
		unlock_buffer(CURRENT->bh);
	}

	wake_up(&CURRENT->waiting);
	wake_up(&wait_for_request);

	// free request
	CURRENT->dev = -1;
	CURRENT = CURRENT->next;
}

#define INIT_REQUEST \
repeat: \
	if (!CURRENT) \
		return; \
	if (MAJOR(CURRENT->dev) != MAJOR_NR) \
		panic(DEVICE_NAME ": request list destroyed");

#endif

#endif
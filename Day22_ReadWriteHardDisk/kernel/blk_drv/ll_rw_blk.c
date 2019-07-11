#include <linux/kernel.h>
#include <linux/sched.h>

#include "blk.h"  

struct request request[NR_REQUEST];

/*
 * used to wait on when there are no free requests
 */
struct task_struct * wait_for_request = NULL;

struct blk_dev_struct blk_dev[NR_BLK_DEV] = {
	{ NULL, NULL },		/* no_dev */
	{ NULL, NULL },		/* dev mem */
	{ NULL, NULL },		/* dev fd */
	{ NULL, NULL },		/* dev hd */
	{ NULL, NULL },		/* dev ttyx */
	{ NULL, NULL },		/* dev tty */
	{ NULL, NULL }		/* dev lp */
};

static void add_request(struct blk_dev_struct * dev, struct request * req)
{
	struct request * tmp;

	req->next = NULL;
	cli();	

	// No request now, execute it immediately !!
	if (!(tmp = dev->current_request)) {
		dev->current_request = req;
		sti();
		(dev->request_fn)();
		return;
	}

	// use elevator algorithm to insert
	for ( ; tmp->next ; tmp=tmp->next)
		if ((IN_ORDER(tmp,req) ||
		    !IN_ORDER(tmp,tmp->next)) &&
		    IN_ORDER(req,tmp->next))
			break;
	req->next=tmp->next;
	tmp->next=req;
	
	sti();
}

static void make_request(int dev, int rw, char* buffer, int blocknr)
{
        struct request * req;

repeat:
	/* we don't allow the write-requests to fill up the queue completely:
 	* we want some room for reads: they take precedence. The last third
 	* of the requests are only for reads.
 	*/
        if (rw == READ)
		req = request+NR_REQUEST;
	else
		req = request+((NR_REQUEST*2)/3);

	/* find an empty request */
	while (--req >= request)
		if (req->dev<0)
			break;

	/* if none found, sleep on new requests: check for rw_ahead */
	if (req < request) 
	{
		sleep_on(&wait_for_request);
		goto repeat;
	}

	lock_request(req);

        /* fill up the request-info, and add it to the queue */
	req->dev        = dev;
	req->cmd        = rw;
	req->errors     = 0;
	req->sector     = blocknr<<1;
	req->nr_sectors = 2;
	req->buffer     = buffer;
	req->waiting    = NULL;
	req->next       = NULL;

        add_request(MAJOR(dev)+blk_dev, req);
}


void ll_rw_block(int dev, int rw, char* buffer, int blocknr)
{        
        unsigned int major;

	if ((major=MAJOR(dev)) >= NR_BLK_DEV || !(blk_dev[major].request_fn)) {
		printk("Trying to read nonexistent block-device\n");
		return;
	}

        make_request(dev, rw, buffer, blocknr);
}

void blk_dev_init(void)
{
	int i;

	for (i=0 ; i<NR_REQUEST ; i++) {
		request[i].dev = -1;
		request[i].next = NULL;
	}
}
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <asm/io.h>

#define MAJOR_NR 3
#include "blk.h"

/* Max read/write errors/sector */
#define MAX_ERRORS	7
#define MAX_HD		2

void do_hd_request(void);
static void recal_intr(void);
static int recalibrate = 0;
static int reset = 0;

/*
 *  This struct defines the HD's and their types.
 */
struct hd_i_struct {
	int head,sect,cyl,wpcom,lzone,ctl;
	};

struct hd_i_struct hd_info[] = { {0,0,0,0,0,0},{0,0,0,0,0,0} };
static int NR_HD = 1;

static struct hd_struct {
	long start_sect;
	long nr_sects;
} hd[5*MAX_HD]={{0,0},};

#define port_read(port,buf,nr) \
__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr))	// outsw: write word = 2 Bytes

#define port_write(port,buf,nr) \
__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr))	// outsw: write word = 2 Bytes

extern void hd_interrupt(void);

/* This may be used only once, enforced by 'static int callable' */
int sys_setup(void * BIOS)
{
        static int callable = 1;
        int i,drive;

        if (!callable)
		return -1;
	callable = 0;

        for (drive = 0; drive < NR_HD ; drive++)
        {
		hd_info[drive].cyl = *(unsigned short *) BIOS;
		hd_info[drive].head = *(unsigned char *) (2+BIOS);
		hd_info[drive].wpcom = *(unsigned short *) (5+BIOS);
		hd_info[drive].ctl = *(unsigned char *) (8+BIOS);
		hd_info[drive].lzone = *(unsigned short *) (12+BIOS);
		hd_info[drive].sect = *(unsigned char *) (14+BIOS);
		BIOS += 16;
	}

	// hd[0] and hd[5] stand for overall information of HD1 and HD2
	// hd[1]~hd[4] and hd[5]~hd[8] stand for information of partiiton
	for (drive = 0 ; drive < NR_HD ; drive++) 
	{
		hd[i*5].start_sect = 0;
		hd[i*5].nr_sects = hd_info[i].head*
				   hd_info[i].sect*
				   hd_info[i].cyl;
	}
	
	//test
	{
		char buffer[1024] = {0};

		ll_rw_block(0x300, READ, buffer, 0);		
		wait_on_request(CURRENT);	// wait for HD interrupt, and wake it up

		if (buffer[510] != 0x55 || (unsigned char) buffer[511] != 0xAA) 
		{
			printk("Bad partition table\n");
			panic("");
		}
	}

	printk("Partition table ok.\n");
}

static int controller_ready(void)
{
	int retries=100000;

	while (--retries && (inb_p(HD_STATUS)&0x80));
	return (retries);
}

static void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
		unsigned int head,unsigned int cyl,unsigned int cmd,
		void (*intr_addr)(void))
{
	register int port asm("dx");

	if (drive>1 || head>15)
		panic("Trying to write bad sector");
	if (!controller_ready())
		panic("HD controller not ready");
	do_hd = intr_addr;
	outb_p(hd_info[drive].ctl,HD_CMD);
	port=HD_DATA;
	outb_p(hd_info[drive].wpcom>>2,++port);
	outb_p(nsect,++port);
	outb_p(sect,++port);
	outb_p(cyl,++port);
	outb_p(cyl>>8,++port);
	outb_p(0xA0|(drive<<4)|head,++port);
	outb(cmd,++port);
}

static int drive_busy(void)
{
	unsigned int i;

	for (i = 0; i < 10000; i++)
		if (READY_STAT == (inb_p(HD_STATUS) & (BUSY_STAT|READY_STAT)))
			break;
	i = inb(HD_STATUS);
	i &= BUSY_STAT | READY_STAT | SEEK_STAT;
	if (i == READY_STAT | SEEK_STAT)
		return(0);
	printk("HD controller times out\n");
	return(1);
}

static void reset_controller(void)
{
	int	i;

	outb(4,HD_CMD);
	for(i = 0; i < 100; i++) nop();
	outb(hd_info[0].ctl & 0x0f ,HD_CMD);
	if (drive_busy())
		printk("HD-controller still busy\n");
	if ((i = inb(HD_ERROR)) != 1)
		printk("HD-controller reset failed: %02x\n",i);
}

static void reset_hd(int nr)
{
	reset_controller();
	hd_out(nr,hd_info[nr].sect,hd_info[nr].sect,hd_info[nr].head-1,
		hd_info[nr].cyl,WIN_SPECIFY,&recal_intr);
}

void unexpected_hd_interrupt(void)
{
	printk("Unexpected HD interrupt\n");
}

static int win_result(void)
{
	int i=inb_p(HD_STATUS);

	if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT))
		== (READY_STAT | SEEK_STAT))
		return(0); /* ok */
	if (i&1) i=inb(HD_ERROR);
	return (1);
}

static void bad_rw_intr(void)
{
	if (++CURRENT->errors >= MAX_ERRORS)
		end_request(0);
	if (CURRENT->errors > MAX_ERRORS/2)
		reset = 1;
}

static void read_intr(void)
{		
	if(win_result())
	{
		bad_rw_intr();
		do_hd_request();
		return;
	}

	port_read(HD_DATA,CURRENT->buffer,256);
	CURRENT->errors = 0;
	CURRENT->buffer += 512;
	CURRENT->sector++;
	if (--CURRENT->nr_sectors) 
	{
		do_hd = &read_intr;
		return;
	}

	end_request(1);
	do_hd_request();
}

static void write_intr(void)
{			
	if (win_result()) 
	{
		bad_rw_intr();
		do_hd_request();
		return;
	}
	if (--CURRENT->nr_sectors) 
	{
		CURRENT->sector++;
		CURRENT->buffer += 512;
		do_hd = &write_intr;
		port_write(HD_DATA,CURRENT->buffer,256);
		return;
	}
	end_request(1);	
	do_hd_request();
}

static void recal_intr(void)
{
	if (win_result())
		bad_rw_intr();
	do_hd_request();
}

void do_hd_request(void)
{			
	int i,r = 0;
	unsigned int block,dev;
	unsigned int sec,head,cyl;
	unsigned int nsect;

	INIT_REQUEST

	// calculate head, cyl from block
	dev = MINOR(CURRENT->dev);
	block = CURRENT->sector;		
	if (dev >= 5*NR_HD || block+2 > hd[dev].nr_sects)
	{
		end_request(0);
		goto repeat;
	}

	dev /= 5;	
	__asm__("divl %4":"=a" (block),"=d" (sec):"0" (block),"1" (0),
		"r" (hd_info[dev].sect));
	__asm__("divl %4":"=a" (cyl),"=d" (head):"0" (block),"1" (0),
		"r" (hd_info[dev].head));
	sec++;	// sector's index is from 1, so add one
	nsect = CURRENT->nr_sectors;

	if (reset) 
	{
		reset = 0;
		recalibrate = 1;
		reset_hd(CURRENT_DEV);
		return;
	}
	if (recalibrate) 
	{
		recalibrate = 0;
		hd_out(dev,hd_info[CURRENT_DEV].sect,0,0,0,
			WIN_RESTORE,&recal_intr);
		return;
	}

	if (CURRENT->cmd == WRITE)
	{				
		hd_out(dev,nsect,sec,head,cyl,WIN_WRITE,&write_intr);
		for(i=0 ; i<3000 && !(r=inb_p(HD_STATUS)&DRQ_STAT) ; i++)
			/* nothing */ ;

		if (!r) 
		{
			bad_rw_intr();
			goto repeat;
		}
		port_write(HD_DATA,CURRENT->buffer,256);
	}
	else if (CURRENT->cmd == READ)
		hd_out(dev,nsect,sec,head,cyl,WIN_READ,&read_intr);
	else
		panic("unknown hd-command");
}

void hd_init(void)
{
	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;

	// setup ISR
	set_intr_gate(0x2E,&hd_interrupt);
	outb_p(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xA1)&0xbf,0xA1);
}
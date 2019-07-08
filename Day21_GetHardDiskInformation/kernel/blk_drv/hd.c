#include <linux/kernel.h>

/* Max read/write errors/sector */
#define MAX_ERRORS	7
#define MAX_HD		2

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

/* This may be used only once, enforced by 'static int callable' */
int sys_setup(void * BIOS)
{
        static int callable = 1;
        int drive;

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

        for (drive = 0; drive < NR_HD ; drive++)
        {
                printk("\nDisk info: \n");
                printk("Cylinders: %d\n",  hd_info[drive].cyl);
                printk("Heads:     %d\n",  hd_info[drive].head);
                printk("Sectors:   %d\n",  hd_info[drive].sect);		
	}	
}

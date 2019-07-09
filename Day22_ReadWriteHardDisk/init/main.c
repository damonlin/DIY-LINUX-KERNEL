#include <linux/kernel.h>
#include <asm/system.h>
#include <unistd.h>

static inline _syscall0(int,fork)
static inline _syscall1(int,setup,void *,BIOS)

extern void init();
extern void con_init();
extern void timer_init();
extern void trap_init();
extern void sched_init();
extern void mem_init(long, long);
extern void hd_init(void);
extern void blk_dev_init(void);

#define DRIVE_INFO (*(struct drive_info *)0x90080)
struct drive_info { char dummy[32]; } drive_info;

void main(void)
{	
	drive_info = DRIVE_INFO; 
	
	// physical memory 0 ~ 1MB for kernel's code and data, 1MB ~ 4MB for cache
	// 4MB ~ 16MB for allocating pages dynamically
        mem_init(4*1024*1024, 16*1024*1024);

	con_init();
	trap_init();
	blk_dev_init();
	timer_init();	
	sched_init();
	hd_init();
	
	printk("Entering kernel ...\n");

	// Enable interrupt
       	sti();

	move_to_user_mode();	

	if (!fork()) {
		init();               
	}

}

void init()
{
        setup((void *) &drive_info);
}

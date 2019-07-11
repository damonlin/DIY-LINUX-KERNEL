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
extern void buffer_init(long);

#define DRIVE_INFO (*(struct drive_info *)0x90080)
struct drive_info { char dummy[32]; } drive_info;

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

void main(void)
{	
	drive_info = DRIVE_INFO; 
	
	memory_end = 16*1024*1024;
        buffer_memory_end = 4*1024*1024;    
        main_memory_start = buffer_memory_end;

	// end ~ 4MB for cache
	// 4MB ~ 16MB for allocating pages dynamically
        mem_init(main_memory_start, memory_end);

	con_init();
	trap_init();
	blk_dev_init();
	timer_init();	
	sched_init();
	buffer_init(buffer_memory_end); 
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

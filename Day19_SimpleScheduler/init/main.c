#include <linux/kernel.h>
#include <asm/system.h>
#include <unistd.h>

static inline _syscall0(int,fork)

extern void con_init();
extern void timer_init();
extern void trap_init();
extern void sched_init();
extern void mem_init(long, long);

void main(void)
{	
	int pid;

	// 4MB ~ 16MB for allocating pages dynamically
        mem_init(4*1024*1024, 16*1024*1024);

	con_init();
	trap_init();
	timer_init();	
	sched_init();

	printk("Entering kernel ...\n");

	// Enable interrupt
       	sti();

	move_to_user_mode();	

	if(!(pid=fork()))
	{
		write("This is child process\n");
	}
	else
	{
		write("This is parent process\n");
	}
	
}

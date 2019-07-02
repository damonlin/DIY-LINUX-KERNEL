#include <linux/kernel.h>
#include <asm/system.h>

extern void con_init();
extern void timer_init();
extern void trap_init();
extern void sched_init();

void main(void)
{	
	con_init();
	trap_init();
	timer_init();	
	sched_init();

	printk("Entering kernel ...\n");

	// Enable interrupt
       	sti();

	move_to_user_mode();	

	// now in user mode, not in kernel, so we use system call to print message instead of printk !!	
        __asm__ ("int $0x80"::"a" (0));  // call system call write
}

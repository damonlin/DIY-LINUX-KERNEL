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
	printk("Entering User ...\n");
}

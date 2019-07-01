#include <linux/kernel.h>
#include <asm/system.h>

extern void con_init();
extern void timer_init();

void main(void)
{
	con_init();
	timer_init();
	
	printk("Entering kernel ...\n");

	// Enable interrupt
       	sti();
}
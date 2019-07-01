#include <linux/kernel.h>
#include <asm/system.h>

extern void con_init();
extern void timer_init();
extern void trap_init();

void main(void)
{	
	con_init();
	timer_init();
	trap_init();
	
	printk("Entering kernel ...\n");

	// Enable interrupt
       	sti();

	{
                int a = 1, b = 0;
                a = a/b;
        }
}
#include <linux/kernel.h>
#include <asm/system.h>

// for kernel's stack
long kernel_stack[1024] ;

struct {
	long * esp;	// esp
	short ss;	// ss in 0x10(data segment)
} stack_start = { &kernel_stack[1024] , 0x10 };

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
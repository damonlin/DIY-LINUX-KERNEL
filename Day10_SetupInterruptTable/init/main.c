#include <linux/kernel.h>

// for kernel's stack
long kernel_stack[1024] ;

struct {
	long * esp;	// esp
	short ss;	// ss in 0x10(data segment)
} stack_start = { &kernel_stack[1024] , 0x10 };

extern void con_init();

void main(void)
{
	con_init();
	
	printk("Entering kernel ...\n");
	
	__asm__ ("int $0x80");
}
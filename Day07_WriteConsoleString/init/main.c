
// for kernel's stack
long kernel_stack[1024] ;

struct {
	long * esp;	// esp
	short ss;	// ss in 0x10(data segment)
} stack_start = { &kernel_stack[1024] , 0x10 };

extern void con_init();
extern void con_write();

void main(void)
{
	con_init();

	con_write("Entering kernel ...\n");        
}
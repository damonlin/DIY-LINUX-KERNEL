
// for kernel's stack
long kernel_stack[1024] ;

struct {
	long * esp;	// esp
	short ss;	// ss in 0x10(data segment)
} stack_start = { &kernel_stack[1024] , 0x10 };

void show_msg();

void main(void)
{
        show_msg();        
}
#include <linux/kernel.h>
#include <asm/system.h>

void divide_error();
void nmi();	
void invalid_op();	
void coprocessor_segment_overrun();
void invalid_TSS();
void segment_not_present();
void stack_segment();
void general_protection();	
void reserved();

static void die(char * str,long esp_ptr,long nr)
{
	long * esp = (long *) esp_ptr;
	printk("EIP: %04x:%p\nEFLAGS:  %p\nESP:  %04x:%p\n",esp[1],     // CS
                                                            esp[0],     // EIP
                                                            esp[2],     // Eflag
                                                            esp[4],     // ESP
                                                            esp[3]);    // SS

        hlt();
}

void do_general_protection(long esp, long error_code)
{
	die("general protection",esp,error_code);
}

void do_divide_error(long esp, long error_code)
{
	die("divide error",esp,error_code);
}

void do_nmi(long esp, long error_code)
{
	die("nmi",esp,error_code);
}

void do_overflow(long esp, long error_code)
{
	die("overflow",esp,error_code);
}

void do_invalid_op(long esp, long error_code)
{
	die("invalid operand",esp,error_code);
}

void do_coprocessor_segment_overrun(long esp, long error_code)
{
	die("coprocessor segment overrun",esp,error_code);
}

void do_invalid_TSS(long esp,long error_code)
{
	die("invalid TSS",esp,error_code);
}

void do_segment_not_present(long esp,long error_code)
{
	die("segment not present",esp,error_code);
}

void do_stack_segment(long esp,long error_code)
{
	die("stack segment",esp,error_code);
}

void do_reserved(long esp, long error_code)
{
	die("reserved (15,17-47) error",esp,error_code);
}

void trap_init(void)
{
	int i;

        for (i=17;i<48;i++)
		set_trap_gate(i,&reserved);

	set_trap_gate(0,&divide_error);	
	set_trap_gate(2,&nmi);	
	set_trap_gate(6,&invalid_op);	
	set_trap_gate(9,&coprocessor_segment_overrun);
	set_trap_gate(10,&invalid_TSS);
	set_trap_gate(11,&segment_not_present);
	set_trap_gate(12,&stack_segment);
	set_trap_gate(13,&general_protection);	
	set_trap_gate(15,&reserved);	
		
}

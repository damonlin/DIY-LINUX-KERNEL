#include <linux/sched.h>
#include <asm/io.h>
#include <asm/system.h>

#define HZ 100
#define LATCH (1193180/HZ)      /* Get the PIT value: hardware clock at 1193180 Hz */

extern int timer_interrupt(void);

// for kernel's stack
long kernel_stack[PAGE_SIZE>>2] ;

struct {
	long * esp;	// esp
	short ss;	// ss in 0x10(data segment)
} stack_start = { &kernel_stack[PAGE_SIZE>>2] , 0x10 };

union task_union {
	struct task_struct task;
	char stack[PAGE_SIZE];      // Kerenl Stack of task
};

static union task_union init_task = {INIT_TASK,};
struct task_struct * task[NR_TASKS] = {&(init_task.task), };

long volatile jiffies = 0;

void do_timer()
{
        jiffies++;
}

static void init_8259A()
{
        // Remap the PIC
        outb(0x11, 0x20);
        outb(0x11, 0xA0);
        outb(0x20, 0x21);
        outb(0x28, 0xA1);
        outb(0x04, 0x21);
        outb(0x02, 0xA1);
        outb(0x01, 0x21);
        outb(0x01, 0xA1);
        outb(0xff, 0x21);
        outb(0xff, 0xA1);
}

void timer_init()
{
        init_8259A();
        
        // Send the command
        outb(0x36, 0x43);
        outb((LATCH & 0xFF), 0x40);
        outb(((LATCH >> 8) & 0xFF), 0x40);

        // Enable timer irq
        outb(inb_p(0x21)&~0x01, 0x21);

        // Install timer isr
        set_intr_gate(0x20, &timer_interrupt);
}

void sched_init()
{    
        // setup the task0 ldt and tss
        set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
	set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));

        // load tss and ldt
        ltr(0);
	lldt(0);
}

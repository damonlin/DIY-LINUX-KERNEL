#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/system.h>

#define HZ 100
#define LATCH (1193180/HZ)      /* Get the PIT value: hardware clock at 1193180 Hz */

extern int timer_interrupt(void);
extern int system_call(void);

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
struct task_struct *current = &(init_task.task);  // set task0 for current process

long volatile jiffies = 0; 

void schedule()
{					
	int i,next,c;
	struct task_struct ** p;

	while (1) {
		c = -1;
		next = 0;
		i = NR_TASKS;
		p = &task[NR_TASKS];
		while (--i) {
			if (!*--p)
				continue;
			// 檢查 task NR_TASKS-1 到 task 1, 選出最大 counter 的 task, 然後執行
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
				c = (*p)->counter, next = i;
		}
		// 如果 c == -1, 代表沒有其他的 task, 就執行 task0
		// 如果 c == 0, 代表有其他 task, 但是他們都已執行完畢, 需要重新計算 counter
		// 如果 c > 0,代表已經選中某一個 task
		if (c) break;		
		for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
			if (*p)
				(*p)->counter = ((*p)->counter >> 1) +
						(*p)->priority;
	}
	
	switch_to(next);
}

void sleep_on(struct task_struct **p)
{
	struct task_struct *pre_wait_task;

	if (!p)
		return;
	if (current == &(init_task.task))
		panic("task[0] trying to sleep");

	// Every task that tries to sleep will record pre-task
	// when waking up, we can use this to wake pre-task
	pre_wait_task = *p;

	// change waiting task list to myself for other process	
	*p = current;

	// reschedule
	current->state = TASK_UNINTERRUPTIBLE;
	schedule();

	// After waking up, wake up pre-task
	if (pre_wait_task)
		pre_wait_task->state = TASK_RUNNING;
}

void wake_up(struct task_struct **p)
{
	// It must wake up the last task of the waiting task list
	// And wake up from last to the first one by one
	if (p && *p) {
		(**p).state = TASK_RUNNING;
		*p=NULL;
	}
}

void do_timer()
{
        jiffies++;
	
        if ((--current->counter)>0) 
                return;
                
        schedule();
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

        // Register 0x80 system call, otherwise cause GP error
	set_system_gate(0x80, &system_call);
}

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/system.h>

long last_pid = 0;

int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)    
{        
        struct task_struct *p;    
        p = (struct task_struct *) get_free_page();
	if (!p)
		return -1;

	printk("call copy_process pid = %d, cs = 0x%X, ds = 0x%X, ss = 0x%X, esp = 0x%X\n", 
                    last_pid, cs, ds, ss, esp);

        task[nr] = p;	
        
	// NOTE! We share the same ldt, so the process that forked won't cause page fault
        *p = *current;
	
        p->state = TASK_UNINTERRUPTIBLE;
        p->pid = last_pid;
	p->counter = p->priority;
        p->tss.back_link = 0;
        p->tss.esp0 = PAGE_SIZE + (long) p;   
        p->tss.ss0 = 0x10;
        p->tss.eip = eip;
	p->tss.eflags = eflags;
	p->tss.eax = 0;			// Due to this, clild process will get pid 0
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = _LDT(nr);
	p->tss.trace_bitmap = 0x80000000;

        set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
	set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));

        p->state = TASK_RUNNING;
        return last_pid;
}

int find_empty_process(void)
{
	int i;

repeat:
	if ((++last_pid)<0) last_pid=1;
        	for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i])
			return i;
	return -1;
}
#ifndef __SYS_H__
#define __SYS_H__

typedef int (*fn_ptr)();

extern int sys_write();

fn_ptr sys_call_table[] = {sys_write};

/* So we don't have to do any more manual updating.... */
int nr_system_calls = 20;//sizeof(sys_call_table)/sizeof(fn_ptr);

#endif
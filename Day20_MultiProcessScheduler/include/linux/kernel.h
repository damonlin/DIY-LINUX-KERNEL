#ifndef _KERNEL_H_
#define _KERNEL_H_

extern int printk(const char *fmt, ...);
extern void panic(const char * s);

#endif
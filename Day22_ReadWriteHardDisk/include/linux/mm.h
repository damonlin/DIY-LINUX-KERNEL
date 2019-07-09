#ifndef __MM_H__
#define __MM_H__

#define PAGE_SIZE 4096

unsigned long get_free_page(void);
void free_page(unsigned long addr);

#endif
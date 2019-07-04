#include <linux/kernel.h>

#define PAGING_MEMORY   (15*1024*1024)
#define PAGING_PAGES    (PAGING_MEMORY>>12)
#define LOW_MEM         0x100000        // 1MB
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED            100

static unsigned char mem_map [ PAGING_PAGES ] = {0,};

static long HIGH_MEMORY = 0;

unsigned long get_free_page(void)
{
        register unsigned long __res asm("ax");

        __asm__("std ; repne ; scasb\n\t"
                "jne 1f\n\t"
                "movb $1,1(%%edi)\n\t"
                "sall $12,%%ecx\n\t"
                "addl %2,%%ecx\n\t"
                "movl %%ecx,%%edx\n\t"
                "movl $1024,%%ecx\n\t"
                "leal 4092(%%edx),%%edi\n\t"
                "rep ; stosl\n\t"
                " movl %%edx,%%eax\n"
                "1: cld"
                :"=a" (__res)
                :"0" (0),"i" (LOW_MEM),"c" (PAGING_PAGES),
                "D" (mem_map+PAGING_PAGES-1)
                );

        return __res;
}

void free_page(unsigned long addr)
{
	if (addr < LOW_MEM) 
                return;

	if (addr >= HIGH_MEMORY)
		panic("trying to free nonexistent page");

	addr -= LOW_MEM;
	addr >>= 12;
	if (mem_map[addr]--) 
                return;

	mem_map[addr]=0;
        panic("trying to free free page");
}

void mem_init(long start_mem, long end_mem)
{
        int i;

	HIGH_MEMORY = end_mem;
	for (i = 0; i < PAGING_PAGES; i++)
		mem_map[i] = USED;

        // Caculate how many pages in main memory( 4~16 MB)
        i = MAP_NR(start_mem);
        end_mem -= start_mem;
	end_mem >>= 12;

        // Clear sharing counts of each page
        while (end_mem-- > 0 )
		mem_map[i++] = 0;
}
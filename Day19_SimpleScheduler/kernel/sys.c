#include <linux/kernel.h>
#include <linux/sys.h>

int sys_write(const char* buf)
{
        printk("%s",buf);
        return 0;
}
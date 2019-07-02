#include <linux/kernel.h>
#include <linux/sys.h>

int sys_write()
{
        printk("Entering User ...\n");
        return 0;
}
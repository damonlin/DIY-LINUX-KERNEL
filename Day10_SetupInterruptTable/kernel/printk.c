#include <stdarg.h>

static char buf[1024];

extern void con_write(const char* str);
extern int vsprintf(char * buf, const char * fmt, va_list args);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);

        con_write(buf);

	return i;
}
#ifndef _STRING_H_
#define _STRING_H_

static inline int strlen(const char* str)
{
    int len = 0;
    while(*str)
    {
        str++;
        len++;
    }

    return len;
}

#endif
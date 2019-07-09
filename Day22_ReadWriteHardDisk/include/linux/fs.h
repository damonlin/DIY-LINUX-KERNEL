#ifndef __FS_H__
#define __FS_H__

#define READ 0
#define WRITE 1

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff)

extern void ll_rw_block(int dev, int rw, char* buffer, int blocknr);

#endif
include ../Makefile.header

LDFLAGS	+= -r
CFLAGS	+= -I../include

OBJS  = printk.o vsprintf.o

.s.o:
	@$(AS) -o $*.o $<

kernel.o: $(OBJS)
	@$(LD) $(LDFLAGS) -o kernel.o $(OBJS)
	@sync

clean:	
	@rm -f *.o *.a
	@for i in chr_drv ; do make clean -C $$i; done
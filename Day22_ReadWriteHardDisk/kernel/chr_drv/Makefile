include ../../Makefile.header

CFLAGS	+= -I../../include

.c.o:
	@$(CC) $(CFLAGS) -c -o $*.o $<

OBJS  = console.o

chr_drv.a: $(OBJS)
	@$(AR) rcs chr_drv.a $(OBJS)
	sync

clean:
	@rm -f *.o *.a	
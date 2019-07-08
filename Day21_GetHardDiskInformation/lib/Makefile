include ../Makefile.header

CFLAGS	+= -I../include

.c.o:
	@$(CC) $(CFLAGS) \
	-c -o $*.o $<

OBJS  = write.o

lib.a: $(OBJS)
	@$(AR) rcs lib.a $(OBJS)
	@sync

clean:
	@rm -f *.o *.a
include ../Makefile.header

LDFLAGS += -r
CFLAGS	+= -I../include

.c.o:
	@$(CC) $(CFLAGS) \
	-c -o $*.o $<

OBJS	= memory.o

all: mm.o

mm.o: $(OBJS)
	@$(LD) $(LDFLAGS) -o mm.o $(OBJS)

clean:	
	@rm -f *.o
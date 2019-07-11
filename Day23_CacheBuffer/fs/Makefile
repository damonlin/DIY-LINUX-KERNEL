include ../Makefile.header

LDFLAGS += -r
CFLAGS	+= -I../include

.c.o:
	@$(CC) $(CFLAGS) \
	-c -o $*.o $<

OBJS	= buffer.o

all: fs.o

fs.o: $(OBJS)
	@$(LD) $(LDFLAGS) -o fs.o $(OBJS)

clean:	
	@rm -f *.o
include ../../Makefile.header
CFLAGS	+= -I../../include

OBJS  = hd.o ll_rw_blk.o

blk_drv.a: $(OBJS)
	@$(AR) rcs blk_drv.a $(OBJS)
	sync

clean:
	@rm -f *.o *.a	

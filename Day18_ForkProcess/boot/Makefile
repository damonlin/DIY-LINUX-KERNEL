include ../Makefile.header
LDFLAGS	+= -Ttext 0 -e start

bootsect: bootsect.s
	@$(AS) -o bootsect.o bootsect.s
	@$(LD) $(LDFLAGS) -o bootsect bootsect.o
	@objcopy -R .pdr -R .comment -R.note -S -O binary bootsect

setup: setup.s
	@$(AS) -o setup.o setup.s
	@$(LD) $(LDFLAGS) -o setup setup.o
	@objcopy -R .pdr -R .comment -R.note -S -O binary setup

head.o: head.s
	@$(AS) -o head.o head.s

clean:
	@rm -f bootsect setup head *.o
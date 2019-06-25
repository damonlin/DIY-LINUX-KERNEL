AS	= as --32
LD	= ld
LDFLAGS = -x -Ttext 0 -e start

# for 32 bit
LDFLAGS	+= -m elf_i386

all:	Image	

Image: bootsect	setup system
	@dd if=bootsect bs=512 count=1 of=Image 2>&1 >/dev/null
	@dd if=setup seek=1 bs=512 count=4 of=Image 2>&1 >/dev/null
	@strip system
	@objcopy -O binary -R .note -R .comment system kernel
	@dd if=kernel seek=5 bs=512 count=2883 of=Image 2>&1 >/dev/null
	@sync

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

system: head.o
	@$(LD) $(LDFLAGS) -o system head.o

clean:
	@rm -f Image *.o bootsect setup

start:
	@qemu-system-x86_64 -m 16M -boot a -fda Image	
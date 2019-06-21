AS	= as --32
LD	= ld
LDFLAGS = -x -Ttext 0 -e start

# for 32 bit
LDFLAGS	+= -m elf_i386

all:	Image	

Image: bootsect	
	@dd if=bootsect bs=512 count=1 of=Image 2>&1 >/dev/null
	@sync

bootsect: bootsect.s	
	@$(AS) -o bootsect.o bootsect.s
	@$(LD) $(LDFLAGS) -o bootsect bootsect.o
	@objcopy -R .pdr -R .comment -R.note -S -O binary bootsect

clean:
	@rm -f Image *.o bootsect	

start:
	@qemu-system-x86_64 -m 16M -boot a -fda Image	
include ./Makefile.header
LDFLAGS	+= -Ttext 0 -e start

all:	Image

Image: boot/bootsect boot/setup	tools/system
	@dd if=boot/bootsect bs=512 count=1 of=Image 2>&1 >/dev/null
	@dd if=boot/setup seek=1 bs=512 count=4 of=Image 2>&1 >/dev/null
	@strip tools/system
	@objcopy -O binary -R .note -R .comment tools/system tools/kernel
	@dd if=tools/kernel seek=5 bs=512 count=2883 of=Image 2>&1 >/dev/null
	@sync

boot/bootsect: boot/bootsect.s	
	@make bootsect -C boot

boot/setup: boot/setup.s
	@make setup -C boot

boot/head.o: boot/head.s
	@make head.o -C boot

tools/system: boot/head.o init/main.o
	@$(LD) $(LDFLAGS) boot/head.o init/main.o -o tools/system

clean:
	@rm -f Image init/*.o tools/*
	@for i in boot; do make clean -C $$i; done 

start:
	@qemu-system-x86_64 -m 16M -boot a -fda Image

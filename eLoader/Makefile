PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include

all:    hbl loader

clean:
	rm -rf *~ *.o *.elf *.bin *.s

CC       = psp-gcc
CFLAGS   := -D DEBUG -D PSP -I $(INCLUDE) -W -Wall -O2 -G0 -fno-pic -mno-abicalls -w -fomit-frame-pointer

ASM      = psp-as

sdk_loader.o: sdk_loader.S
	$(ASM) sdk_loader.S -o sdk_loader.o

sdk_hbl.o: sdk_hbl.S
	$(ASM) sdk_hbl.S -o sdk_hbl.o

lib.o: lib.c
	$(CC) $(CFLAGS) -S lib.c -o lib.s
	$(ASM) lib.s -o lib.o

eloader.o: eloader.c 
	$(CC) $(CFLAGS) -S eloader.c -o eloader.s
	$(ASM) eloader.s -o eloader.o

elf.o: elf.c 
	$(CC) $(CFLAGS) -S elf.c -o elf.s
	$(ASM) elf.s -o elf.o

loader.o: loader.c
	$(CC) $(CFLAGS) -S loader.c -o loader.s
	$(ASM) loader.s -o loader.o	
	
thread.o: thread.c 
	$(CC) $(CFLAGS) -S thread.c -o thread.s
	$(ASM) thread.s -o thread.o
	
modmgr.o: modmgr.c 
	$(CC) $(CFLAGS) -S modmgr.c -o modmgr.s
	$(ASM) modmgr.s -o modmgr.o

syscall.o: syscall.c 
	$(CC) $(CFLAGS) -S syscall.c -o syscall.s
	$(ASM) syscall.s -o syscall.o

memory.o: memory.c
	$(CC) $(CFLAGS) -S memory.c -o memory.s
	$(ASM) memory.s -o memory.o

debug.o: debug.c
	$(CC) $(CFLAGS) -S debug.c -o debug.s
	$(ASM) debug.s -o debug.o
	
config.o: config.c
	$(CC) $(CFLAGS) -S config.c -o config.s
	$(ASM) config.s -o config.o
	
hbl: eloader.o elf.o thread.o memory.o lib.o sdk_hbl.o debug.o config.o modmgr.o linker_hbl.x
	$(PSPDEV)/bin/psp-ld -T linker_hbl.x -L$(PSPSDK)/lib eloader.o elf.o thread.o syscall.o memory.o lib.o sdk_hbl.o debug.o config.o modmgr.o -o eloader.elf
	$(PSPDEV)/bin/psp-strip -s eloader.elf
	$(PSPDEV)/bin/psp-objcopy -O binary eloader.elf hbl.bin
	
loader: loader.o debug.o lib.o sdk_loader.o elf.o config.o linker_loader.x
	$(PSPDEV)/bin/psp-ld -T linker_loader.x -L$(PSPSDK)/lib loader.o debug.o lib.o sdk_loader.o elf.o config.o -o loader.elf
	$(PSPDEV)/bin/psp-strip -s loader.elf
	$(PSPDEV)/bin/psp-objcopy -O binary loader.elf h.bin

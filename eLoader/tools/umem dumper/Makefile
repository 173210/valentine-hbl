PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include

CC       = psp-gcc
CFLAGS   := -D DEBUG -D PSP -I $(INCLUDE) -W -Wall -O2 -G0 -fno-pic -mno-abicalls -w -fomit-frame-pointer
ASM      = psp-as


all: umemdumper

clean:
	rm -rf *~ *.o *.elf *.bin *.s umemdumper

#sdk.o: sdk.S
#	$(ASM) sdk.S -o sdk.o

umemdumper: main.c sdk.o linker.x
	$(CC) $(CFLAGS) -S main.c -o main.s
	$(ASM) main.s -o main.o
	$(PSPDEV)/bin/psp-ld -T linker.x -L$(PSPSDK)/lib main.o sdk.o -o umemdumper.elf
	$(PSPDEV)/bin/psp-strip -s umemdumper.elf
	$(PSPDEV)/bin/psp-objcopy -O binary umemdumper.elf h.bin

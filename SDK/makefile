PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include

all:    main

clean:
	rm -rf *~ *.o *.elf *.bin main.s *.bin 

CC       = psp-gcc
CFLAGS   := -D PSP -I $(INCLUDE) -W -Wall -O2 -G0 -fno-pic -mno-abicalls -w -fomit-frame-pointer

ASM      = psp-as

sdk.o: sdk.S
	$(ASM) sdk.S -o sdk.o


main.o: main.c
	$(CC) $(CFLAGS) -S main.c -o main.s
	$(ASM) main.s -o main.o

main: main.o sdk.o linker.x
	$(PSPDEV)/bin/psp-ld -T linker.x -L$(PSPSDK)/lib main.o sdk.o -o main.elf
	$(PSPDEV)/bin/psp-strip -s main.elf
	$(PSPDEV)/bin/psp-objcopy -O binary main.elf H.BIN

PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include

# default : dev
# make distrib to compile without debug info
# make or make dev to compile with debug info
all: base
nonids: base
base: hbl loader

clean:
	rm -rf *~ *.o *.elf *.bin *.s

# use a different FOLDER to make for different exploits
# Exploit-specific files go in the subfolders, see targets hbl and loader below
FOLDER=patapon2

CC       = psp-gcc

CFLAGS   := -D PSP -I $(INCLUDE) -I $(FOLDER) -I . -Os -W -Wall -Werror -Wshadow -G0 -fno-pic -mno-abicalls -fomit-frame-pointer

#svn revision in code
SVNVERSION=$(shell svnversion -n 2> Makefile.cache)
ifeq ($(SVNVERSION),)
#try windows with tortoise svn
base:
	@echo svnrevision not found, trying SubWCRev
	@SubWCRev . svnversion.txt svnversion.h
else
#linux
CFLAGS += -DSVNVERSION=\"$(SVNVERSION)\"
endif


all: CFLAGS += -D DEBUG -D NID_DEBUG
nonids: CFLAGS += -D DEBUG
dev: all
distrib: base

AS      = psp-as

%.o : %.c

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

.S.o:  
	$(AS) $< -o $@
	
.PRECIOUS: %.s
  	    	
hbl: eloader.o elf.o memory.o lib.o $(FOLDER)/sdk_hbl.o debug.o config.o globals.o modmgr.o syscall.o malloc.o md5.o utils.o graphics.o font.o settings.o tables.o hook.o reloc.o resolve.o test.o $(FOLDER)/linker_hbl.x
	$(PSPDEV)/bin/psp-ld -T $(FOLDER)/linker_hbl.x -L$(PSPSDK)/lib utils.o eloader.o elf.o globals.o syscall.o lib.o  md5.o memory.o $(FOLDER)/sdk_hbl.o debug.o config.o modmgr.o malloc.o graphics.o font.o tables.o settings.o hook.o reloc.o resolve.o test.o -o eloader.elf
	$(PSPDEV)/bin/psp-strip -s eloader.elf
	$(PSPDEV)/bin/psp-objcopy -O binary eloader.elf hbl.bin
	
loader: loader.o debug.o globals.o lib.o $(FOLDER)/sdk_loader.o config.o utils.o malloc.o $(FOLDER)/linker_loader.x
	$(PSPDEV)/bin/psp-ld -T $(FOLDER)/linker_loader.x -L$(PSPSDK)/lib  globals.o utils.o loader.o debug.o lib.o $(FOLDER)/sdk_loader.o config.o malloc.o -o loader.elf
	$(PSPDEV)/bin/psp-strip -s loader.elf
	$(PSPDEV)/bin/psp-objcopy -O binary loader.elf h.bin

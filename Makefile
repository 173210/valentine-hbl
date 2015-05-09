# make  to compile without debug info
# make DEBUG=1 to compile with debug info
ifeq ($(EXPLOIT),loader)
TARGET = H.PRX
else
TARGET = H.BIN
endif
TARGET += HBL.PRX

all: $(TARGET)

CC = psp-gcc
AS = psp-as
FIXUP = psp-fixup-imports

PSPSDK = $(shell psp-config --pspsdk-path)

PRXEXPORTS = $(PSPSDK)/lib/prxexports.o
PRX_LDSCRIPT = -Wl,-T$(PSPSDK)/lib/linkfile.prx

INCDIR = -I$(PSPSDK)/include -Iinclude -I.
LIBDIR = -L$(PSPSDK)/lib

CFLAGS = $(INCDIR) -G1 -Os -Werror -Wl,-q -nostdlib -mno-abicalls -fomit-frame-pointer -fno-pic -fno-zero-initialized-in-bss -flto
ASFLAGS = $(INCDIR)

ifeq ($(EXPLOIT),loader)
CFLAGS += -DLAUNCHER
endif
ifdef NID_DEBUG
DEBUG = 1
CFLAGS += -DNID_DEBUG
endif
ifdef DEBUG
CFLAGS += -DDEBUG
endif

OBJS_COMMON = \
	common/stubs/syscall.o common/stubs/tables.o \
	common/utils/fnt.o common/utils/scr.o common/utils/string.o \
	common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS_COMMON += common/debug.o
endif
ifneq ($(EXPLOIT),loader)
OBJS_COMMON += common/utils/cache.o 
endif

OBJS_LOADER = loader/loader.o loader/bruteforce.o loader/freemem.o loader/runtime.o
ifneq ($(EXPLOIT),loader)
OBJS_LOADER += loader/start.o
endif

OBJS_HBL = hbl/modmgr/elf.o hbl/modmgr/modmgr.o \
	hbl/stubs/hook.o hbl/stubs/md5.o hbl/stubs/resolve.o \
	hbl/eloader.o hbl/settings.o

ifeq ($(EXPLOIT),loader)
LOADER_LDSCRIPT = -Wl,-Tlauncher.ld $(PRX_LDSCRIPT)
else
LOADER_LDSCRIPT = -Wl,-Tloader.ld
endif

LIBS = -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

IMPORTS = common/imports/imports.a

EBOOT.PBP: PARAM.SFO assets/ICON0.PNG assets/PIC1.PNG H.PRX
	pack-pbp EBOOT.PBP PARAM.SFO assets/ICON0.PNG NULL \
		NULL assets/PIC1.PNG NULL H.PRX NULL

PARAM.SFO:
	mksfo 'Half Byte Loader' $@

H.BIN: H.elf
	psp-objcopy -S -O binary -R .sceStub.text $< $@

H.elf: $(PRXEXPORTS) $(OBJS_COMMON) $(OBJS_LOADER) $(IMPORTS)
	$(CC) $(CFLAGS) $(LOADER_LDSCRIPT) $(LIBDIR) $^ $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_LOADER): config.h

HBL.elf: $(PRXEXPORTS) $(OBJS_COMMON) $(OBJS_HBL) $(IMPORTS)
	$(CC) $(CFLAGS) $(PRX_LDSCRIPT) $(LIBDIR) $^ $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_HBL): config.h

%.PRX: %.elf
	psp-prxgen $< $@

$(OBJS_COMMON): config.h

config.h: include/exploits/$(EXPLOIT).h
	cp -f $< $@

$(IMPORTS):
	make -C common/imports

clean:
	rm -f config.h $(OBJS_COMMON) $(OBJS_LOADER) $(OBJS_HBL) H.elf HBL.elf H.BIN HBL.PRX H.PRX PARAM.SFO EBOOT.PBP
	make -C common/imports clean

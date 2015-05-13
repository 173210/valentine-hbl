CC := psp-gcc
AS := psp-as
FIXUP := psp-fixup-imports

PSPSDK := $(shell psp-config --pspsdk-path)

PRXEXPORTS := $(PSPSDK)/lib/prxexports.o
PRX_LDSCRIPT := -Wl,-T$(PSPSDK)/lib/linkfile.prx

INCDIR := -I$(PSPSDK)/include -Iinclude -I.
LIBDIR := -L$(PSPSDK)/lib

CFLAGS := $(INCDIR) -G1 -Os -Werror -Wl,-q -nostdlib -mno-abicalls -fno-pic -flto
ASFLAGS := $(INCDIR)

ifeq ($(EXPLOIT),loader)
CFLAGS += -DLAUNCHER
endif
ifdef NID_DEBUG
DEBUG := 1
CFLAGS += -DNID_DEBUG
endif
ifdef DEBUG
CFLAGS += -DDEBUG
endif

OBJS_COMMON := \
	common/stubs/syscall.o common/stubs/tables.o \
	common/utils/fnt.o common/utils/scr.o common/utils/string.o \
	common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS_COMMON += common/debug.o
endif
ifneq ($(EXPLOIT),loader)
OBJS_COMMON += common/utils/cache.o
endif

LIBS := -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

IMPORTS := common/imports/imports.a

$(OBJS_COMMON): config.h

$(IMPORTS): common/imports
	make -C $<

%.PRX: %.elf
	psp-prxgen $< $@

config.h: include/exploits/$(EXPLOIT).h
	cp -f $< $@

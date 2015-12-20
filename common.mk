CC := psp-gcc
AS := psp-as
FIXUP := psp-fixup-imports

PSPSDK := $(shell psp-config --pspsdk-path)

PRXEXPORTS := $(PSPSDK)/lib/prxexports.o
PRX_LDSCRIPT := -Wl,-T$(PSPSDK)/lib/linkfile.prx

INCDIR := -I$(PSPSDK)/include -Iinclude -I.
LIBDIR := -L$(PSPSDK)/lib

CFLAGS := $(INCDIR) -G1 -Os -Werror -nostdlib -mno-abicalls -fno-pic -flto
LDFLAGS	:= -G1 -Os -Werror -Wl,-q -nostdlib -mno-abicalls -fno-pic -flto
ASFLAGS := $(INCDIR)

ifeq ($(EXPLOIT),launcher)
CFLAGS += -DLAUNCHER
endif
ifdef NID_DEBUG
DEBUG := 1
CFLAGS += -DNID_DEBUG
endif
ifdef DEBUG
CFLAGS += -DDEBUG
endif
ifdef NO_SYSCALL_RESOLVER
CFLAGS += -DNO_SYSCALL_RESOLVER
endif

OBJ_DEBUG := common/debug.o
OBJ_CACHE := common/utils/cache.o
OBJS_COMMON := common/utils/fnt.o common/utils/scr.o common/utils/string.o \
	common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS_COMMON += $(OBJ_DEBUG)
endif
ifneq ($(EXPLOIT),launcher)
OBJS_COMMON += $(OBJ_CACHE)
endif
ifdef NO_SYSCALL_RESOLVER
OBJS_COMMON += common/stubs/tables.o
else
OBJS_COMMON += common/stubs/syscall.o
endif

LIBS := -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

IMPORTS := common/imports/imports.a

$(OBJS_COMMON): config.h

$(IMPORTS): common/imports
	$(MAKE) -C $<

%.PRX: %.elf
	psp-prxgen $< $@

config.h: include/exploits/$(EXPLOIT).h
	cp -f $< $@

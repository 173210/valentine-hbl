CC := psp-gcc
AS := psp-as
FIXUP := psp-fixup-imports

PSPSDK := $(shell psp-config --pspsdk-path)

PRXEXPORTS := $(PSPSDK)/lib/prxexports.o
PRX_LDSCRIPT := -Wl,-T$(PSPSDK)/lib/linkfile.prx

INCDIR := -I$(PSPSDK)/include -Iinclude -I$(O)
LIBDIR := -L$(PSPSDK)/lib

CFLAGS := $(INCDIR) -G1 -Os -Werror -nostdlib -mno-abicalls -fno-pic -flto -std=c99
LDFLAGS	:= -G1 -Os -Werror -Wl,-q -nostdlib -mno-abicalls -fno-pic -flto -fwhole-program
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
OBJS_COMMON := common/utils/cache.o common/utils/fnt.o common/utils/scr.o	\
	common/utils/string.o common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS_COMMON += $(OBJ_DEBUG)
endif
ifdef NO_SYSCALL_RESOLVER
OBJS_COMMON += common/stubs/tables.o
else
OBJS_COMMON += common/stubs/syscall.o
endif

LIBS := -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

IMPORTS := $(O)/imports/imports.a

$(addprefix $(O_PRIV)/,$(OBJS_COMMON)): $(O)/config.h

$(IMPORTS): common/imports
	$(MAKE) -C $< O=../../$(dir $@)

.PHONY: clean-imports
clean-imports:
	rm -rf $(dir $(IMPORTS))

$(O)/%.PRX: $(O_PRIV)/%.elf
	psp-prxgen $< $@

define COMPILE_C_RULE
	$(COMPILE.c) $< $(OUTPUT_OPTION)
	@$(COMPILE.c) -MM $< -MT $@ > $(O_PRIV)/.deps/$(basename $<).d
endef

$(O_PRIV)/%.o: %.c $(O)/config.h
	$(COMPILE_C_RULE)

$(O_PRIV)/%.o: %.S $(O)/config.h
	$(COMPILE_C_RULE)

$(O_PRIV)/%.o: %.s
	$(COMPILE.s) $< $(OUTPUT_OPTION)

$(O)/config.h: include/exploits/$(EXPLOIT).h | $(O)/D
	cp -f $< $@

DEPDIR = $(foreach f,$1,$(eval $f : | $(dir $f)D))

.PHONY:
%/D:
	$(call Q,MKDIR,$(dir $@))mkdir -p $(dir $@)

# make  to compile without debug info
# make DEBUG=1 to compile with debug info
all: H.BIN HBL.PRX

CC = psp-gcc
AS = psp-as
LD = psp-ld
FIXUP = psp-fixup-imports

PSPSDK = $(shell psp-config --pspsdk-path)

INCDIR = -I$(PSPSDK)/include -Iinclude -I.
LIBDIR = -L$(PSPSDK)/lib

CFLAGS = $(INCDIR) -G1 -Os -Wall -Werror -mno-abicalls -fomit-frame-pointer -fno-pic -fno-strict-aliasing -fno-zero-initialized-in-bss
ASFLAGS = $(INCDIR)
LDFLAGS = -O1 -G0

ifdef NID_DEBUG
DEBUG = 1
CFLAGS += -DNID_DEBUG
endif
ifdef DEBUG
CFLAGS += -DDEBUG
endif

OBJS_DMAC = common/imports/sceDmac_0000.o common/imports/sceDmac_0001.o

OBJS_INTERRUPT = common/imports/InterruptManager_0000.o common/imports/InterruptManager_0001.o

OBJS_IO = common/imports/IoFileMgrForUser_0000.o common/imports/IoFileMgrForUser_0001.o common/imports/IoFileMgrForUser_0002.o common/imports/IoFileMgrForUser_0003.o common/imports/IoFileMgrForUser_0004.o common/imports/IoFileMgrForUser_0005.o common/imports/IoFileMgrForUser_0006.o common/imports/IoFileMgrForUser_0007.o common/imports/IoFileMgrForUser_0008.o common/imports/IoFileMgrForUser_0009.o common/imports/IoFileMgrForUser_0010.o common/imports/IoFileMgrForUser_0011.o common/imports/IoFileMgrForUser_0012.o

OBJS_LOADEXEC = common/imports/LoadExecForUser_0000.o common/imports/LoadExecForUser_0001.o common/imports/LoadExecForUser_0002.o common/imports/LoadExecForUser_0003.o

OBJS_MODMGR = common/imports/ModuleMgrForUser_0000.o common/imports/ModuleMgrForUser_0001.o common/imports/ModuleMgrForUser_0002.o common/imports/ModuleMgrForUser_0003.o common/imports/ModuleMgrForUser_0004.o common/imports/ModuleMgrForUser_0005.o common/imports/ModuleMgrForUser_0006.o

OBJS_POWER = common/imports/scePower_0000.o common/imports/scePower_0001.o common/imports/scePower_0002.o common/imports/scePower_0003.o common/imports/scePower_0004.o common/imports/scePower_0005.o common/imports/scePower_0006.o common/imports/scePower_0007.o

OBJS_SUSPEND = common/imports/sceSuspendForUser_0000.o common/imports/sceSuspendForUser_0001.o

OBJS_SYSMEM = common/imports/SysMemUserForUser_0000.o common/imports/SysMemUserForUser_0001.o common/imports/SysMemUserForUser_0002.o common/imports/SysMemUserForUser_0003.o

OBJS_THREADMAN = common/imports/ThreadManForUser_0000.o common/imports/ThreadManForUser_0001.o common/imports/ThreadManForUser_0002.o common/imports/ThreadManForUser_0003.o common/imports/ThreadManForUser_0004.o common/imports/ThreadManForUser_0005.o common/imports/ThreadManForUser_0006.o common/imports/ThreadManForUser_0007.o common/imports/ThreadManForUser_0008.o common/imports/ThreadManForUser_0009.o common/imports/ThreadManForUser_0010.o common/imports/ThreadManForUser_0011.o common/imports/ThreadManForUser_0012.o common/imports/ThreadManForUser_0013.o common/imports/ThreadManForUser_0014.o common/imports/ThreadManForUser_0015.o common/imports/ThreadManForUser_0016.o common/imports/ThreadManForUser_0017.o common/imports/ThreadManForUser_0018.o common/imports/ThreadManForUser_0019.o common/imports/ThreadManForUser_0020.o common/imports/ThreadManForUser_0021.o common/imports/ThreadManForUser_0022.o common/imports/ThreadManForUser_0023.o common/imports/ThreadManForUser_0024.o common/imports/ThreadManForUser_0025.o common/imports/ThreadManForUser_0026.o

OBJS_UTILS = common/imports/UtilsForUser_0000.o common/imports/UtilsForUser_0001.o common/imports/UtilsForUser_0002.o common/imports/UtilsForUser_0003.o common/imports/UtilsForUser_0004.o

OBJS_COMMON = $(OBJS_DMAC) $(OBJS_INTERRUPT) $(OBJS_IO) $(OBJS_LOADEXEC) $(OBJS_MODMGR) $(OBJS_POWER) $(OBJS_SUSPEND) $(OBJS_SYSMEM) $(OBJS_THREADMAN) $(OBJS_UTILS) \
	common/stubs/syscall.o common/stubs/tables.o \
	common/utils/cache.o common/utils/fnt.o common/utils/scr.o common/utils/string.o \
	common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS += common/debug.o
endif

OBJS_LOADER = loader/start.o loader/loader.o loader/bruteforce.o loader/freemem.o loader/runtime.o

OBJS_HBL = hbl/modmgr/elf.o hbl/modmgr/modmgr.o \
	hbl/stubs/hook.o hbl/stubs/md5.o hbl/stubs/resolve.o \
	hbl/exports.o hbl/eloader.o hbl/settings.o

LIBS = -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

H.BIN: H.elf
	psp-objcopy -S -O binary -R .sceStub.text $< $@

H.elf: $(OBJS_COMMON) $(OBJS_LOADER) loader.ld
	$(LD) $(LDFLAGS) -Tloader.ld $(LIBDIR) $(OBJS_COMMON) $(OBJS_LOADER) $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_LOADER): config.h

HBL.PRX: HBL.elf
	psp-prxgen $< $@

HBL.elf: $(OBJS_COMMON) $(OBJS_HBL) $(PSPSDK)/lib/linkfile.prx
	$(LD) $(LDFLAGS) -q -T$(PSPSDK)/lib/linkfile.prx $(LIBDIR) $(OBJS_COMMON) $(OBJS_HBL) $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_HBL): config.h

$(OBJS_DMAC): common/imports/sceDmac.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_INTERRUPT): common/imports/InterruptManager.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_IO): common/imports/IoFileMgrForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_LOADEXEC): common/imports/LoadExecForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_MODMGR): common/imports/ModuleMgrForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_POWER): common/imports/scePower.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_SUSPEND): common/imports/sceSuspendForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_SYSMEM): common/imports/SysMemUserForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_THREADMAN): common/imports/ThreadManForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_UTILS): common/imports/UtilsForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_COMMON): config.h

%.c: %.exp
	psp-build-exports -b $< > $@

config.h: config.txt
	-@echo "//config.h is automatically generated by the Makefile!!!" > $@
	-@echo "#ifndef SVNVERSION" >> $@
	-@echo "#define SVNVERSION \"$(shell svnversion -n)\"" >> $@
	-SubWCRev . $< $@
	-@echo "#include <exploits/$(EXPLOIT).h>" >> $@
	-@echo "#endif" >> $@

clean:
	rm -f config.h $(OBJS_COMMON) $(OBJS_LOADER) $(OBJS_HBL) H.elf HBL.elf H.BIN HBL.PRX

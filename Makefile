# make  to compile without debug info
# make DEBUG=1 to compile with debug info
ifdef EXPLOIT
TARGET = H.BIN
else
TARGET = H.PRX
endif
TARGET += HBL.PRX

all: $(TARGET)

CC = psp-gcc
AS = psp-as
LD = psp-ld
FIXUP = psp-fixup-imports

PSPSDK = $(shell psp-config --pspsdk-path)

PRXEXPORTS = $(PSPSDK)/lib/prxexports.o
PRX_LDSCRIPT = -T$(PSPSDK)/lib/linkfile.prx

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

OBJS_IMPOSE = common/imports/sceImpose_0000.o common/imports/sceImpose_0001.o common/imports/sceImpose_0002.o common/imports/sceImpose_0003.o common/imports/sceImpose_0004.o common/imports/sceImpose_0005.o common/imports/sceImpose_0006.o common/imports/sceImpose_0007.o common/imports/sceImpose_0008.o common/imports/sceImpose_0009.o common/imports/sceImpose_0010.o common/imports/sceImpose_0011.o common/imports/sceImpose_0012.o common/imports/sceImpose_0013.o common/imports/sceImpose_0014.o common/imports/sceImpose_0015.o

OBJS_POWER = common/imports/scePower_0000.o common/imports/scePower_0001.o common/imports/scePower_0002.o common/imports/scePower_0003.o common/imports/scePower_0004.o common/imports/scePower_0005.o common/imports/scePower_0006.o common/imports/scePower_0007.o common/imports/scePower_0008.o common/imports/scePower_0009.o common/imports/scePower_0010.o common/imports/scePower_0011.o common/imports/scePower_0012.o common/imports/scePower_0013.o common/imports/scePower_0014.o common/imports/scePower_0015.o common/imports/scePower_0016.o common/imports/scePower_0017.o common/imports/scePower_0018.o common/imports/scePower_0019.o common/imports/scePower_0020.o common/imports/scePower_0021.o common/imports/scePower_0022.o common/imports/scePower_0023.o common/imports/scePower_0024.o common/imports/scePower_0025.o common/imports/scePower_0026.o common/imports/scePower_0027.o common/imports/scePower_0028.o common/imports/scePower_0029.o common/imports/scePower_0030.o common/imports/scePower_0031.o common/imports/scePower_0032.o common/imports/scePower_0033.o common/imports/scePower_0034.o common/imports/scePower_0035.o common/imports/scePower_0036.o common/imports/scePower_0037.o common/imports/scePower_0038.o common/imports/scePower_0039.o common/imports/scePower_0040.o common/imports/scePower_0041.o common/imports/scePower_0042.o common/imports/scePower_0043.o common/imports/scePower_0044.o common/imports/scePower_0045.o common/imports/scePower_0046.o common/imports/scePower_0047.o common/imports/scePower_0048.o

OBJS_INTERRUPT = common/imports/InterruptManager_0000.o common/imports/InterruptManager_0001.o common/imports/InterruptManager_0002.o common/imports/InterruptManager_0003.o common/imports/InterruptManager_0004.o common/imports/InterruptManager_0005.o common/imports/InterruptManager_0006.o common/imports/InterruptManager_0007.o common/imports/InterruptManager_0008.o common/imports/InterruptManager_0009.o

OBJS_IO = common/imports/IoFileMgrForUser_0000.o common/imports/IoFileMgrForUser_0001.o common/imports/IoFileMgrForUser_0002.o common/imports/IoFileMgrForUser_0003.o common/imports/IoFileMgrForUser_0004.o common/imports/IoFileMgrForUser_0005.o common/imports/IoFileMgrForUser_0006.o common/imports/IoFileMgrForUser_0007.o common/imports/IoFileMgrForUser_0008.o common/imports/IoFileMgrForUser_0009.o common/imports/IoFileMgrForUser_0010.o common/imports/IoFileMgrForUser_0011.o common/imports/IoFileMgrForUser_0012.o common/imports/IoFileMgrForUser_0013.o common/imports/IoFileMgrForUser_0014.o common/imports/IoFileMgrForUser_0015.o common/imports/IoFileMgrForUser_0016.o common/imports/IoFileMgrForUser_0017.o common/imports/IoFileMgrForUser_0018.o common/imports/IoFileMgrForUser_0019.o common/imports/IoFileMgrForUser_0020.o common/imports/IoFileMgrForUser_0021.o common/imports/IoFileMgrForUser_0022.o common/imports/IoFileMgrForUser_0023.o common/imports/IoFileMgrForUser_0024.o common/imports/IoFileMgrForUser_0025.o common/imports/IoFileMgrForUser_0026.o common/imports/IoFileMgrForUser_0027.o common/imports/IoFileMgrForUser_0028.o common/imports/IoFileMgrForUser_0029.o common/imports/IoFileMgrForUser_0030.o common/imports/IoFileMgrForUser_0031.o common/imports/IoFileMgrForUser_0032.o common/imports/IoFileMgrForUser_0033.o common/imports/IoFileMgrForUser_0034.o common/imports/IoFileMgrForUser_0035.o common/imports/IoFileMgrForUser_0036.o

OBJS_KERNEL = common/imports/Kernel_Library_0000.o common/imports/Kernel_Library_0001.o common/imports/Kernel_Library_0002.o common/imports/Kernel_Library_0003.o common/imports/Kernel_Library_0004.o common/imports/Kernel_Library_0005.o

OBJS_LOADEXEC = common/imports/LoadExecForUser_0000.o common/imports/LoadExecForUser_0001.o common/imports/LoadExecForUser_0002.o common/imports/LoadExecForUser_0003.o common/imports/LoadExecForUser_0004.o

OBJS_MODMGR = common/imports/ModuleMgrForUser_0000.o common/imports/ModuleMgrForUser_0001.o common/imports/ModuleMgrForUser_0002.o common/imports/ModuleMgrForUser_0003.o common/imports/ModuleMgrForUser_0004.o common/imports/ModuleMgrForUser_0005.o common/imports/ModuleMgrForUser_0006.o common/imports/ModuleMgrForUser_0007.o common/imports/ModuleMgrForUser_0008.o common/imports/ModuleMgrForUser_0009.o common/imports/ModuleMgrForUser_0010.o common/imports/ModuleMgrForUser_0011.o common/imports/ModuleMgrForUser_0012.o common/imports/ModuleMgrForUser_0013.o

OBJS_STDIO = common/imports/StdioForUser_0000.o common/imports/StdioForUser_0001.o common/imports/StdioForUser_0002.o common/imports/StdioForUser_0003.o common/imports/StdioForUser_0004.o common/imports/StdioForUser_0005.o common/imports/StdioForUser_0006.o common/imports/StdioForUser_0007.o common/imports/StdioForUser_0008.o common/imports/StdioForUser_0009.o

OBJS_SUSPEND = common/imports/sceSuspendForUser_0000.o common/imports/sceSuspendForUser_0001.o common/imports/sceSuspendForUser_0002.o common/imports/sceSuspendForUser_0003.o common/imports/sceSuspendForUser_0004.o common/imports/sceSuspendForUser_005.o common/imports/sceSuspendForUser_0006.o

OBJS_SYSMEM = common/imports/SysMemUserForUser_0000.o common/imports/SysMemUserForUser_0001.o common/imports/SysMemUserForUser_0002.o common/imports/SysMemUserForUser_0003.o common/imports/SysMemUserForUser_0004.o common/imports/SysMemUserForUser_0005.o common/imports/SysMemUserForUser_0006.o common/imports/SysMemUserForUser_0007.o common/imports/SysMemUserForUser_0008.o common/imports/SysMemUserForUser_0009.o

OBJS_THREADMAN = common/imports/ThreadManForUser_0000.o common/imports/ThreadManForUser_0001.o common/imports/ThreadManForUser_0002.o common/imports/ThreadManForUser_0003.o common/imports/ThreadManForUser_0004.o common/imports/ThreadManForUser_0005.o common/imports/ThreadManForUser_0006.o common/imports/ThreadManForUser_0007.o common/imports/ThreadManForUser_0008.o common/imports/ThreadManForUser_0009.o common/imports/ThreadManForUser_0010.o common/imports/ThreadManForUser_0011.o common/imports/ThreadManForUser_0012.o common/imports/ThreadManForUser_0013.o common/imports/ThreadManForUser_0014.o common/imports/ThreadManForUser_0015.o common/imports/ThreadManForUser_0016.o common/imports/ThreadManForUser_0017.o common/imports/ThreadManForUser_0018.o common/imports/ThreadManForUser_0019.o common/imports/ThreadManForUser_0020.o common/imports/ThreadManForUser_0021.o common/imports/ThreadManForUser_0022.o common/imports/ThreadManForUser_0023.o common/imports/ThreadManForUser_0024.o common/imports/ThreadManForUser_0025.o common/imports/ThreadManForUser_0026.o common/imports/ThreadManForUser_0027.o common/imports/ThreadManForUser_0028.o common/imports/ThreadManForUser_0029.o common/imports/ThreadManForUser_0030.o common/imports/ThreadManForUser_0031.o common/imports/ThreadManForUser_0032.o common/imports/ThreadManForUser_0033.o common/imports/ThreadManForUser_0034.o common/imports/ThreadManForUser_0035.o common/imports/ThreadManForUser_0036.o common/imports/ThreadManForUser_0037.o common/imports/ThreadManForUser_0038.o common/imports/ThreadManForUser_0039.o common/imports/ThreadManForUser_0040.o common/imports/ThreadManForUser_0041.o common/imports/ThreadManForUser_0042.o common/imports/ThreadManForUser_0043.o common/imports/ThreadManForUser_0044.o common/imports/ThreadManForUser_0045.o common/imports/ThreadManForUser_0046.o common/imports/ThreadManForUser_0047.o common/imports/ThreadManForUser_0048.o common/imports/ThreadManForUser_0049.o common/imports/ThreadManForUser_0050.o common/imports/ThreadManForUser_0051.o common/imports/ThreadManForUser_0052.o common/imports/ThreadManForUser_0053.o common/imports/ThreadManForUser_0054.o common/imports/ThreadManForUser_0055.o common/imports/ThreadManForUser_0056.o common/imports/ThreadManForUser_0057.o common/imports/ThreadManForUser_0058.o common/imports/ThreadManForUser_0059.o common/imports/ThreadManForUser_0060.o common/imports/ThreadManForUser_0061.o common/imports/ThreadManForUser_0062.o common/imports/ThreadManForUser_0063.o common/imports/ThreadManForUser_0064.o common/imports/ThreadManForUser_0065.o common/imports/ThreadManForUser_0066.o common/imports/ThreadManForUser_0067.o common/imports/ThreadManForUser_0068.o common/imports/ThreadManForUser_0069.o common/imports/ThreadManForUser_0070.o common/imports/ThreadManForUser_0071.o common/imports/ThreadManForUser_0072.o common/imports/ThreadManForUser_0073.o common/imports/ThreadManForUser_0074.o common/imports/ThreadManForUser_0075.o common/imports/ThreadManForUser_0076.o common/imports/ThreadManForUser_0077.o common/imports/ThreadManForUser_0078.o common/imports/ThreadManForUser_0079.o common/imports/ThreadManForUser_0080.o common/imports/ThreadManForUser_0081.o common/imports/ThreadManForUser_0082.o common/imports/ThreadManForUser_0083.o common/imports/ThreadManForUser_0084.o common/imports/ThreadManForUser_0085.o common/imports/ThreadManForUser_0086.o common/imports/ThreadManForUser_0087.o common/imports/ThreadManForUser_0088.o common/imports/ThreadManForUser_0089.o common/imports/ThreadManForUser_0090.o common/imports/ThreadManForUser_0091.o common/imports/ThreadManForUser_0092.o common/imports/ThreadManForUser_0093.o common/imports/ThreadManForUser_0094.o common/imports/ThreadManForUser_0095.o common/imports/ThreadManForUser_0096.o common/imports/ThreadManForUser_0097.o common/imports/ThreadManForUser_0098.o common/imports/ThreadManForUser_0099.o common/imports/ThreadManForUser_0100.o common/imports/ThreadManForUser_0101.o common/imports/ThreadManForUser_0102.o common/imports/ThreadManForUser_0103.o common/imports/ThreadManForUser_0104.o common/imports/ThreadManForUser_0105.o common/imports/ThreadManForUser_0106.o common/imports/ThreadManForUser_0107.o common/imports/ThreadManForUser_0108.o common/imports/ThreadManForUser_0109.o common/imports/ThreadManForUser_0110.o common/imports/ThreadManForUser_0111.o common/imports/ThreadManForUser_0112.o common/imports/ThreadManForUser_0113.o common/imports/ThreadManForUser_0114.o common/imports/ThreadManForUser_0115.o common/imports/ThreadManForUser_0116.o common/imports/ThreadManForUser_0117.o common/imports/ThreadManForUser_0118.o common/imports/ThreadManForUser_0119.o common/imports/ThreadManForUser_0120.o common/imports/ThreadManForUser_0121.o common/imports/ThreadManForUser_0122.o common/imports/ThreadManForUser_0123.o common/imports/ThreadManForUser_0124.o common/imports/ThreadManForUser_0125.o common/imports/ThreadManForUser_0126.o common/imports/ThreadManForUser_0127.o common/imports/ThreadManForUser_0128.o common/imports/ThreadManForUser_0129.o

OBJS_UTILS = common/imports/UtilsForUser_0000.o common/imports/UtilsForUser_0001.o common/imports/UtilsForUser_0002.o common/imports/UtilsForUser_0003.o common/imports/UtilsForUser_0004.o common/imports/UtilsForUser_0005.o common/imports/UtilsForUser_0006.o common/imports/UtilsForUser_0007.o common/imports/UtilsForUser_0008.o common/imports/UtilsForUser_0009.o common/imports/UtilsForUser_0010.o common/imports/UtilsForUser_0011.o common/imports/UtilsForUser_0012.o common/imports/UtilsForUser_0013.o common/imports/UtilsForUser_0014.o common/imports/UtilsForUser_0015.o common/imports/UtilsForUser_0016.o common/imports/UtilsForUser_0017.o common/imports/UtilsForUser_0018.o common/imports/UtilsForUser_0019.o common/imports/UtilsForUser_0020.o common/imports/UtilsForUser_0021.o common/imports/UtilsForUser_0022.o common/imports/UtilsForUser_0023.o common/imports/UtilsForUser_0024.o common/imports/UtilsForUser_0025.o common/imports/UtilsForUser_0026.o

OBJS_COMMON = $(OBJS_DMAC) $(OBJS_INTERRUPT) $(OBJS_IO) $(OBJS_LOADEXEC) $(OBJS_MODMGR) $(OBJS_POWER) $(OBJS_SUSPEND) $(OBJS_SYSMEM) $(OBJS_THREADMAN) $(OBJS_UTILS) \
	common/stubs/syscall.o common/stubs/tables.o \
	common/utils/cache.o common/utils/fnt.o common/utils/scr.o common/utils/string.o \
	common/memory.o common/prx.o common/utils.o
ifdef DEBUG
OBJS_COMMON += common/debug.o
endif

OBJS_LOADER = loader/start.o loader/loader.o loader/bruteforce.o loader/freemem.o loader/runtime.o

OBJS_HBL = hbl/modmgr/elf.o hbl/modmgr/modmgr.o \
	hbl/stubs/hook.o hbl/stubs/md5.o hbl/stubs/resolve.o \
	hbl/eloader.o hbl/settings.o

ifdef EXPLOIT
LOADER_LDSCRIPT = -Tloader.ld
else
LOADER_LDSCRIPT = -Tlauncher.ld $(PRX_LDSCRIPT)
endif

LIBS = -lpspaudio -lpspctrl -lpspdisplay -lpspge -lpsprtc -lpsputility

EBOOT.PBP: PARAM.SFO assets/ICON0.PNG assets/PIC1.PNG H.PRX
	pack-pbp EBOOT.PBP PARAM.SFO assets/ICON0.PNG NULL \
		NULL assets/PIC1.PNG NULL H.PRX NULL

PARAM.SFO:
	mksfo 'Half Byte Loader'

H.BIN: H.elf
	psp-objcopy -S -O binary -R .sceStub.text $< $@

H.elf: $(OBJS_COMMON) $(OBJS_LOADER)
	$(LD) -q $(LDFLAGS) $(LOADER_LDSCRIPT) $(LIBDIR) $(PRXEXPORTS) $(OBJS_COMMON) $(OBJS_LOADER) $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_LOADER): config.h

HBL.elf: $(OBJS_COMMON) $(OBJS_HBL)
	$(LD) $(LDFLAGS) -q $(PRX_LDSCRIPT) $(LIBDIR) $(PRXEXPORTS) $(OBJS_COMMON) $(OBJS_HBL) $(LIBS) -o $@
	$(FIXUP) $@

$(OBJS_HBL): config.h

%.PRX: %.elf
	psp-prxgen $< $@

$(OBJS_DMAC): common/imports/sceDmac.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_IMPOSE): common/imports/sceImpose.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_POWER): common/imports/scePower.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_INTERRUPT): common/imports/InterruptManager.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_IO): common/imports/IoFileMgrForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_LOADEXEC): common/imports/LoadExecForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_MODMGR): common/imports/ModuleMgrForUser.S
	$(CC) $(ASFLAGS) -DF_$(subst common/imports/,,$*) $< -c -o $@

$(OBJS_STDIO): common/imports/StdioForUser.S
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

config.h: config.txt
	-@echo "//config.h is automatically generated by the Makefile!!!" > $@
	-@echo "#ifndef SVNVERSION" >> $@
	-@echo "#define SVNVERSION \"$(shell svnversion -n)\"" >> $@
	-SubWCRev . $< $@
ifdef EXPLOIT
	-@echo "#include <exploits/$(EXPLOIT).h>" >> $@
else
	-@echo "#define LAUNCHER" >> $@
	-@echo "#define HBL_ROOT \"ms0:/HBL\"" >> $@
endif
	-@echo "#endif" >> $@

clean:
	rm -f config.h $(OBJS_COMMON) $(OBJS_LOADER) $(OBJS_HBL) H.elf HBL.elf H.BIN HBL.PRX

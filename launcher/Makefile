TARGET = hbl
OBJS = main.o IoFileMgrForUser.o ModuleMgrForUser.o sceAudio.o sceCtrl.o sceDisplay.o sceGe_user.o scePower.o sceRtc.o ThreadManForUser.o InterruptManager.o StdioForUser.o sceSuspendForUser.o SysMemUserForUser.o UtilsForUser.o sceUtility.o LoadExecForUser.o sceImpose.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

BUILD_PRX = 1
ENCRYPT = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Half Byte Loader
PSP_EBOOT_ICON = assets\ICON0.PNG
PSP_EBOOT_PIC1 = assets\PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

TARGET = eboot
OBJS = main.o graphics.o font.o

PSPSDK=$(shell psp-config --pspsdk-path)

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

LIBS =  -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = HBL Menu
PSP_FW_VERSION=371
BUILD_PRX = 1


include $(PSPSDK)/lib/build.mak

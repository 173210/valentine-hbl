# make  to compile without debug info
# make DEBUG=1 to compile with debug info
EXPLOIT ?= launcher

FLAGS := EXPLOIT=$(EXPLOIT)
ifdef DEBUG
FLAGS += DEBUG=1
endif

all:
	$(MAKE) -f Makefile_loader $(FLAGS)
	rm -f $(OBJS_COMMON)
	$(MAKE) -f Makefile_hbl $(FLAGS)

H.BIN: Makefile_loader
	$(MAKE) -f $< $@ $(FLAGS)

H.PRX: Makefile_loader
	$(MAKE) -f $< $@ $(FLAGS)

HBL.PRX: Makefile_hbl
	$(MAKE) -f $< $@ $(FLAGS)

clean:
	$(MAKE) -f Makefile_loader clean
	$(MAKE) -f Makefile_hbl clean

include common.mk

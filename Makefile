# make  to compile without debug info
# make DEBUG=1 to compile with debug info
EXPLOIT ?= launcher

export EXPLOIT
ifdef DEBUG
export DEBUG=1
endif

all:
	$(MAKE) -f Makefile_loader
	rm -f $(OBJS_COMMON)
	$(MAKE) -f Makefile_hbl

H.BIN: Makefile_loader
	$(MAKE) -f $< $@

H.PRX: Makefile_loader
	$(MAKE) -f $< $@

HBL.PRX: Makefile_hbl
	$(MAKE) -f $< $@

clean:
	$(MAKE) -f Makefile_loader clean
	$(MAKE) -f Makefile_hbl clean

include common.mk

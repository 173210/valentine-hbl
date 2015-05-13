# make  to compile without debug info
# make DEBUG=1 to compile with debug info
FLAGS := EXPLOIT=$(EXPLOIT)
ifdef DEBUG
FLAGS += DEBUG=1
endif

all:
	make -f Makefile_loader $(FLAGS)
	rm -f $(OBJS_COMMON)
	make -f Makefile_hbl $(FLAGS)

H.BIN: Makefile_loader
	make -f $< $(FLAGS)

H.PRX: Makefile_loader
	make -f $< $(FLAGS)

HBL.PRX: Makefile_hbl
	make -f $< $(FLAGS)

clean:
	make -f Makefile_loader clean
	make -f Makefile_hbl clean

include common.mk

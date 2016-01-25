# make  to compile without debug info
# make DEBUG=1 to compile with debug info
EXPLOIT ?= launcher
O ?= output

export EXPLOIT
export O
ifdef DEBUG
export DEBUG=1
endif

.PHONY: all clean
all:
	$(MAKE) -f Makefile_loader
	$(MAKE) -f Makefile_hbl

clean:
	rm -rf $(O)

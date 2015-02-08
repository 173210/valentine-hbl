OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(_start)

SECTIONS
{
	. = hbl;
	.text.start : {
		*(.text.start)
	}
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}

	.lib.stub.top (NOLOAD) : {
		*(.lib.stub.top)
	}
	.lib.stub (NOLOAD) : {
		*(.lib.stub)
	}
	.lib.stub.btm (NOLOAD) : {
		*(.lib.stub.btm)
	}
	.rodata.sceResident (NOLOAD) : {
		*(.rodata.sceResident)
	}
	.rodata.sceNid (NOLOAD) : {
		KEEP (*(.rodata.sceNid))
	}
	. = 0x10000;
	.sceStub.text (NOLOAD) : {
		*(.sceStub.text)
		*(SORT(.sceStub.text.*))
	}
}

OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(_start)

SECTIONS
{
	. = loader;
	.text.start : {
		*(.text.start)
	}
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.lib.stub.top : {
		libStubTop = .;
		*(.lib.stub.top)
	}
	.lib.stub : {
		*(.lib.stub)
	}
	.lib.stub.btm : {
		libStubBtm = .;
		*(.lib.stub.btm)
	}
	.rodata.sceResident : {
		*(.rodata.sceResident)
	}
	.rodata.sceNid : {
		KEEP (*(.rodata.sceNid))
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}

	. = 0x10000;
	.sceStub.text (NOLOAD) : {
		*(.sceStub.text)
		*(SORT(.sceStub.text.*))
	}
}

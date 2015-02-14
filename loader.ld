OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(_start)

SECTIONS
{
	. = __executable_start;
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
		*(.lib.stub.top)
	}
	.lib.stub : {
		libStubTop = .;
		*(.lib.stub)
		libStubBtm = .;
	}
	.lib.stub.btm : {
		*(.lib.stub.btm)
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
	.sceStub.text : {
		*(.sceStub.text)
		*(SORT(.sceStub.text.*))
	}

	.rodata.sceModuleInfo (NOLOAD) : {
		*(.rodata.sceModuleInfo)
	}
	.rodata.sceResident (NOLOAD) : {
		*(.rodata.sceResident)
	}
}

OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(_start)

module_start = 0;

SECTIONS
{
	. = __executable_start;
	.text.start : {
		*(.text.start)
	}
	.text : {
		*(.text)
	}
	.sceStub.text : {
		stubText = .;
		*(.sceStub.text)
		*(SORT(.sceStub.text.*))
	}
	.rodata : {
		*(.rodata)
	}
	.rodata.sceNid : {
		KEEP (*(.rodata.sceNid))
	}
	.rodata.sceResident : {
		*(.rodata.sceResident)
	}
	.lib.stub.top : {
		*(.lib.stub.top)
	}
	.lib.stub : {
		libStub = .;
		*(.lib.stub)
	}
	libStubSize = SIZEOF(.lib.stub);
	.lib.stub.btm : {
		*(.lib.stub.btm)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
	stubTextSize = SIZEOF(.sceStub.text);

	.rodata.sceModuleInfo (NOLOAD) : {
		*(.rodata.sceModuleInfo)
	}
}

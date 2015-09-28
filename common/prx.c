#include <common/utils/string.h>
#include <common/debug.h>
#include <common/prx.h>
#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

#ifdef DEBUG
static void log_modinfo(SceModuleInfo *modinfo)
{
	dbg_printf("\n->Module information:\n"
		"Module name: %s\n"
		"Module version: %d.%d\n"
		"Module attributes: 0x%08X\n"
		"Entry top: 0x%08X\n"
		"Stubs top: 0x%08X\n"
		"Stubs end: 0x%08X\n"
		"GP value: 0x%08X\n",
		modinfo->modname,
		modinfo->modversion[1], modinfo->modversion[0],
		modinfo->modattribute,
		(int)modinfo->ent_top,
		(int)modinfo->stub_top,
		(int)modinfo->stub_end,
		(int)modinfo->gp_value);
}
#endif

static uint32_t unalignLw(void *p)
{
	uint8_t *_p = p;
	return _p[0] | (_p[1] << 8) | (_p[2] << 16) | (_p[3] << 24);
}

static int16_t unalignLh(void *p)
{
	uint8_t *_p = p;
	return _p[0] | (_p[1] << 8);
}

static uint16_t unalignLhu(void *p)
{
	uint8_t *_p = p;
	return _p[0] | (_p[1] << 8);
}

static void unalignSw(void *p, uint32_t w)
{
	uint8_t *_p = p;

	_p[0] = w & 0xFF;
	_p[1] = (w >> 8) & 0xFF;
	_p[2] = (w >> 16) & 0xFF;
	_p[3] = (w >> 24) & 0xFF;
}

static void unalignSh(void *p, uint16_t h)
{
	uint8_t *_p = p;

	_p[0] = h & 0xFF;
	_p[1] = (h >> 8) & 0xFF;
}

static int relocSec(SceUID fd, SceOff off, const Elf32_Shdr *shdr, void *base)
{
	SceUID block;
	tRelEntry *top, *entry;
	void *dst;
	Elf32_Word hiAdd, w;
	int r;

	if (shdr == NULL || base == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	r = sceIoLseek(fd, off + shdr->sh_offset, PSP_SEEK_SET);
	if (r < 0)
		return r;

	block = sceKernelAllocPartitionMemory(2, "HBL Relocation Section",
		PSP_SMEM_Low, shdr->sh_size, NULL);
	if (block < 0)
		return block;

	top = sceKernelGetBlockHeadAddr(block);
	if (top == NULL)
		return SCE_KERNEL_ERROR_ERROR;

	r = sceIoRead(fd, top, shdr->sh_size);
	if (r < 0)
		return r;

	hiAdd = 0;
	entry = (void *)((uintptr_t)top + shdr->sh_size);
	while ((uintptr_t)entry > (uintptr_t)top) {
		entry--;
		dst = (void *)((uintptr_t)base + entry->r_offset);

		switch (ELF32_R_TYPE(entry->r_info)) {
			case R_MIPS_NONE:
			case R_MIPS_GPREL16:
			case R_MIPS_PC16:
				break;

			case R_MIPS_32:
				unalignSw(dst, unalignLw(dst) + (uint32_t)base);
				break;

			case R_MIPS_26:
				w = unalignLw(dst);
				unalignSw(dst, (w & 0xFC000000)
					| ((((uint32_t)base >> 2) + w) & 0x03FFFFFF));
				break;

			case R_MIPS_HI16:
				if (hiAdd == 0) {
					dbg_puts("warning: corresponding R_MIPS_LO16"
						"for R_MIPS_HI16 not found");
					hiAdd = (uint32_t)base;
				}

				unalignSh(dst, unalignLhu(dst) + (hiAdd >> 16));
				break;

			case R_MIPS_LO16:
				w = (uint32_t)base + unalignLh(dst);
				hiAdd = w + 0x8000;
				unalignSh(dst, w & 0xFFFF);
				break;

			default:
				dbg_printf("warning: invalid r_info: 0x%X\n",
					entry->r_info);
				break;
		}
	}

	return sceKernelFreePartitionMemory(block);
}

// Relocates all sections that need to
static int relocAll(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, void *base)
{
	SceSize size;
	SceUID block;
	Elf32_Shdr *p, *btm;
	int r;

	if (hdr == NULL || base == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	size = hdr->e_shnum * sizeof(Elf32_Shdr);

	r = sceIoLseek(fd, off + hdr->e_shoff, PSP_SEEK_SET);
	if (r < 0)
		return r;

	block = sceKernelAllocPartitionMemory(2, "HBL Module Section Headers",
		PSP_SMEM_Low, size, NULL);
	if (block < 0)
		return block;

	p = sceKernelGetBlockHeadAddr(block);
	if (p == NULL) {
		sceKernelFreePartitionMemory(block);
		return SCE_KERNEL_ERROR_ERROR;
	}

	r = sceIoRead(fd, p, size);
	if (r < 0) {
		sceKernelFreePartitionMemory(block);
		return r;
	}

	for (btm = p + hdr->e_shnum; p != btm; p++) {
		if (p->sh_type != LOPROC)
			continue;

		r = relocSec(fd, off, p, base);
		if (r)
			dbg_printf("warning: relocating failed 0x%08X\n", r);
	}

	return sceKernelFreePartitionMemory(block);
}

// Loads relocatable executable in memory using fixed address
// Loads address of first stub header in stub
// Returns total size copied in memory
int prx_load(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, _sceModuleInfo *modinfo, void **addr,
	void *(* allocForModule)(const char *name, SceSize, void *))
{
	Elf32_Phdr phdr;
	int excess, ret;

	//dbg_printf("prx_load_program -> Offset: 0x%08X\n", off);

	if (hdr == NULL || addr == NULL || allocForModule == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	// Read the program header
	ret = sceIoLseek(fd, off + hdr->e_phoff, PSP_SEEK_SET);
	if (ret < 0)
		return ret;
	ret = sceIoRead(fd, &phdr, sizeof(Elf32_Phdr));
	if (ret < 0)
		return ret;
    
	// Check if kernel mode
	if ((int)phdr.p_paddr & 0x80000000)
		return SCE_KERNEL_ERROR_UNSUPPORTED_PRX_TYPE;

	// Read module info from PRX
	ret = sceIoLseek(fd, off + (int)phdr.p_paddr, PSP_SEEK_SET);
	if (ret < 0)
		return ret;
	ret = sceIoRead(fd, modinfo, sizeof(_sceModuleInfo));
	if (ret < 0)
		return ret;

#ifdef DEBUG
	log_modinfo(modinfo);
#endif

	*addr = allocForModule(modinfo->modname, phdr.p_memsz, *addr);
	if (*addr == NULL)
		return SCE_KERNEL_ERROR_NO_MEMORY;

	// Loads program segment
	ret = sceIoLseek(fd, off + phdr.p_off, PSP_SEEK_SET);
	if (ret < 0)
		return ret;
	ret = sceIoRead(fd, *addr, phdr.p_filesz);
	if (ret < 0)
		return ret;

	excess = phdr.p_memsz - ret;
	if (excess > 0)
		memset((void *)((int)*addr + ret + 1), 0, excess);

	dbg_printf("Before reloc -> Offset: 0x%08X\n", off);

	//Relocate all sections that need to
	ret = relocAll(fd, off, hdr, *addr);
	if (ret < 0)
		dbg_printf("warning: relocation error: 0x%08X\n", ret);

	// Return size of total size copied in memory
	return phdr.p_memsz;
}

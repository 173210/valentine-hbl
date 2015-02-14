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
		modinfo->modversion[0], modinfo->modversion[1],
		modinfo->modattribute,
		(int)modinfo->ent_top,
		(int)modinfo->stub_top,
		(int)modinfo->stub_end,
		(int)modinfo->gp_value);
}
#endif

// Relocates based on a MIPS relocation type
// Returns 0 on success, -1 on fail
static int reloc_entry(const tRelEntry *entry, void *addr)
{
	int *target = addr + (int)entry->r_offset;
	int buf;

	if (addr == NULL)
		return -1;

	memcpy(&buf, target, sizeof(int));

	//Relocate depending on reloc type
	switch(ELF32_R_TYPE(entry->r_info)) {
		// Nothing
		case R_MIPS_NONE:
			return 0;

		// Word address
		case R_MIPS_32:
			buf += (int)addr;
			break;

		// Jump instruction
		case R_MIPS_26:
			buf = (buf & 0xFC000000) | ((buf & 0x03FFFFFF) + ((int)addr >> 2));
			break;

		// Low 16 bits relocate as high 16 bits
		case R_MIPS_HI16:
			buf = (buf & 0xFFFF0000) | ((buf & 0x0000FFFF) + ((int)addr >> 16));
			break;

		// Low 16 bits relocate as low 16 bits
		case R_MIPS_LO16:
			buf = (buf & 0xFFFF0000) | ((buf + (int)addr) & 0x0000FFFF);
			break;

		default:
			return -1;
	}

	memcpy(target, &buf, sizeof(int));

	return 0;
}

// Relocates PRX sections that need to
// Returns number of relocated entries
static int reloc(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, void *addr)
{
	Elf32_Shdr shdr;
	tRelEntry relentry;
	SceOff cur;
	int i, j;
	int relocated = 0;

	dbg_printf("relocate_sections\n");

	dbg_printf("Number of sections: %d\n", hdr->e_shnum);

	// First section header
	cur = off + hdr->e_shoff;
	if (sceIoLseek(fd, cur, PSP_SEEK_SET) < 0)
		return relocated;

	// Browse all section headers
	for (i = 0; i < hdr->e_shnum; i++) {
		// Get section header
		if (sceIoRead(fd, &shdr, sizeof(Elf32_Shdr)) < 0)
			break;
		// Next section header
		cur += sizeof(Elf32_Shdr);
		
		if (shdr.sh_type == LOPROC) {
			//dbg_printf("Relocating...\n");

			if (sceIoLseek(fd, off + shdr.sh_offset, PSP_SEEK_SET) < 0)
				continue;

			for (j = 0; j < shdr.sh_size; j += sizeof(tRelEntry)) {
				if (sceIoRead(fd, &relentry, sizeof(tRelEntry)) < 0)
					continue;
				if (reloc_entry(&relentry, addr))
					continue;
				relocated++;
			}

			if (sceIoLseek(fd, cur, PSP_SEEK_SET) < 0)
				break;
		}
	}

	// All relocation section processed
	return relocated;
}

// Loads relocatable executable in memory using fixed address
// Loads address of first stub header in stub
// Returns total size copied in memory
int prx_load(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, _sceModuleInfo *modinfo, void **addr,
	void *(* malloc)(const char *name, SceSize, void *))
{
	Elf32_Phdr phdr;
	int excess, ret;

	//dbg_printf("prx_load_program -> Offset: 0x%08X\n", off);

	if (hdr == NULL || addr == NULL || malloc == NULL)
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

	*addr = malloc(modinfo->modname, phdr.p_memsz, *addr);
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
	ret = reloc(fd, off, hdr, *addr);

	dbg_printf("Relocated entries: %d\n", ret);
	if (!ret)
		dbg_puts("WARNING: no entries to relocate on a relocatable ELF\n");

	// Return size of total size copied in memory
	return phdr.p_memsz;
}

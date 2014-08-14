#include <common/utils/string.h>
#include <common/sdk.h>
#include <common/debug.h>
#include <hbl/mod/elf.h>
#include <hbl/eloader.h>
#include <common/malloc.h>
#include <exploit_config.h>

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
int reloc(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, void *addr)
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

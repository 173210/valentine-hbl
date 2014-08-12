#include <common/utils/string.h>
#include <common/sdk.h>
#include <hbl/mod/elf.h>
#include <common/debug.h>
#include <hbl/eloader.h>
#include <hbl/stubs/hook.h>
#include <common/malloc.h>
#include <common/utils.h>
#include <exploit_config.h>

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

//utility
// Allocates memory for homebrew so it doesn't overwrite itself
//This function is inly used in elf.c. Let's move it to malloc.c ONLY if other files start to need it
static void *elf_malloc(SceSize size, void *addr)
{
	SceUID blockid;

	dbg_printf("-->ALLOCATING MEMORY @ 0x%08X size 0x%08X... ",
		(int)addr, size);

	//hook is used to monitor ram usage and free it on exit
	blockid = _hook_sceKernelAllocPartitionMemory(
		2, "ELFMemory", addr == NULL ? PSP_SMEM_Low : PSP_SMEM_Addr, size, addr);

	if (blockid < 0) {
		dbg_printf("FAILED: 0x%08X\n", blockid);
		return NULL;
	}

	dbg_printf("OK\n");

	return sceKernelGetBlockHeadAddr(blockid);
}


/*****************/
/* ELF FUNCTIONS */
/*****************/
// Loads static executable in memory using virtual address
// Returns total size copied in memory
int elf_load(SceUID fd, SceOff off, const Elf32_Ehdr *hdr)
{
	Elf32_Phdr phdr;
	size_t size, read;
	int excess;
	int i;
	void *buf;

	if (hdr == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	size = 0;
	for (i = 0; i < hdr->e_phnum; i++) {
		dbg_printf("Reading program section %d of %d\n", i, hdr->e_phnum);

		// Read the program header
		if (sceIoLseek(fd, off + hdr->e_phoff + sizeof(Elf32_Phdr) * i, PSP_SEEK_SET) < 0)
			continue;
		if (sceIoRead(fd, &phdr, sizeof(Elf32_Phdr)) < 0)
			continue;

		buf = elf_malloc(phdr.p_memsz, phdr.p_vaddr);
		// Loads program segment at virtual address
		if (sceIoLseek(fd, off + phdr.p_off, PSP_SEEK_SET) < 0)
			continue;
		read = sceIoRead(fd, buf, phdr.p_filesz);
		if (read < 0)
			continue;

		// Fills excess memory with zeroes
		excess = phdr.p_memsz - read;
		if (excess > 0)
			memset(phdr.p_vaddr + read, 0, excess);

		size += phdr.p_memsz;
	}

	return size;
}

// Loads relocatable executable in memory using fixed address
// Loads address of first stub header in stub
// Returns total size copied in memory
int prx_load(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, _sceModuleInfo *modinfo, void **addr)
{
	Elf32_Phdr phdr;
	int excess, ret;

	//dbg_printf("prx_load_program -> Offset: 0x%08X\n", off);

	if (hdr == NULL || addr == NULL)
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

	dbg_printf("Module info @ 0x%08X offset\n", off + (int)phdr.p_paddr);

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

	*addr = elf_malloc(phdr.p_memsz, *addr);

	// Loads program segment at fixed address
	ret = sceIoLseek(fd, off + phdr.p_off, PSP_SEEK_SET);
	if (ret < 0)
		return ret;
	ret = sceIoRead(fd, *addr, phdr.p_filesz);
	if (ret < 0)
		return ret;

	excess = phdr.p_memsz - ret;
	if (excess > 0)
		memset(*addr + ret + 1, 0, excess);

	// Return size of total size copied in memory
	return phdr.p_memsz;
}

// Get index of section if you know the section name
static int elf_get_shdr(
	SceUID fd, SceOff off, const Elf32_Ehdr* hdr, const char *name,
	Elf32_Shdr *shdr)
{
	Elf32_Shdr shdr_names;
	const size_t name_max_len = 22;
	char buf[name_max_len]; // Should be enough to hold the name
	int i, ret;
	size_t name_len = strlen(name);

	if (name_len + 1 > name_max_len) {
		// Section name too long
		dbg_printf("Section name to find is too long (strlen = %d, max = %d)\n",
			name_len, name_max_len);
		name_len = name_max_len;
	}

	// Seek to the section name table
	ret = sceIoLseek(fd, off + hdr->e_shoff + hdr->e_shstrndx * sizeof(Elf32_Shdr), PSP_SEEK_SET);
	if (ret < 0)
		return ret;
	ret = sceIoRead(fd, &shdr_names, sizeof(Elf32_Shdr));
	if (ret < 0)
		return ret;

	// Read section names and compare them
	for (i = 0; i < hdr->e_shnum; i++)
	{
		// Get name index from current section header
		ret = sceIoLseek(fd, off + hdr->e_shoff + i * sizeof(Elf32_Shdr), PSP_SEEK_SET);
		if (ret < 0)
			continue;
		ret = sceIoRead(fd, shdr, sizeof(Elf32_Shdr));
		if (ret < 0)
			continue;

		// Seek to the name entry in the name table
		ret = sceIoLseek(fd, off + shdr_names.sh_offset + shdr->sh_name, PSP_SEEK_SET);
		if (ret < 0)
			continue;
		// Read name in (is the section name length stored anywhere?)
		ret = sceIoRead(fd, buf, name_len);
		if (ret < 0)
			continue;

		if (!strncmp(buf, name, name_len))
			return 0;
	}

	// Section not found
	dbg_printf("ERROR: Section could not be found!\n");
	return -1;
}

// Returns address and size (size) of ".lib.stub" section (imports)
tStubEntry *elf_find_imports(SceUID fd, SceOff off, const Elf32_Ehdr* hdr, size_t *size)
{
	Elf32_Shdr shdr;
	int ret = elf_get_shdr(fd, off, hdr, ".lib.stub", &shdr);
	if (ret < 0)
		return NULL;

	*size = shdr.sh_size;

	return (tStubEntry *)shdr.sh_addr;
}

// Get module GP
int elf_get_gp(SceUID fd, SceOff off, const Elf32_Ehdr* hdr, void **buf)
{
	Elf32_Shdr shdr;
	int ret;

	ret = elf_get_shdr(fd, off, hdr, ".rodata.sceModuleInfo", &shdr);
	if (ret < 0) {
		// Module info not found in sections
		dbg_printf("ERROR: section rodata.sceModuleInfo not found\n");
		return ret;
	}

	// Read in module info
	ret = sceIoLseek(fd,
		off + shdr.sh_offset + offsetof(SceModuleInfo, gp_value),
		PSP_SEEK_SET);
	if (ret < 0)
		return ret;

	return sceIoRead(fd, buf, sizeof(void *));
}


void eboot_get_elf_off(SceUID eboot, SceOff *off)
{
	*off = 0;

	sceIoLseek(eboot, 0x20, PSP_SEEK_SET);
	sceIoRead(eboot, (void *)off, sizeof(int));
}

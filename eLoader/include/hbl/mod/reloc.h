#ifndef ELOADER_RELOC
#define ELOADER_RELOC

// Relocates PRX sections that need to
// Returns number of relocated entries
int reloc(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, void *addr);

#endif

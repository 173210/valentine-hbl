#ifndef _PRX_H_
#define _PRX_H_

#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

int prx_load(SceUID fd, SceOff off, const Elf32_Ehdr *ehdr, const Elf32_Phdr *phdrs,
	_sceModuleInfo *modinfo, void **addr,
	void *(* allocForModule)(const char *name, SceSize, void *));
#endif

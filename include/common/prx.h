#ifndef _PRX_H_
#define _PRX_H_

#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

int prx_load(SceUID fd, SceOff off, const Elf32_Ehdr *hdr, _sceModuleInfo *modinfo, void **addr,
	void *(* malloc)(const char *name, SceSize size, void *p));
#endif

#ifndef ELOADER_MODMGR
#define ELOADER_MODMGR

#include "sdk.h"

typedef struct
{
	unsigned int id;
	unsigned long size;
	void* text_addr;
	void* text_entry;
	void* libstub_addr;	
} HBLModuleInfo;

// Get module ID from module name
SceUID find_module(const char *name);

/*
// Get complete module info from an UID
int get_module_info(SceUID modid, tModuleInfo *modinfo);
*/

/* debug info */
void DumpModuleList();

#endif


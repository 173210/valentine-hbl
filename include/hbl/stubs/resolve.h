#ifndef ELOADER_RESOLVE
#define ELOADER_RESOLVE

#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

// Subsitutes the right instruction
void resolve_call(int *call_to_resolve, u32 call_resolved);

// Resolves imports in ELF's program section already loaded in memory
// Uses game's imports to do the resolving (this can be further improved)
// Returns number of resolves
unsigned int resolve_imports(const tStubEntry* pstub_entry, unsigned int stubs_size);

#endif

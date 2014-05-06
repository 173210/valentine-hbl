#ifndef ELOADER_RESOLVE
#define ELOADER_RESOLVE

// Autoresolves HBL missing stubs
// Some stubs are compulsory, like sceIo*
void resolve_missing_stubs();

// Subsitutes the right instruction
void resolve_call(u32 *call_to_resolve, u32 call_resolved);

// Resolves imports in ELF's program section already loaded in memory
// Uses game's imports to do the resolving (this can be further improved)
// Returns number of resolves
unsigned int resolve_imports(tStubEntry* pstub_entry, unsigned int stubs_size);

#endif

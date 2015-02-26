#include <exploit_config.h>

#define MAX_RUNTIME_STUB_HEADERS 64

extern const tStubEntry libStubTop[];
extern const tStubEntry libStubBtm[];

// If we want to load additional modules in advance to use their syscalls
void load_utils();
void unload_utils();

// Returns !=0 if stub entry is valid, 0 if it's not
int elf_check_stub_entry(const tStubEntry *pentry);

int p2_add_stubs();
int p5_add_stubs();
int resolve_hbl_stubs(const tStubEntry *top, const tStubEntry *end);

#define PSP_MODULE_NET_UPNP             0x0107
#define PSP_MODULE_NET_GAMEUPDATE       0x0108
#define PSP_MODULE_AV_MP4               0x0308

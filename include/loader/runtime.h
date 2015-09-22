#include <config.h>

#define MAX_RUNTIME_STUB_HEADERS 64

#ifndef LAUNCHER
extern tStubEntry libStub[];
extern const int libStubSize[];

extern int stubText[];
extern const int stubTextSize[];
#endif

// If we want to load additional modules in advance to use their syscalls
void load_utils();
void unload_utils();

// Returns !=0 if stub entry is valid, 0 if it's not
int elf_check_stub_entry(const tStubEntry *pentry);

#ifdef NO_SYSCALL_RESOLVER
int p2_add_stubs();
int p5_add_stubs();
#endif

void initLoaderStubs();
int resolveLoaderSyscall();
int resolveHblSyscall(tStubEntry *p, size_t n);

#define PSP_MODULE_NET_UPNP             0x0107
#define PSP_MODULE_NET_GAMEUPDATE       0x0108
#define PSP_MODULE_AV_MP4               0x0308

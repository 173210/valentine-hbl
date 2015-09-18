#ifndef COMMON_SYSCALL_H
#define COMMON_SYSCALL_H

#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

int loadNetCommon(void);
int unloadNetCommon(void);
tStubEntry *getNetLibStubInfo(void);
int resolveSyscall(tStubEntry *dst, tStubEntry *netLib);

#endif

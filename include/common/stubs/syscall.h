#ifndef COMMON_SYSCALL_H
#define COMMON_SYSCALL_H

#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

struct syscallResolver {
	SceUID module;
	tStubEntry *lib;
};

#ifndef UTILITY_NET_COMMON_PATH
int utilityLoadNetCommon(void);
int utilityUnloadNetCommon(void);
#endif

int initSyscallResolver(struct syscallResolver *ctx);
int deinitSyscallResolver(struct syscallResolver *ctx);
int resolveSyscall(tStubEntry *dst, struct syscallResolver *ctx);

#endif

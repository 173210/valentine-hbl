#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/memory.h>
#include <common/sdk.h>
#include <hbl/modmgr/elf.h>
#include <config.h>

typedef struct {
	tStubEntry *dst;
	tStubEntry *src;
} Arg;

#define JUMP_P ((void *)0x10000)

static const char importedLib[] = "sceNetIfhandle_lib";

static int storeStubEntry(tStubEntry *dst, const tStubEntry *src)
{
	dst->lib_name = src->lib_name;
	dst->import_flags = src->import_flags;
	dst->lib_ver = src->lib_ver;
	dst->import_stubs = src->import_stubs;
	dst->stub_size = src->stub_size;
	dst->nid_p = src->nid_p;
	dst->jump_p = JUMP_P;
}

#ifndef UTILITY_NET_COMMON_PATH
int utilityLoadNetCommon()
{
	if (isImported(sceUtilityLoadNetModule))
		return sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	else if (isImported(sceUtilityLoadModule))
		return sceUtilityLoadModule(PSP_MODULE_NET_COMMON);
	else
		return SCE_KERNEL_ERROR_ERROR;
}

int utilityUnloadNetCommon()
{
	if (isImported(sceUtilityUnloadNetModule))
		return sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
	else if (isImported(sceUtilityUnloadModule))
		return sceUtilityUnloadModule(PSP_MODULE_NET_COMMON);
	else
		return SCE_KERNEL_ERROR_ERROR;
}

static int storeThread(SceSize args, const Arg *argp)
{
	while (1) {
		storeStubEntry(argp->dst, argp->src);
		sceKernelDelayThread(0);
	}
}
#endif

static tStubEntry *getNetLibStubInfo()
{
	uintptr_t p;

	for (p = 0x08804000; p < 0x09FFFF00; p += 4)
	{
		if (!strcmp((char *)p, "sceNet_Library")
			&& !strcmp((char *)p + 0x34, importedLib))
		{
			return (void *)(p - 0x80);
		}
	}

	return NULL;
}

int initSyscallResolver(struct syscallResolver *ctx)
{
	int r;

	if (ctx == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

#ifdef UTILITY_NET_COMMON_PATH
	r = sceKernelLoadModule(UTILITY_NET_COMMON_PATH, 0, NULL);
	if (r < 0)
		return r;

	ctx->module = r;
#else
	r = utilityLoadNetCommon();
	if (r < 0)
		return r;
#endif

	ctx->lib = getNetLibStubInfo();
	if (ctx->lib == NULL)
		return SCE_KERNEL_ERROR_ERROR;

	return 0;
}

int deinitSyscallResolver(struct syscallResolver *ctx)
{
#ifdef UTILITY_NET_COMMON_PATH
	return sceKernelUnloadModule(ctx->module);
#else
	return utilityUnloadNetCommon();
#endif
}

int resolveSyscall(tStubEntry *dst, struct syscallResolver *ctx)
{
	size_t jump_size;
	int r;
#ifdef UTILITY_NET_COMMON_PATH
	int s;
#else
	Arg arg;
	SceUID thid;
#endif

	if (dst == NULL || ctx == NULL || ctx->lib == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	jump_size = dst->stub_size * 8;

	dst->import_flags = 0x0011;
	dst->lib_ver = strcmp(dst->lib_name, "sceSuspendForUser") ? 0x4001 : 0x4000;

#ifdef UTILITY_NET_COMMON_PATH
	r = sceKernelUnloadModule(ctx->module);
	if (r)
		return r;

	ctx->module = sceKernelLoadModule(UTILITY_NET_COMMON_PATH, 0, NULL);
	if (ctx->module < 0)
		return ctx->module;

	if ((uintptr_t)ctx->lib->lib_name < 0x08800000
		|| (uintptr_t)ctx->lib->lib_name >= 0x0A000000 - sizeof(importedLib)
		|| strcmp(ctx->lib->lib_name, importedLib))
	{
		ctx->lib = getNetLibStubInfo();
		if (ctx->lib == NULL)
			return SCE_KERNEL_ERROR_ERROR;
	}

	storeStubEntry(ctx->lib, dst);

	r = sceKernelStartModule(ctx->module, 0, NULL, &s, NULL);
	if (s) {
		dbg_printf("warning: %s: the internal routine failed\n"
			"warning: to start the module: 0x%08X\n",
			__func__, s);
	}

	if (r)
		return r;
#else
	arg.src = dst;
	arg.dst = ctx->lib;

	thid = sceKernelCreateThread("HBL Stub Information Injector",
		(void *)storeThread, 8, 512, THREAD_ATTR_USER, NULL);
	if (thid < 0)
		return thid;

	r = utilityUnloadNetCommon();
	if (r) {
		sceKernelDeleteThread(thid);
		return r;
	}

	r = sceKernelStartThread(thid, sizeof(arg), &arg);
	if (r) {
		sceKernelDeleteThread(thid);
		utilityLoadNetCommon();
		return r;
	}

	r = utilityLoadNetCommon();
	if (r) {
		kill_thread(thid);
		return r;
	}

	r = kill_thread(thid);
	if (r)
		return r;
#endif

	memcpy(dst->jump_p, JUMP_P, jump_size);

#ifdef UTILITY_NET_COMMON_PATH
	r = sceKernelStopModule(ctx->module, 0, NULL, &s, NULL);
	if (s)
		dbg_printf("warning: %s: the internal routine failed\n"
			"warning: to stop the module: 0x%08X\n",
			__func__, s);

	return r;
#else
	return 0;
#endif
}

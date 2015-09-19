#include <common/stubs/syscall.h>
#include <common/utils/string.h>
#include <common/memory.h>
#include <common/sdk.h>
#include <hbl/modmgr/elf.h>

typedef struct {
	tStubEntry *dst;
	tStubEntry *src;
} Arg;

int loadNetCommon()
{
	if (isImported(sceUtilityLoadNetModule))
		return sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	else if (isImported(sceUtilityLoadModule))
		return sceUtilityLoadModule(PSP_MODULE_NET_COMMON);
	else
		return SCE_KERNEL_ERROR_ERROR;
}

int unloadNetCommon()
{
	if (isImported(sceUtilityUnloadNetModule))
		return sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
	else if (isImported(sceUtilityUnloadModule))
		return sceUtilityUnloadModule(PSP_MODULE_NET_COMMON);
	else
		return SCE_KERNEL_ERROR_ERROR;
}

static int store(SceSize args, const Arg *argp)
{
	while (1) {
		argp->dst->lib_name = argp->src->lib_name;
		argp->dst->import_flags = argp->src->import_flags;
		argp->dst->lib_ver = argp->src->lib_ver;
		argp->dst->import_stubs = argp->src->import_stubs;
		argp->dst->stub_size = argp->src->stub_size;
		argp->dst->nid_p = argp->src->nid_p;

		sceKernelDelayThread(0);
	}
}

tStubEntry *getNetLibStubInfo()
{
	uintptr_t p;

	for (p = 0x08804000; p < 0x09FFFF00; p += 4)
	{
		if (!strcmp((char *)p, "sceNet_Library")
			&& !strcmp((char *)p + 0x34, "sceNetIfhandle_lib"))
		{
			return (void *)(p - 0x80);
		}
	}

	return NULL;
}

int resolveSyscall(tStubEntry *dst, tStubEntry *netLib)
{
	Arg arg;
	SceUID thid;
	int r;

	if (dst == NULL || netLib == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	dst->import_flags = 0x0011;
	dst->lib_ver = strcmp(dst->lib_name, "sceSuspendForUser") ? 0x4001 : 0x4000;

	arg.src = dst;
	arg.dst = netLib;

	thid = sceKernelCreateThread("HBL Stub Information Injector",
		(void *)store, 8, 512, THREAD_ATTR_USER, NULL);
	if (thid < 0)
		return thid;

	r = unloadNetCommon();
	if (r) {
		sceKernelDeleteThread(thid);
		return r;
	}

	r = sceKernelStartThread(thid, sizeof(arg), &arg);
	if (r) {
		sceKernelDeleteThread(thid);
		loadNetCommon();
		return r;
	}

	r = loadNetCommon();
	if (r) {
		kill_thread(thid);
		loadNetCommon();
		return r;
	}

	r = kill_thread(thid);
	if (r)
		return r;

	memcpy(dst->jump_p, netLib->jump_p, dst->stub_size * 8);

	return 0;
}

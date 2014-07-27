#include <common/stubs/syscall.h>
#include <common/sdk.h>
#include <common/debug.h>

SceUID _test_sceIoDopen(const char* path)
{
	SceUID ret = sceIoDopen(path);

#ifdef REESTIMATE_SYSCALL
	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOG_PRINTF("Reestimating sceIoDopen -> Method: %d\n", i);
		//reestimate_syscall("IoFileMgrForUser", 0xb29ddf9c, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0028, i);
		reestimate_syscall("IoFileMgrForUser", 0xb29ddf9c, (u32*)0x10028, i);
		ret = sceIoDopen(path);
	}
#endif

	return ret;
}

int _test_sceIoDread(SceUID id, SceIoDirent* entry)
{
	int ret = sceIoDread(id, entry);

#ifdef REESTIMATE_SYSCALL
	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOG_PRINTF("Reestimating sceIoDread -> Method: %d\n", i);
		//reestimate_syscall("IoFileMgrForUser", 0xe3eb004c, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0030, i);
		reestimate_syscall("IoFileMgrForUser", 0xe3eb004c, (u32*)0x10030, i);
		ret = sceIoDread(id, entry);
	}
#endif

	return ret;
}

int _test_sceIoDclose(SceUID id)
{
	int ret = sceIoDclose(id);

#ifdef REESTIMATE_SYSCALL
	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOG_PRINTF("Reestimating sceIoDclose -> Method: %d\n", i);
		//reestimate_syscall("IoFileMgrForUser", 0xeb092469, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0038, i);
		reestimate_syscall("IoFileMgrForUser", 0xeb092469, (u32*)0x10038, i);
		ret = sceIoDclose(id);
	}
#endif

	return ret;
}

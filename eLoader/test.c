#include "sdk.h"
#include "debug.h"
#include "syscall.h"
#include "scratchpad.h"

SceUID _test_sceIoDopen(const char* path)
{
	SceUID ret = sceIoDopen(path);

	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOGSTR1("Reestimating sceIoDopen -> Method: %d\n", i);
		reestimate_syscall("IoFileMgrForUser", 0xb29ddf9c, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0028, i);	
		ret = sceIoDopen(path);
	}	
	
	return ret;
}

int _test_sceIoDread(SceUID id, SceIoDirent* entry)
{
	int ret = sceIoDread(id, entry);

	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOGSTR1("Reestimating sceIoDread -> Method: %d\n", i);
		reestimate_syscall("IoFileMgrForUser", 0xe3eb004c, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0030, i);
		ret = sceIoDread(id, entry);
	}	

	return ret;
}

int _test_sceIoDclose(SceUID id)
{
	int ret = sceIoDclose(id);
	
	int i = -1;
	while (ret < 0 && i < MAX_REESTIMATE_ATTEMPTS)
	{
		i++;
		LOGSTR1("Reestimating sceIoDclose -> Method: %d\n", i);
		reestimate_syscall("IoFileMgrForUser", 0xeb092469, *(u32*)ADDR_HBL_STUBS_BLOCK_ADDR + 0x0038, i);
		ret = sceIoDclose(id);
	}

	return ret;
}

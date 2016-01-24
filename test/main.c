#include <string.h>
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psppower.h>

#define printf	pspDebugScreenPrintf
#define puts	pspDebugScreenPuts

PSP_MODULE_INFO("HBL Tester", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(256);

int nbErrors = 0;

int ok()
{
    printf("ok\n");
    return 1;
}

int error()
{
    printf("ERROR\n");
    nbErrors++;
    return 0;
}

// Expect at least 20MB Ram Free
int testFreeRam()
{
    printf("sceKernelMaxFreeMemSize > 20MB...");
    int ram = sceKernelMaxFreeMemSize();
    if (ram < 20 * 1024 * 1024)
        return error();

    return ok();
}

int testFrequencies()
{
    int result = 1;
    scePowerSetClockFrequency(333, 333, 166);

    printf("scePowerGetCpuClockFrequency = 333...");
    int cpu = scePowerGetCpuClockFrequency();
    if (cpu < 332 ||cpu > 334) {
        result = error();
    } else {
        ok();
    }

    return result;

}

static int testMd5()
{
	int ret;
	int i = 0;

	u8 digest[16];
	const u8 correctDigest[] = {
		0x38, 0xA6, 0xB2, 0x65, 0x28, 0x08, 0x13, 0x93,
		0xD8, 0xAF, 0x17, 0xE0, 0xEC, 0x20, 0xD6, 0xDB
	};
	u8 data[] = {
		0x5A, 0x0A, 0x3B, 0xD4, 0x25, 0x0D, 0x7F, 0x63,
		0xC7, 0x97, 0x39, 0x23, 0x0F, 0x06, 0x45, 0x43,
		0x41, 0x27, 0xD9, 0x31, 0x1A, 0x7D, 0xA7, 0xA4,
		0x62, 0xCD, 0xB0, 0x66, 0xEE, 0x93, 0x5D, 0x42,
		0x3C, 0xE1, 0xC4, 0x77, 0xD1, 0x96, 0xF2, 0xCA,
		0x73, 0xBC, 0x85, 0xEF, 0x7E, 0x85, 0x4A, 0x5C,
		0x39, 0x01, 0xDC, 0x86, 0x45, 0x86, 0xFA, 0x17,
		0x96, 0x27, 0x25, 0x3F, 0xDC, 0x8B, 0x99, 0x27,
		0x88, 0xBD, 0x2E, 0x3D, 0x41, 0x03, 0x46, 0x52,
		0xB1, 0x3E, 0x32, 0xA7, 0x18, 0xBA, 0xF3, 0x0F,
		0xE8, 0x4C, 0x93, 0xA2, 0xDE, 0x81, 0x41, 0xCA,
		0x57, 0x44, 0x7E, 0x98, 0x9D, 0xED, 0x98, 0x1B,
		0x2A, 0x3B, 0x30, 0xD1, 0x71, 0xB9, 0x7A, 0x87,
		0x18, 0xAF, 0x91, 0xFE, 0x89, 0xA7, 0xCD, 0x5D,
		0x8C, 0x68, 0xE4, 0x38, 0xC0, 0x39, 0x61, 0xBF,
		0xFD, 0x58, 0xF9, 0x71, 0x0F, 0x0F, 0x92, 0x10,
		0x5D, 0xDA, 0x5C, 0x68, 0x81, 0x71, 0xCC, 0x96,
		0xD4, 0x08, 0xE1, 0xC3, 0xA7, 0x38, 0x7C, 0x49,
		0x05, 0x23, 0x29, 0x81, 0x4C, 0xDA, 0x08, 0xBF,
		0x91, 0x37, 0xBE, 0x50, 0x8C, 0xC1, 0x11, 0x6B,
		0x3E, 0x72, 0x52, 0xC8, 0xEB, 0x70, 0x64, 0x75,
		0xB1, 0xE0, 0x74, 0xA6, 0x46, 0x65, 0xCA, 0x74,
		0x11, 0x23, 0x8C, 0xE7, 0x75, 0xB5, 0xA6, 0x79
	};

	printf("sceKernelUtilsMd5Digest...");
	ret = sceKernelUtilsMd5Digest(data, sizeof(data), (void *)digest);
	printf("0x%08X - %s\n", ret, ret ? "ERROR" : "OK");
	if (ret)
		return ret;

	printf("Digest:           ");
	for (i = 0; i < 16; i++)
		printf("%02X", digest[i]);
	printf("\nCorrect Digest: ");
	for (i = 0; i < 16; i++)
		printf("%02X", correctDigest[i]);

	printf("\nVerfying...");
	for (i = 0; i < 16; i++)
		if (digest[i] != correctDigest[i]) {
			printf("NG\n");
			return -1;
		}
	printf("OK\n");

	return 0;
}

static int testIoDread(const char *path)
{
	SceIoDirent ent;
	SceUID uid;
	int r, trial;

	printf("sceIoDopen(\"%s\")...", path);
	uid = sceIoDopen(path);
	if (uid < 0) {
		printf("error: 0x%08X\n", uid);
		return uid;
	}
	puts("OK\n");

	trial = 0;
	do {
		trial++;
		printf("sceIoDread (trial: %d)...", trial);
		r = sceIoDread(uid, &ent);
		if (r < 0)
			printf("error: 0x%08X\n", r);
		else
			puts("OK\n");
	} while (r > 0);

	puts("sceIoDclose...");
	r = sceIoDclose(uid);
	if (r)
		printf("error: 0x%08X\n", r);
	else
		puts("OK\n");

	return 0;
}

int main(int argc, char *argv[])
{
	SceCtrlData pad;
	char *dir, *slash;

	pspDebugScreenInit();
	pspDebugScreenClear();

    printf("Testing HBL\n\n");

    testFreeRam();
    testFrequencies();
    testMd5();

	dir = argv[0];
	if (dir == NULL) {
		puts("error: argv[0] == NULL");
		goto skipDread;
	}

	slash = strrchr(dir, '/');
	if (slash == NULL) {
		printf("error: no slash in argv[0]: %s\n", dir);
		goto skipDread;
	}

	*slash = 0;
	testIoDread(dir);
skipDread:

	printf("\nPress X to exit\n\n");

	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS));

	printf("Exiting...\n");
	sceKernelExitGame();

	return 0;
}

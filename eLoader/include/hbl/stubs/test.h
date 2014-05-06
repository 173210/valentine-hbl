#ifndef ELOADER_TEST
#define ELOADER_TEST

SceUID _test_sceIoDopen(const char* path);
int _test_sceIoDread(SceUID id, SceIoDirent* entry);
int _test_sceIoDclose(SceUID id);

#endif

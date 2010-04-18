#ifndef ELOADER_THREADMAN
#define ELOADER_THREADMAN

/* Find a thread by name */
SceUID find_thread(const char *name);

/* Find a semaphore by name */
SceUID find_sema(const char *name);

/* Find a FPL by name */
SceUID find_fpl(const char *name);

/* Debug functions */
void DumpThreadmanObjects();

#endif


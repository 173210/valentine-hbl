#include <common/utils/string.h>
#include <common/utils.h>
#include <common/debug.h>
#include <exploit_config.h>

// Searches for s1 string on memory
// Returns pointer to string
void *memfindsz(const char *s, const void *p, size_t size)
{
	while (size) {
                if (!strcmp((const char *)p, s))
                        return (void *)p;
		size--;
		p++;
	}

        return NULL;
}

// Searches for word value on memory
// Starting address must be word aligned
// Returns pointer to value
void *memfindint(int val, const void *p, size_t size)
{
	while (size) {
                if (*(int *)p == val)
                        return (void *)p;
		size -= sizeof(int);
		p += sizeof(int);
	}

        return NULL;
}

// Returns 1 if a given file exists, 0 otherwise
int file_exists(const char *file)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	if (fd < 0)
		return 0;

	sceIoClose(fd);

	return 1;
}


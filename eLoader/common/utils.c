#include <common/utils/string.h>
#include <common/utils.h>

// Searches for s string in memory
// Returns pointer to string
void *findstr(const char *s, const void *p, size_t size)
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
// Returns pointer to value
void *findw(int val, const void *p, size_t size)
{
	while (size) {
                if (*(int *)p == val)
                        return (void *)p;
		size -= sizeof(int);
		p += sizeof(int);
	}

        return NULL;
}


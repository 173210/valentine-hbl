#ifndef _UTILS_H_
#define _UTILS_H_

#include <common/sdk.h>

// Returns 1 if pointer is a valid PSP umem pointer, 0 otherwise
#define valid_umem_pointer(p) ((int)(p) >= 0x08400000 && (int)(p) < 0x0A000000)

// Searches for s string in memory
// Returns pointer to string
void *findstr(const char *s, const void *p, size_t size);

// Searches for word value on memory
// Returns pointer to value
void *findw(int val, const void *p, size_t size);

#endif


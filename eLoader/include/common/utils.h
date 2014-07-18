#ifndef _UTILS_H_
#define _UTILS_H_

#include <common/sdk.h>

#define PSP_GO 2
#define PSP_OTHER 3

#ifndef VITA
u32 get_fw_ver();
u32 getPSPModel();
#endif

//Returns 1 if address is a valid PSP umem address, 0 otherwise
int valid_umem_pointer(u32 addr);

// Searches for s1 string in memory
// Returns pointer to string
void *memfindsz(const char *s, const void *p, size_t size);

// Searches for word value on memory
// Returns pointer to value
void *memfindint(int val, const void *p, size_t size);

// Returns 1 if a given file exists, 0 otherwise
int file_exists(const char *file);

#endif

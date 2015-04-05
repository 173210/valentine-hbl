#ifndef CACHE_H
#include <stddef.h>
#include <config.h>

#ifdef LAUNCHER
#define hblIcacheFillRange(top, end) \
	sceKernelDcacheInvalidateRange((top), (uintptr_t)(end) - (uintptr_t)(top))
#else
void hblIcacheFillRange(const void *top, const void *end);
#endif

#endif

#ifndef CTYPE_H
#define CTYPE_H

static inline int toupper(int c)
{
	return c >= 0x61 && c <= 0x7A ? c & ~0x40 : c;
}

#endif

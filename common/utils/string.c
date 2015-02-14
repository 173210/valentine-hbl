#include <common/utils/string.h>

static char *_sprintf_dst;

// Sets a memory region to a specific value
void *memset(void *s, int c, size_t n)
{
	char *p;

	for(p = s; (int)p < (int)s + n; p++)
		*p = c;

	return s;
}

// Copies one memory buffer into another
void *memcpy(void *dst, const void *src, size_t n)
{
	char *p;

	for(p = (char *)dst; (int)p < (int)dst + n; p++) {
		*p = *(char *)src;
		src++;
	}

	return dst;
}

//Scan s for the character.  When this loop is finished,
//    s will either point to the end of the string or the
//    character we were looking for
char *strchr(const char *s, int c)
{
	while (*s != c) {
		if (!*s)
			return NULL;
		s++;
	}

	return (char *)s;
}

// Compares s1 with s2, returns 0 if both equal
int strcmp(const char *s1, const char *s2)
{
	while(*s1 == *s2) {
		if(!*s1)
			return 0;

		s1++;
		s2++;
	}

	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Compares only "count" chars from strings
int strncmp(const char *s1, const char *s2, size_t n)
{
	while(*s1 == *s2) {
		n--;
		if(!*s1 || !n)
			return 0;

		s1++;
		s2++;
	}

	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static int toupper(int c)
{
	if (c > 96 && c < 123)
		c -= 32;

	return c;
}

int strcasecmp(const char *s1, const char *s2)
{
	while(toupper(*s1) == toupper(*s2)) {
		if(!*s1)
			return 0;

		s1++;
		s2++;
	}

	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Copies string src into dst
char *strcpy(char *dst, const char *src)
{
	char *p = dst;

	while(*src) {
		*p = *src;
		p++;
		src++;
	}

	*p = '\0';

	return dst;
}

// Returns string length
size_t strlen(const char *s)
{
	int i = 0;

	while (*s) {
		s++;
		i++;
	}

	return i;
}


/*
 * Basic Sprintf functions
 */ 

static void _itoa(unsigned val, void (* f)(int), int base, int w)
{
	char buf[10];
	int i = 0;
	int rem;

	do {
		rem = val % base;
		buf[i++] = (rem < 10 ? 0x30 : 0x37) + rem;
		val /= base;
		w--;
	} while (val || w > 0);

	while (i > 0)
		f(buf[--i]);
}

void _vfprintf(void (* f)(int), const char *fmt, va_list va)
{
	const char *p;
	char c, w;
	int val;

	while ((c = *fmt++) != '\0') {
		if (c != '%') {
			f(c);
		} else {
			c = *fmt++;

			w = 0;
			if (c == '0') {
				c = *fmt++;
				if (c >= '0' && c <= '8') {
					w = c - '0';
					c = *fmt++;
				}
			}

			switch (c) {
				case 'd':
					val = va_arg(va, int);
					if (val < 0) {
						f('-');
						val = -val;
					}
					_itoa((unsigned)val, f, 10, w);
					break;
				case 'X' : 
					_itoa((unsigned)va_arg(va, int), f, 16, w);
					break;
				case 's' :
					p = va_arg(va, const char *);
					while (*p != '\0')
						f(*p++);
					break;
			}
		}
	}
}

static void _sprintf_write(int s)
{
	*_sprintf_dst++ = s;
}

void _sprintf(char *s, const char *fmt, ...)
{
	va_list va;

	_sprintf_dst = s;

	va_start(va, fmt);
	_vfprintf(_sprintf_write, fmt, va);
	va_end(va);

	*_sprintf_dst = '\0';
}


#include <common/sdk.h>
#include <common/debug.h>

// Sets a memory region to a specific value
void *memset(void *s, int c, size_t n)
{
	char *p;

	for(p = s; (int)p < (int)(s + n); p++)
		*p = c;

	return s;
}

// Copies one memory buffer into another
void *memcpy(void *dst, const void *src, size_t n)
{
	char *p = dst;

	while (n != 0) {
		*p = *(const char *)src;
		p++;
		src++;
		n--;
	}

	return dst;
}

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


//Scan s for the character.  When this loop is finished,
//    s will either point to the end of the string or the
//    character we were looking for
char *strchr(const char *s, int c)
{
	while (*s != '\0') {
		if (*s == c)
			return (char *)s;
		s++;
	}

	return NULL;
}


// Returns string length
size_t strlen(const char *s)
{
	int i = 0;

	while (*s != '\0') {
		s++;
		i++;
	}

	return i;
}

// Copies string src into dst
char *strcpy(char *dst, const char *src)
{
	while(*src) {
		*dst = *src;
		dst++;
		src++;
	}

	*dst = '\0';

	return dst;
}

// Copies a line from a file
// Returns string length
char *fgets(char *s, size_t n, SceUID fd)
{
	char *p = s;

	while (n) {
		if (sceIoRead(fd, p, sizeof(char)) != sizeof(char))
			break;
		if (*p == '\0' || *p == '\n')
			break;
		p++;
		n--;
	}

	p = '\0';

	return s;
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

/*
 * Basic Sprintf functions
 */ 

void _itoa(unsigned val, char **buf, int base, int w)
{
	char *p2;
	char *p1 = *buf;
	char c;
	int rem;

	do {
		rem = val % base;
		*(*buf)++ = (rem < 10 ? 0x30 : 0x37) + rem;
		val /= base;
		w--;
	} while (val || w > 0);

	p2 = *buf - 1;
	while (p1 < p2) {
		c = *p1;
		*p1++ = *p2;
		*p2-- = c;
	}
}

void _vsprintf(char *s, const char *fmt, va_list va)
{
	const char *p;
	char c, w;
	int val;

	while ((c = *fmt++) != '\0') {
		if (c != '%') {
			*s++ = c;
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
						*s++ = '-';
						val = -val;
					}
					_itoa((unsigned)val, &s, 10, w);
					break;
				case 'X' : 
					_itoa((unsigned)va_arg(va, int), &s, 16, w);
					break;
				case 'c' :
					*s++ = (char)va_arg(va, int);
					break;
				case 's' :
					p = va_arg(va, const char *);
					while (*p != '\0')
						*s++ = *p++;
					break;
			}
		}
	}
	*s = '\0';
}

void _sprintf(char *s, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_vsprintf(s, fmt, va);
	va_end(va);
}



/* * * * * * * * * * * *
 * For functions below:
 * * * * * * * * * * * *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 * * * * * * * * * * * *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * * * * * * * * * * * *
 */

// Compares s1 with s2, returns 0 if both equal
int strcmp(const char *s1, const char *s2)
{
	int val = 0;
	const unsigned char *u1, *u2;

	u1 = (unsigned char *) s1;
	u2 = (unsigned char *) s2;

	while(1)
	{
		if(*u1 != *u2)
		{
			val = (int) *u1 - (int) *u2;
			break;
		}

		if((*u1 == 0) && (*u2 == 0))
		{
			break;
		}

		u1++;
		u2++;
	}

	return val;
}

// Concatenates string s + append
char *strcat(char *s, const char *append)
{
	char *pRet = s;
	while(*s)
	{
		s++;
	}

	while(*append)
	{
		*s++ = *append++;
	}

	*s = 0;

	return pRet;
}

// Compares only "count" chars from strings
int strncmp(const char *s1, const char *s2, size_t count)
{
	int val = 0;
	const unsigned char *u1, *u2;

	u1 = (unsigned char *) s1;
	u2 = (unsigned char *) s2;

	while(count > 0)
	{
		if(*u1 != *u2)
		{
			val = (int) *u1 - (int) *u2;
			break;
		}

		if((*u1 == 0) && (*u2 == 0))
		{
			break;
		}

		u1++;
		u2++;
		count--;
	}

	return val;
}

char * strrchr(const char *cp, int ch)
{
    char *save;
    char c;

    for (save = (char *) NULL; (c = *cp); cp++) {
	if (c == ch)
	    save = (char *) cp;
    }

    return save;
}

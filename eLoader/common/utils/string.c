#include <common/utils/string.h>

char *_sprintf_dst;

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
				case 'c' :
					f((char)va_arg(va, int));
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


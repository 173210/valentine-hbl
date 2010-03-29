#include "sdk.h"
#include "debug.h"

// Sets a memory region to a specific value
void memset(void *ptr, byte c, int size)
{
	byte *p1 = ptr;
	byte *p2 = ptr + size;
	while(p1 != p2) {
		*p1 = c;
		p1++;
	}
}

// Copies one memory buffer into another
void memcpy(void *out, void *in, int size)
{
	byte *pout = out;
	byte *pin = in;
	int i;
	for(i = 0; i < size; i++) {
		*pout = *pin;
		pout++;
		pin++;
	}
}

// Returns string length
int strlen(byte *text)
{
	int i = 0;
	while(*text) {
		text++;
		i++;
	}
	return i;
}

// Copies string src into dest
char *strcpy(char *dest, const char *src)
{	
	while(*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	
	*dest = '\0';
	return dest;
}

// Copies a null-terminated string from a file
// Returns string length
unsigned int fgetsz(SceUID file, char* dest)
{
	int ret;
	char c;
	unsigned int i = 0;

	ret = sceIoRead(file, &c, sizeof(c));

	while ((c != 0) && (ret > 0))
	{
		dest[i++] = c;
		ret = sceIoRead(file, &c, sizeof(c));
	}

	dest[i] = '\0';
	
	return i;	
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

// Searches for a substring in a string
char *strstr(const char * string, const char * substring)
{
	char* strpos;

	if (string == 0)
		return 0;

	if (strlen(substring) == 0)
		return (char*)string;

	strpos = (char*)string;

	while (*strpos != 0)
	{
		if (strncmp(strpos, substring, strlen(substring)) == 0)
		{
			return strpos;
		}

		strpos++;
	}

	return 0;
}

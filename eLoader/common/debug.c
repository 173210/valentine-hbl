#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <exploit_config.h>

#ifdef DEBUG
void dbg_puts(const char *s)
{
	SceUID fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
	size_t len = strlen(s);

	sceIoWrite(PSPLINK_OUT, s, len);

	sceIoWrite(fd, s, len);
	sceIoClose(fd);
}

// DBG_PRINTF implementation

static int dbg_itoa(unsigned val, char *buf, int base, int w)
{
	char *p1, *p2;
	char c;
	int i = 0;
	int rem;

	do {
		rem = val % base;
		buf[i++] = (rem < 10 ? 0x30 : 0x37) + rem;
		val /= base;
		w--;
	} while (val || w > 0);

	p1 = buf;
	p2 = buf + i - 1;
	while (p1 < p2) {
		c = *p2;
		*p2-- = *p1;
		*p1++ = c;
	}

	return i;
}

void dbg_printf(const char *fmt, ...)
{
	SceUID fd;
	va_list va;
	const char *p;
	char buf[11];
	char c, w;
	int i, val;

	va_start(va, fmt);
	fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);

	while ((c = *fmt) != '\0') {
		if (c != '%') {
			p = fmt;

			for (i = 1; fmt[i] != '%' && fmt[i]; i++);
			fmt += i;
		} else {
			fmt++;
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
					p = buf;

					val = va_arg(va, int);
					if (val < 0) {
						buf[0] = '-';
						val= -val;
						i = dbg_itoa((unsigned)val, buf + 1, 10, w);
					} else
						i = dbg_itoa((unsigned)val, buf, 10, w);
					break;
				case 'X' : 
					p = buf;
					i = dbg_itoa((unsigned)va_arg(va, int), buf, 16, w);
					break;
				case 's' :
					p = va_arg(va, const char *);
					i = strlen(p);
					break;
				default:
					p = NULL;
					i = 0;
			}
		}

		sceIoWrite(PSPLINK_OUT, p, i);
		sceIoWrite(fd, p, i);
	}

	va_end(va);
	sceIoClose(fd);
}
#endif


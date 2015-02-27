#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <config.h>

void dbg_puts(const char *s)
{
	SceUID fd;
	size_t len;
	int ret;

	len = strlen(s);

	ret = sceIoWrite(PSPLINK_OUT, s, len);
	if (ret != len)
		dbg_printf("%s: stdout writing failed 0x%08X\n",
			__func__, ret);

	fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
	if (fd < 0)
		dbg_printf("%s: " DBG_PATH " opening failed 0x%08X\n",
			__func__, fd);
	else {
		ret = sceIoWrite(fd, s, len);
		if (ret != len)
			dbg_printf("%s: " DBG_PATH " writing failed 0x%08X\n",
				__func__, ret);
		ret = sceIoClose(fd);
		if (ret)
			dbg_printf("%s: " DBG_PATH " closing failed 0x%08X\n",
				__func__, ret);
	}
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

void dbg_vprintf(const char *fmt, va_list va)
{
	SceUID fd;
	const char dbgOpenError[] = "dbg_vprintf: " DBG_PATH "opening failed\n";
	const char dbgCloseError[] = "dbg_vprintf: " DBG_PATH "closing failed\n";
	const char dbgWriteError[] = "\ndbg_vprintf: " DBG_PATH "writing failed\n";
	const char stdWriteError[] = "\ndbg_vprintf: stdout writing failed\n";
	const char *p;
	char buf[11];
	char c, w;
	int i, ret, val;

	fd = sceIoOpen(DBG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
	if (fd < 0)
		sceIoWrite(PSPLINK_OUT, dbgOpenError, sizeof(dbgOpenError) - 1);

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

		ret = sceIoWrite(PSPLINK_OUT, p, i);
		if (ret != i)
			sceIoWrite(fd, stdWriteError, sizeof(stdWriteError) - 1);
		ret = sceIoWrite(fd, p, i);
		if (ret != i)
			sceIoWrite(PSPLINK_OUT, dbgWriteError, sizeof(dbgWriteError) - 1);
	}

	ret = sceIoClose(fd);
	if (ret)
		sceIoWrite(PSPLINK_OUT, dbgCloseError, sizeof(dbgCloseError) - 1);
}

void dbg_printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	dbg_vprintf(fmt, va);
	va_end(va);
}

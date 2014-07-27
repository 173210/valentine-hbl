#include <common/utils/string.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/sdk.h>
#include <common/utils.h>
#include <exploit_config.h>

#define DEBUG_PATH HBL_ROOT"DBGLOG"

void init_debug()
{
	globals->dbg_fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
}

void log_putc(int c)
{
	sceIoWrite(PSPLINK_OUT, &c, 1);
	sceIoWrite(globals->dbg_fd, &c, 1);
}

void log_puts(const char *s)
{
	size_t len = strlen(s);

	sceIoWrite(PSPLINK_OUT, s, len);
	sceIoWrite(globals->dbg_fd, s, len);
}

// LOG_PRINTF implementation

void log_printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_vfprintf(log_putc, fmt, va);
	va_end(va);
}


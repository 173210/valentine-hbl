#include <common/utils/fnt.h>
#include <common/utils/scr.h>
#include <common/utils/string.h>
#include <common/debug.h>
#include <common/sdk.h>

#define	LINE_SIZE 512
#define SCR_WIDTH 480
#define SCR_HEIGHT 272

static int (* vram)[LINE_SIZE] = (void *)0x04000000;

int cur_x;
int cur_y;

void scr_init()
{
	sceDisplaySetFrameBuf(
		(void *)vram,
		LINE_SIZE,
		PSP_DISPLAY_PIXEL_FORMAT_8888,
		PSP_DISPLAY_SETBUF_NEXTFRAME);
	memset((void *)vram, 0, SCR_HEIGHT * LINE_SIZE * 4);
	cur_x = cur_y = 0;
}

void scr_putc_col(int c, int col)
{
	int fnt_x, fnt_y;

	switch (c) {
		case '\n':
			cur_x = 0;
		case '\v':
			cur_y += FNT_HEIGHT;
			if (cur_y > SCR_HEIGHT)
				cur_y = 0;
		case '\0':
		case '\r':
			break;
		case '\b':
			cur_x -= FNT_WIDTH;
			break;
		case '\t':
			cur_x += FNT_WIDTH * 4;
			break;
		default:
			if (cur_x + FNT_WIDTH > SCR_WIDTH) {
				cur_x = 0;
				cur_y += FNT_HEIGHT;
				if (cur_y < 0)
					cur_y = 0;
			}

			for (fnt_x = 0; fnt_x < FNT_WIDTH; fnt_x++)
				for (fnt_y = 0; fnt_y < FNT_HEIGHT; fnt_y++)
					vram[cur_y + fnt_y][cur_x + fnt_x]
						= (0x80 >> fnt_x) & fnt[c][fnt_y] ? col : 0;

			cur_x += FNT_WIDTH;
	}

	LOG_PUTC(c);
}

void scr_putc(int c)
{
	scr_putc_col(c, 0x00FFFFFF);
}

void scr_puts_col(const char *s, int col)
{
	while (*s) {
		scr_putc_col(*s, col);
		s++;
	}
}

void scr_puts(const char *s)
{
	while (*s) {
		scr_putc(*s);
		s++;
	}
}

void scr_printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_vfprintf(scr_putc, fmt, va);
	va_end(va);
}


#ifndef SCR_H
#define SCR_H

void scr_init();

void scr_puts_col(const char *s, int col);
#define scr_puts(s) scr_puts_col(s, 0x00FFFFFF)

void scr_printf(const char *fmt, ...);

#endif

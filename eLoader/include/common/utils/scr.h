#ifndef SCR_H
#define SCR_H

void scr_init();

void scr_putc_col(int c, int col);
void scr_putc(int c);

void scr_puts_col(const char *s, int col);
void scr_puts(const char *s);

void scr_printf(const char *fmt, ...);

#endif

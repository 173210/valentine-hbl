#ifndef ELOADER_DEBUG
#define ELOADER_DEBUG

#define PSPLINK_OUT 2

//init the debug file
void init_debug();

void log_putc(int c);
void scr_puts(const char *s);
void log_printf(const char *fmt, ...);


#ifdef DEBUG
#define LOG_PUTC(s) log_putc(s)
#define LOG_PRINTF(...) log_printf(__VA_ARGS__)

#else
#define LOG_PUTS(s) {}
#define LOG_PRINTF(...) {}
#endif

#ifdef NID_DEBUG
#define NID_LOG_PRINTF(...) log_printf(__VA_ARGS__)

#else
#define NID_LOG_PRINTF(...) {}
#endif

#endif

#ifndef ELOADER_DEBUG
#define ELOADER_DEBUG

#define PSPLINK_OUT 2

#define DBG_PATH HBL_ROOT"DBGLOG"

#ifdef DEBUG
void dbg_puts(const char *s);
void dbg_printf(const char *fmt, ...);
#else
#define dbg_puts(s) {}
#define dbg_printf(...) {}
#endif

#ifdef NID_DEBUG
#define NID_DBG_PRINTF(...) dbg_printf(__VA_ARGS__)
#else
#define NID_DBG_PRINTF(...) {}
#endif

#endif

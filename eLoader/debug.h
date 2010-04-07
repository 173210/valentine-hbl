#ifndef ELOADER_DEBUG
#define ELOADER_DEBUG

// Comment to avoid generating debug logs
#define DEBUG
#define DEBUG_PATH "ms0:/dbglog"
#define STDOUT 1
#define PSPLINK_OUT 2

// Macro to make a BREAK instruction
#define MAKE_BREAK(n) ((((u32)n << 6) & 0x03ffffc0) | 0x0000000d)

//init the debug file
void init_debug();

// Write a string + data to debug path
void write_debug(const char* description, void* value, unsigned int size);

// Write debug and exit
void exit_with_log(const char* description, void* value, unsigned int size);

#ifdef DEBUG
#define DEBUG_PRINT(a,b,c) write_debug(a,b,c);
#else
#define DEBUG_PRINT(a,b,c) {};
#endif

#endif
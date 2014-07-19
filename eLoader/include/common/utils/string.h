#ifndef ELOADER_STRING
#define ELOADER_STRING

#include <sys/types.h>
#include <stdarg.h>

// Sets a memory region to a specific value
void *memset(void *s, int c, size_t n);

// Copies one memory buffer into another
void *memcpy(void *dst, const void *src, size_t n);

//Scan s for the character.  When this loop is finished,
//    s will either point to the end of the string or the
//    character we were looking for
char *strchr(const char *s, int c);

// Returns string length
size_t strlen(const char *s);

// Copies string src into dest
char *strcpy(char *dst, const char *src);

// limited sprintf function - avoids pulling in large library
void _vsprintf(char *s, const char *fmt, va_list va);
void _sprintf(char *s, const char *fmt, ...);

// Compares s1 with s2, returns 0 if both equal
int strcmp(const char *s1, const char *s2);

// Concatenates string s + append
char *strcat(char *s, const char *append);

// Compares only "count" chars from strings
int strncmp(const char *s1, const char *s2, size_t count);

char * strrchr(const char *cp, int ch);

#endif

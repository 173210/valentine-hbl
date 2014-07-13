#ifndef ELOADER_LIBC
#define ELOADER_LIBC

#include <common/sdk.h>

// Sets a memory region to a specific value
void * memset(void *ptr, int c, size_t size);

// Copies one memory buffer into another
void * memcpy(void *out, const void *in, int size);

//Scan s for the character.  When this loop is finished,
//    s will either point to the end of the string or the
//    character we were looking for
char *(strchr)(const char *s, int c);

// Returns string length
int strlen(const char *text);

// Copies string src into dest
char *strcpy(char *dest, const char *src);

// Copies a null-terminated string from a file
// Returns string length
unsigned int fgetsz(SceUID file, char* dest);

// Returns 1 if a given file exists, 0 otherwise
int file_exists(const char * filename);

void numtohex8(char *dst, int n);
void numtohex4(char *dst, int n);
void numtohex2(char *dst, int n);

// limited sprintf function - avoids pulling in large library
int writeFormat(char *xibuff, const char *xifmt, u32 xidata);
void mysprintf11(char *xobuff, const char *xifmt,
   u32 xidata,
   u32 xidata2,
   u32 xidata3,
   u32 xidata4,
   u32 xidata5,
   u32 xidata6,
   u32 xidata7,
   u32 xidata8,
   u32 xidata9,
   u32 xidata10,
   u32 xidata11);

void mysprintf8(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3, u32 xidata4, u32 xidata5, u32 xidata6, u32 xidata7, u32 xidata8);
void mysprintf4(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3, u32 xidata4);
void mysprintf3(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3);
void mysprintf2(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2);
void mysprintf1(char *xobuff, const char *xifmt, u32 xidata);
void mysprintf0(char *xobuff, const char *xifmt);

// Searches for s1 string in memory
// Returns pointer to string
void *memfindsz(const char *s1, void *p, int size);

// Searches for 32-bit value on memory
// Returns pointer to value
u32* memfindu32(const u32 val, u32* start, unsigned int size);

// Compares s1 with s2, returns 0 if both equal
int strcmp(const char *s1, const char *s2);

// Concatenates string s + append
char *strcat(char *s, const char *append);

// Compares only "count" chars from strings
int strncmp(const char *s1, const char *s2, size_t count);

char * strrchr(const char *cp, int ch);

#endif

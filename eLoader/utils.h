#ifndef _UTILS_H_
#define _UTILS_H_

#include "sdk.h"

#ifndef ULONG
#define ULONG unsigned long
#endif

u32 getFirmwareVersion();
int file_exists(const char * filename);


void mysprintf8(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2, ULONG xidata3, ULONG xidata4, ULONG xidata5, ULONG xidata6, ULONG xidata7, ULONG xidata8);
void mysprintf4(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2, ULONG xidata3, ULONG xidata4);
void mysprintf2(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2);
void mysprintf1(char *xobuff, const char *xifmt, ULONG xidata);
void mysprintf0(char *xobuff, const char *xifmt);

#endif
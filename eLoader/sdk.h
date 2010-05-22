#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudio.h>
#include <psputility.h>

#ifndef VAL_SDK_H
#define VAL_SDK_H

// Typedefs
typedef unsigned char byte;

#ifndef ULONG
#define ULONG unsigned long
#endif

#define EXIT sceKernelExitGame()

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif


#endif


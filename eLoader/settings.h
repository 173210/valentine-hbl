// Initial code thanks to Fanjita and N00bz
// settings.h : Settings variables read from file
//

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

// Variables for overriding functions
// Override overrdies with a hook if available
// Generic success overrides with a function that returns 0
#define DONT_OVERRIDE 0
#define OVERRIDE 1
#define GENERIC_SUCCESS -1


void loadConfig(const char * file);
void loadGlobalConfig();

#if INSTANTIATE_CONFIG_VARS==1
#define MAYBE_EXTERN
#else
#define MAYBE_EXTERN extern
#endif

/*****************************************************************************/
/* Special case control variables read from file                             */
/*****************************************************************************/
MAYBE_EXTERN int     g_override_sceKernelUtilsMd5Digest;
MAYBE_EXTERN int     g_override_sceIoMkdir;
MAYBE_EXTERN char *  g_hb_folder;


#endif

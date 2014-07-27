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

extern int override_sceIoMkdir;
extern int override_sceCtrlPeekBufferPositive;
extern int return_to_xmb_on_exit;
extern unsigned int force_exit_buttons;
extern char hb_fname[];


void loadConfig(const char * file);
void loadGlobalConfig();


#endif

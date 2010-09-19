#include "utils.h"
#include "debug.h"
#include "globals.h"
#include <exploit_config.h>

// "cache" for the firmware version
// 1 means: not set yet
// 0 means unknown

// cache for the psp model
// 0 means unknown
// 1 means not set yet
// 2 is PSPGO
// 3 is other

/* Attempt to get the firmware version by checking a specific address in Ram
 * The address and values are based on a series of memdump that can mostly be found here:
 * http://advancedpsp.tk/foro_es/viewtopic.php?f=37&t=293
 *
 * It is important to call this function (once) early, before that point in memory gets erased
 */
u32 getFirmwareVersion()
{
    tGlobals * g = get_globals();

	if (g->firmware_version != 1) 
		return g->firmware_version;

    g->firmware_version = 0;
    
#ifdef DETECT_FIRMWARE_ADDR    
    u32 value = *(u32*)DETECT_FIRMWARE_ADDR;

    switch (value) 
    {
#ifdef DETECT_FIRMWARE_500    
		case DETECT_FIRMWARE_500:
		    g->firmware_version = 500;
		    break;  
#endif
#ifdef DETECT_FIRMWARE_503            
		case DETECT_FIRMWARE_503:
		    g->firmware_version = 503;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_550              
		case DETECT_FIRMWARE_550:
		    g->firmware_version = 550;
		    break;
#endif
#ifdef DETECT_FIRMWARE_551              
		case DETECT_FIRMWARE_551:
		    g->firmware_version = 551;
		    break;
#endif
#ifdef DETECT_FIRMWARE_555              
		case DETECT_FIRMWARE_555:
		    g->firmware_version = 555;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_570              
		case DETECT_FIRMWARE_570:
		    g->firmware_version = 570;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_600              
		case DETECT_FIRMWARE_600:
		    g->firmware_version = 600;
		    break;
#endif
#ifdef DETECT_FIRMWARE_610              
		case DETECT_FIRMWARE_610:
		    g->firmware_version = 610;
		    break;     
#endif
#ifdef DETECT_FIRMWARE_620              
		case DETECT_FIRMWARE_620:
		    g->firmware_version = 620;
		    break;          
#endif
#ifdef DETECT_FIRMWARE_630 
		case DETECT_FIRMWARE_630:
		    g->firmware_version = 630;
		    break;    
#endif
#ifdef DETECT_FIRMWARE_631
		case DETECT_FIRMWARE_631:
		    g->firmware_version = 631;
		    break;     
#endif            
    }
#endif
    return g->firmware_version; 
}

u32 getPSPModel()
{
    tGlobals * g = get_globals();

    if (g->psp_model!= 1)
		return g->psp_model;

    g->psp_model = 0;

	// This call will always fail, but with a different error code depending on the model
	SceUID result = sceIoOpen("ef0:/", 1, 0777);

	// Check for "No such device" error
	g->psp_model = (result == (int)0x80020321) ? PSP_OTHER : PSP_GO;	

    return g->psp_model; 
}

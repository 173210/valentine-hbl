#include "utils.h"
#include "debug.h"
#include <exploit_config.h>

//TODO move these global variables into g!!!

// "cache" for the firmware version
// 1 means: not set yet
// 0 means unknown
u32 firmware_version = 1;

// cache for the psp model
// 0 means unknown
// 1 means not set yet
// 2 is PSPGO
// 3 is other
u32 psp_model = 1;

/* Attempt to get the firmware version by checking a specific address in Ram
 * The address and values are based on a series of memdump that can mostly be found here:
 * http://advancedpsp.tk/foro_es/viewtopic.php?f=37&t=293
 *
 * It is important to call this function (once) early, before that point in memory gets erased
 */
u32 getFirmwareVersion()
{
	if (firmware_version != 1) return firmware_version;

    firmware_version = 0;
    
#ifdef DETECT_FIRMWARE_ADDR    
    u32 value = *(u32*)DETECT_FIRMWARE_ADDR;

    switch (value) 
    {
#ifdef DETECT_FIRMWARE_500    
		case DETECT_FIRMWARE_500:
		    firmware_version = 500;
		    break;  
#endif
#ifdef DETECT_FIRMWARE_503            
		case DETECT_FIRMWARE_503:
		    firmware_version = 503;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_550              
		case DETECT_FIRMWARE_550:
		    firmware_version = 550;
		    break;
#endif
#ifdef DETECT_FIRMWARE_551              
		case DETECT_FIRMWARE_551:
		    firmware_version = 551;
		    break;
#endif
#ifdef DETECT_FIRMWARE_555              
		case DETECT_FIRMWARE_555:
		    firmware_version = 555;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_570              
		case DETECT_FIRMWARE_570:
		    firmware_version = 570;
		    break;   
#endif
#ifdef DETECT_FIRMWARE_600              
		case DETECT_FIRMWARE_600:
		    firmware_version = 600;
		    break;
#endif
#ifdef DETECT_FIRMWARE_610              
		case DETECT_FIRMWARE_610:
		    firmware_version = 610;
		    break;     
#endif
#ifdef DETECT_FIRMWARE_620              
		case DETECT_FIRMWARE_620:
		    firmware_version = 620;
		    break;          
#endif
#ifdef DETECT_FIRMWARE_630 
		case DETECT_FIRMWARE_630:
		    firmware_version = 630;
		    break;    
#endif
#ifdef DETECT_FIRMWARE_631
		case DETECT_FIRMWARE_631:
		    firmware_version = 631;
		    break;     
#endif            
    }
#endif
    return firmware_version; 
}

u32 getPSPModel()
{
    if (psp_model!= 1) return psp_model;

    psp_model = 0;
#ifdef DETECT_MODEL_ADDR 
    u32 value = *(u32*)DETECT_MODEL_ADDR;
    value = value >> 24;

    switch (value) 
    {
		case 5:
		    psp_model = PSP_GO;
		    break;            
		case 4:
		    psp_model = PSP_OTHER;
		    break;           
    }
#endif    
    return psp_model; 
}

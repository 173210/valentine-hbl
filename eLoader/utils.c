#include "utils.h"
#include "debug.h"

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
 * This code is experimental, it hasn't been run on "enough" psps to be sure it works
 */
u32 getFirmwareVersion()
{
    if (firmware_version != 1) return firmware_version;

    firmware_version = 0;
    u32 value = *(u32*)0x09E7b68c;

    switch (value) 
    {
		case 0xB533E9FC:
		    firmware_version = 500;
		    break;  
		case 0x4CFA7F33:
		    firmware_version = 550; //actually 5.03
		    break;               
		case 0xA67D3F99:
		    firmware_version = 550;
		    break;
		case 0x67D3F99F:
		    firmware_version = 555; //actually 5.51
		    break;
		case 0xCFA7F33F:
		    firmware_version = 555;
		    break;   
		case 0xC5B13597:
		    firmware_version = 570;
		    break;   
		case 0x22B5CE0D:
		    firmware_version = 600;
		    break;
		case 0x32A80E1B  :
		    firmware_version = 610;
		    break;     
		case 0x73880F1B  :
		    firmware_version = 620;
		    break;            
    }
    return firmware_version; 
}

u32 getPSPModel()
{
    if (psp_model!= 1) return psp_model;

    psp_model = 0;
    u32 value = *(u32*)0x08B46140;
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
    return psp_model; 
}


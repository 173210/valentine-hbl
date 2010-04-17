#include "utils.h"
#include "debug.h"

// "cache" for the firmware version
// 1 means: not set yet
// 0 means unknown
u32 firmware_version = 1;

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
    case 0x22B5CE0D:
        firmware_version = 600;
        break;
    case 0x67D3F99F:
        firmware_version = 550; //actually 5.51
        break;
    case 0xA67D3F99:
        firmware_version = 550;
        break;
    case 0xB533E9FC:
        firmware_version = 500;
        break;    
    case 0xC5B13597:
        firmware_version = 570;
        break;   
    case 0x73880F1B  :
        firmware_version = 620;
        break;  
    case 0x32A80E1B  :
        firmware_version = 610;
        break;              
    }
    return firmware_version; 
}

// Returns 1 if a given file exists, 0 otherwise
int file_exists(const char * filename)
{
    SceUID id = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    if (id < 0) 
		return 0;
    sceIoClose(id);
    return 1;
}
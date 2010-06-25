// Initial code thanks to Fanjita and N00bz
// settings.c : Settings variables read from file
//

#include "settings.h"
#include "eloader.h"
#include "debug.h"
#include "lib.h"
#include "graphics.h"
#include "globals.h"

/*****************************************************************************/
/* configYnParse : return TRUE if parameter is Y, FALSE if N                 */
/*****************************************************************************/
int configYnParse(const char *xival)
{
    if ((xival[0] == 'y') ||
        (xival[0] == 'Y'))
    {
        return(1);
    } 
    else
    {
        return(0);
    }
}

/*****************************************************************************/
/* configIntParse : return simple integer value (unsigned, currently)        */
/*****************************************************************************/
int configIntParse(const char *xival)
{
    int   lrunningvalue = 0;
    char *lptr = (char *)xival;
    int minus = 1;
    
    if(*lptr == '-')
    {
        lptr++;
        minus = -1;
    }
  
    while ((*lptr != 0) &&
         (*lptr >= '0') &&
         (*lptr <= '9'))
    {
        lrunningvalue *= 10;
        lrunningvalue += (*lptr) - '0';

        lptr++;
    }

    lrunningvalue *= minus;
    LOGSTR1("Parsed int param: %d\n", lrunningvalue);

    return(lrunningvalue);
}

/*****************************************************************************/
/* configAddrParse : return 32-bit hex address                               */
/*****************************************************************************/
u32 configAddrParse(const char *xival)
{
    u32 lrunningvalue = 0;
    char *lptr = (char *)xival;
    int   lhexdigit;

    /***************************************************************************/
    /* If the string is just '0', then return 0.                               */
    /***************************************************************************/
    if ((lptr[0] == '0') &&
        (lptr[1] == '\0'))
    {
        return(0);
    }

    /***************************************************************************/
    /* The start of the string should be '0x'.                                 */
    /***************************************************************************/
    if ((lptr[0] != '0') ||
        (lptr[1] != 'x'))
    {
        PRTSTR1("Invalid address format in config.  Address must start with '0x'. Entry: %s", (u32)lptr);
    }

    lptr += 2;

    while ((*lptr != 0) &&
            (((*lptr >= '0') && (*lptr <= '9'))   ||
            ((*lptr >= 'A') && (*lptr <= 'F'))))
    {
        if ((*lptr >= 'A') &&
            (*lptr <= 'F'))
        {
            lhexdigit = (*lptr) - 'A' + 10;
        }
        else
        {
            lhexdigit = (*lptr) - '0';
        }
        lrunningvalue *= 16;
        lrunningvalue += lhexdigit;

        lptr++;
    }

    LOGSTR1("Parsed addr param: %08lX\n", lrunningvalue);

    return(lrunningvalue);
}


SceUID gconfigfd = -1;
u32  gconfigoffset = 0;

/*****************************************************************************/
/* readLine : Read a line from a file, updating the tracked offset.          */
/*****************************************************************************/
int readLine(char *xobuff, int xibufflen, SceUID xifd, u32 *xboffset)
{
    int   lrc;
    char *lptr;

    lrc = sceIoRead(xifd, xobuff, xibufflen);
    if (lrc <= 0)
    {
        /*************************************************************************/
        /* Error reading.                                                        */
        /*************************************************************************/
        lrc = 0;
    }
    else
    {
        if (lrc < xibufflen)
        {
            xobuff[lrc] = '\0';
        }
        else
        {
            xobuff[xibufflen - 1] = '\0';
        }

        lptr = strchr(xobuff, '\n');
        if (lptr)
        {
            *lptr = '\0';
        }

        lptr = strchr(xobuff, '\r');
        if (lptr)
        {
            *lptr = '\0';
        }

        /*************************************************************************/
        /* update xboffset, seek ready for next line                             */
        /*************************************************************************/
        *xboffset += strlen(xobuff) + strlen("\n");
        sceIoLseek(xifd, *xboffset, PSP_SEEK_SET);

        lrc = 1;
    }

    return(lrc);
}

/*****************************************************************************/
/* configReadParameter : read a line from the file, repeating until we       */
/* get a 'name=val' line.  Abort if we get a line starting with '['.         */
/*                                                                           */
/* Return non-zero if we got some data.                                      */
/*****************************************************************************/
int configReadParameter(char *xoname, char *xoval)
{
    char lbuff[256];
    int  lrc;
    int  ldone = 0;
    char *lptr;

    do
    {
        lrc = readLine(lbuff, 256, gconfigfd, &gconfigoffset);
        if (lrc)
        {
            //    LOGSTR1("Got line: %s\n", lbuff);
            if (lbuff[0] != '#')
            {
                if (lbuff[0] == '[')
                {
                    lrc = 0;
                    ldone = 1;
                }
                else
                {
                    lptr = strchr(lbuff, '=');
                    if (lptr)
                    {
                        *lptr = '\0';
                        strcpy(xoname, lbuff);
                        strcpy(xoval, lptr + 1);
                        ldone = 1;
                    }
                }
            }
        }
    } while (!ldone && lrc);

    return(lrc);
}

void configGetProcessingOptions()
{
    char lstr[256];
    char lval[256];

    tGlobals * g = get_globals();
    LOGSTR0("Read params\n");
    while (configReadParameter(lstr, lval))
    {
        LOGSTR2("Parm %s = %s\n", (u32)lstr, (u32)lval);
        if (strcmp(lstr,"override_sceIoMkdir")==0)
        {
            g->override_sceIoMkdir = configIntParse(lval);
        }
        else if (strcmp(lstr,"return_to_xmb_on_exit")==0)
        {
            g->return_to_xmb_on_exit = configIntParse(lval);
        }        
        else if (strcmp(lstr,"hb_folder")==0)
        {
            //note: g->hb_folder is initialized in loadGlobalConfig
            strcpy(g->hb_filename,lval);
        }
        else
        {
            PRTSTR1("Unrecognised config parameter: %s", (u32)lstr);
        }
    }
}


// Load default config
void loadGlobalConfig()
{
    tGlobals * g = get_globals();
    //default values
    g->override_sceIoMkdir = DONT_OVERRIDE;
    g->return_to_xmb_on_exit = 0;
    strcpy(g->hb_filename, "ms0:/PSP/GAME/");
    
    //load Config file
    loadConfig(HBL_ROOT HBL_CONFIG);
}

void closeConfig() {
    if (gconfigfd >= 0)
        sceIoClose(gconfigfd);
    gconfigfd = -1;
    gconfigoffset = 0;
    
}

/*****************************************************************************/
/* Locate config file, read basic info.                                      */
/*                                                                           */
/* Design constraints : minimise mem usage, so must be able to read required */
/* info on the fly as required.                                              */
/*                                                                           */
/* Laid out as .ini file.  Use '#' at the start of the line for comments     */
/*                                                                           */
/* Read this config here:                                                    */
/*                                                                           */
/* [global]                                                                  */
/* menu=menu EBOOT filename                                                  */
/*                                                                           */
/* [defaults]                                                                */
/* (as per per-module config below)                                          */
/*                                                                           */
/*****************************************************************************/
void loadConfig(const char * path)
{
    LOGSTR1("Attempt to Load Config file: %s\n", (u32)path);
    closeConfig();
        
    /***************************************************************************/
    /* Open file, read globals and configured defaults                         */
    /***************************************************************************/
    gconfigfd = sceIoOpen(path, PSP_O_RDONLY, 0777);
    if (gconfigfd < 0 )
    {
        LOGSTR1("Error Loading 0x%08lX\n",gconfigfd);
        return;
    }
    print_to_screen("Config file:");
    print_to_screen(path);

    configGetProcessingOptions();
    closeConfig();
}

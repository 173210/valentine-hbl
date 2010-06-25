#include "globals.h"
#include "debug.h"

tGlobals * get_globals() 
{
    return (tGlobals *) GLOBALS_ADDR;
}

void init_globals()
{
    u32 size = sizeof(tGlobals);
    if ((u32)GLOBALS_ADDR < 0x14000 
        && size + (u32)GLOBALS_ADDR >= 0x14000)
    {
        logstr1("Globals structure doesn't fit in Scratchpad RAM: 0x%08lX\n", size);
        exit_with_log("Fatal error: exiting", NULL, 0);
    }
    tGlobals * g = get_globals();
    memset(g, 0, sizeof(tGlobals)); 
    strcpy(g->menupath,"ms0:/hbl/menu/EBOOT.PBP");
    strcpy(g->menubackground, "ms0:/hbl/menu.png");
    
    g->memSema = sceKernelCreateSema("hblmemsema",0,1,1,0);
    g->thrSema = sceKernelCreateSema("hblthrsema",0,1,1,0);
    g->cbSema = sceKernelCreateSema("hblcbsema",0,1,1,0);
    g->audioSema = sceKernelCreateSema("hblaudiosema",0,1,1,0);
    g->curr_channel_id = -1;
}
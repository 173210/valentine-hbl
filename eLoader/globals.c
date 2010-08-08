#include "globals.h"
#include "debug.h"
#include "utils.h"

tGlobals * get_globals() 
{
    return (tGlobals *) GLOBALS_ADDR;
}

void init_globals()
{
    if ((u32)GLOBALS_ADDR < 0x14000)
    {
        if (HBL_STUBS_START + NUM_HBL_IMPORTS*2*sizeof(u32) > (u32)GLOBALS_ADDR)
        {
            exit_with_log("FATAL: Increase GLOBALS_ADDR", NULL, 0);
        }
        
        u32 size = sizeof(tGlobals); 
        if (size + (u32)GLOBALS_ADDR >= 0x14000)
        {
            logstr1("Globals structure doesn't fit in Scratchpad RAM: 0x%08lX\n", size);
            exit_with_log("FATAL: Globals too big", NULL, 0);
        }
    }
    tGlobals * g = get_globals();
    memset(g, 0, sizeof(tGlobals)); 
    strcpy(g->menupath,"ms0:/hbl/menu/EBOOT.PBP");

	// Select syscall estimation method
	// For later I would propose a per-library flag.    
    g->syscalls_known = ((getFirmwareVersion() <= 610) || (getPSPModel() == PSP_GO));

    g->memSema = sceKernelCreateSema("hblmemsema",0,1,1,0);
    g->thrSema = sceKernelCreateSema("hblthrsema",0,1,1,0);
    g->cbSema = sceKernelCreateSema("hblcbsema",0,1,1,0);
    g->audioSema = sceKernelCreateSema("hblaudiosema",0,1,1,0);
    g->curr_channel_id = -1;
}
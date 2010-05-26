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
        logstr1("globals structure doesn't fit in Scratchpad RAM: 0x%08lX\n", size);
        exit_with_log("Fatal error: exiting", NULL, 0);
    }
    tGlobals * globals = get_globals();
    memset(globals, 0, sizeof(tGlobals)); 
}
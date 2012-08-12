#include "sdk.h"
#include "debug.h"
#include "utils.h"
#include "runtime_stubs.h"
#include <exploit_config.h>

// If we want to load additional modules in advance to use their syscalls
#ifdef LOAD_MODULES_FOR_SYSCALLS
#ifndef AUTO_SEARCH_STUBS
#define AUTO_SEARCH_STUBS
#endif
#endif

#ifdef AUTO_SEARCH_STUBS
int strong_check_stub_entry(tStubEntry* pentry)
{
	return ( 
    elf_check_stub_entry(pentry) && 
    (pentry->import_stubs && (pentry->import_stubs < 100)) &&
    (pentry->stub_size && (pentry->stub_size < 100)) &&
    ((pentry->import_flags == 0x11) || (pentry->import_flags == 0))
    );
}

int search_stubs(u32 * stub_addresses) {
    u32 start = 0x08800000;
    u32 end = start + 0x01800000;
    int current = 0;
    
    u32 i = start;
    while (i < end) {
        tStubEntry* pentry = (tStubEntry*)i;
        if (strong_check_stub_entry(pentry)) {  
            //boundaries check
            if (current >= MAX_RUNTIME_STUB_HEADERS)
            {
                LOGSTR0("More stubs than Maximum allowed number, won't enumerate all stubs\n");
                return current;
            }
            stub_addresses[current] = i;
            current++; 
            LOGSTR1("Found Stubs at 0x%08lX\n", (u32)pentry);

            // We found a valid pentry, skip consecutive valid pentry objects
            while ((i < end) && elf_check_stub_entry(pentry))  {
                pentry++;
                i = (u32)pentry;          
            }
        }
        i+=4;
    }
    return current;
}

void load_modules_for_stubs() {
#ifdef MODULES
 	unsigned int moduleIDs[] = MODULES;
    int i;
    
    // Load modules in order
    for(i = 0; (u32)i < sizeof(moduleIDs)/sizeof(u32); i++)
    {    
        unsigned int modid = moduleIDs[i];
        LOGSTR1("Loading 0x%08lX\n", (u32)(modid) );
        int result = sceUtilityLoadModule(modid);
        if (result < 0)
        {
            LOGSTR2(((u32)result == 0x80111102)?"...Already loaded\n":"...Error 0x%08lX Loading 0x%08lX\n", (u32)result, (u32)(modid) );
        }
    }
#endif	
}

void unload_modules_for_stubs() {
#ifdef MODULES
    unsigned int moduleIDs[] = MODULES;
    int i;
    //Unload modules in reverse order
    for(i = sizeof(moduleIDs)/sizeof(u32) - 1 ; i >= 0; i--)
    {    
        unsigned int modid = moduleIDs[i];
        LOGSTR1("UnLoading 0x%08lX\n", (u32)(modid) );
#ifndef HOOK_sceUtilityUnloadModule
    	if( !(modid == PSP_MODULE_AV_MP3 && getFirmwareVersion() <= 620) )
    	{
	        int result = sceUtilityUnloadModule(modid);
	        if (result < 0)
	        {
	            LOGSTR2("...Error 0x%08lX Unloading 0x%08lX\n", (u32)result, (u32)(modid) );
	        }
    	}
#endif	
    }
#endif	
}

#else
// Dummy functions
int search_stubs(u32 * stub_addresses) {
    return stub_addresses ? -1 : -2;
}
void load_modules_for_stubs() {}
void unload_modules_for_stubs() {}
#endif

#ifndef COMMON_PATH
#define COMMON_PATH

#include <exploit_config.h>

//
//Files locations
//
#ifndef HBL_ROOT
#define HBL_ROOT "ms0:/hbl/"
#endif
#define HBL_PRX "HBL.PRX"
// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT "GAME/EBOOT.PBP"
#ifdef VITA
#define MENU_PATH HBL_ROOT "EBOOT.PBP"
#else
#define MENU_PATH HBL_ROOT "GAME/EBOOT.PBP"
#endif
#define ELF_PATH HBL_ROOT "GAME/EBOOT.ELF"
#define HBL_PATH HBL_ROOT HBL_PRX
#define KDUMP_PATH "ef0:/KMEM.BIN" // Always dump to the internal flash
#define HBL_CONFIG "HBLCONF.TXT"

#endif


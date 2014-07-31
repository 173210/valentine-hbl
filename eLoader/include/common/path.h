#ifndef COMMON_PATH
#define COMMON_PATH

#include <exploit_config.h>

//
//Files locations
//
#ifndef HBL_ROOT
#define HBL_ROOT "ms0:/hbl/"
#endif
#define HBL_BIN "hbl.bin"
// Fixed path for EBOOT loading (used if no menu available)
#define EBOOT_PATH HBL_ROOT"GAME/EBOOT.PBP"
#ifdef VITA
#define MENU_PATH HBL_ROOT"EBOOT.PBP"
#else
#define MENU_PATH HBL_ROOT"GAME/EBOOT.PBP"
#endif
#define ELF_PATH HBL_ROOT"GAME/eboot.elf"
#define HBL_PATH HBL_ROOT HBL_BIN
#define KDUMP_PATH "ef0:/kmem.dump" // Always dump to the internal flash (fixes issue 43)
#define HBL_CONFIG "hblconf.txt"

#endif


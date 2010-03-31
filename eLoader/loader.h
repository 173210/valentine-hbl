#ifndef ELOADER_LOADER
#define ELOADER_LOADER

#define ELOADER_PATH "ms0:/hbl.bin"

// Where to load HBL
#define LOADER_BUFFER 0x00010000 
// 0x08804000
// Address of stubs on scratchpad
#define SCRATCHPAD_STUBS_START  0x00013F00
//0x0880FF00

// HBL stubs
// If this value is changed, you should also change it in imports.config
// Should get rid of this
#define NUM_HBL_IMPORTS 0x1B 

// IMPORTANT: (0x00014000 - SCRATCHPAD_STUBS_START) / 8 should be > NUM_HBL_IMPORTS

#define MAX_ELOADER_SIZE (u32)SCRATCHPAD_STUBS_START - (u32)LOADER_BUFFER

#endif

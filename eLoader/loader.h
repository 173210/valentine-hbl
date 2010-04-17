#ifndef ELOADER_LOADER
#define ELOADER_LOADER

// HBL stubs
// If this value is changed, you should also change it in imports.config
// Should get rid of this

#define NUM_HBL_IMPORTS 0x24

// IMPORTANT: (0x00014000 - SCRATCHPAD_STUBS_START) / 8 should be > NUM_HBL_IMPORTS

// 32 kB should be enough :P
#define MAX_ELOADER_SIZE 0x8000

#endif

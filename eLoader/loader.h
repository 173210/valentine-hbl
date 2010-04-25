#ifndef ELOADER_LOADER
#define ELOADER_LOADER

// HBL stubs
// If this value is changed, you should also change it in imports.config
// Should get rid of this

#define NUM_HBL_IMPORTS 0x28

// 32 kB should be enough :P
// Eternal computing dilemma: there's never enough xD
// Set to 64 KiB
// Removed intermediate buffer, ab5000's allocation works just fine
// #define MAX_ELOADER_SIZE 0x10000

#endif

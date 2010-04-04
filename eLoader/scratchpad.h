#ifndef ELOADER_SCRATCH
#define ELOADER_SCRATCH

/* Values in scratchpad:
	0x00010004: HBL memory block UID
	0x00010008: HBL stubs memory block UID
	0x00010014: HBL memory block head address
	0x00010018: HBL stubs memory block head address
*/

#define ADDR_HBL_BLOCK_UID 0x00010004
#define ADDR_HBL_STUBS_BLOCK_UID 0x00010008
#define ADDR_HBL_BLOCK_ADDR 0x00010014
#define ADDR_HBL_STUBS_BLOCK_ADDR 0x00010018

#endif

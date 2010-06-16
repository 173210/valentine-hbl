#ifndef _MENU_H_
#define _MENU_H_

// Those are random addresses. Anything's fine as long as it doesn't overwrite KernelLib (0x08800000 -> 0x08804000)
// Or the HBL itself.
// If the menu gets destroyed after the homebrew is loaded, it doesn't matter

#define EBOOT_SET_ADDRESS 0x08D14000
#define EBOOT_PATH_ADDRESS 0x08D15000
#define MENU_LOAD_ADDRESS 0x08D20000 //see linker_menu.x!


#define FOLDERNAME_SIZE 30
#define NB_FOLDERS 40
#define MAXMENUSIZE 17

#define MAX_INDEX_TABLE_SIZE 11 // max number of entries in psf table 
#define MAX_NAME_SIZE 59
#define FILE_BUFFER_SIZE 80

// index table offsets
#define OFFSET_KEY_NAME 0
#define DATA_ALIGN 2
#define DATA_TYPE 3
#define SIZE_OF_VALUE 4
#define SIZE_DATA_AND_PADDING 8
#define OFFSET_DATA_VALUE 12

#endif

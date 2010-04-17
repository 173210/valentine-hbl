#ifndef _MENU_H_
#define _MENU_H_

// Those are random addresses. Anything's fine as long as it doesn't overwrite KernelLib (0x08800000 -> 0x08804000)
// Or the HBL itself.
// If the menu gets destroyed after the homebrew is loaded, it doesn't matter

#define EBOOT_SET_ADDRESS 0x08D14000
#define EBOOT_PATH_ADDRESS 0x08D15000
#define MENU_LOAD_ADDRESS 0x08D20000 //see linker_menu.x!

#endif
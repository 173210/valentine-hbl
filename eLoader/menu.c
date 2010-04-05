/* Half Byte Loader is GPL
* Crappy Menu(tm) wololo (One day we'll get something better)
* thanks to N00b81 for the graphics library!
* 
*/
#include "sdk.h"
#include "debug.h"
#include "menu.h"

char * currentPath = "ms0:/PSP/GAME/";

int currentFile;;
u32 * isSet;
void * ebootPath;
int nbFiles;

typedef union {
	int rgba;
	struct {
		byte r;
		byte g;
		byte b;
		byte a;
	} c;
} color_t;

void *fb = (void *)0x44000000;

void SetColor(int col)
{
	int i;
	color_t *pixel = (color_t *)fb;
	for(i = 0; i < 512*272; i++) {
		pixel->rgba = col;
		pixel++;
	}
}


/* Yes. This reads the memory stick EVERY time
the user presses a key.
Yes, I'll fix that
*/
void refreshMenu() {
  DEBUG_PRINT("MENU Refresh", NULL, 0);
  DEBUG_PRINT("currentFile ID:", &currentFile, sizeof(int));
  SceUID id = sceIoDopen(currentPath);
  if (id <=0) {
    DEBUG_PRINT("FATAL: Menu can't open directory ", NULL, 0);
    return;
  }
  SceIoDirent entry;
  memset(&entry, 0, sizeof(SceIoDirent)); 
  int i = 0;
  while (sceIoDread(id, &entry) > 0)
  {
    if (strcmp(".", entry.d_name) == 0 || strcmp("..", entry.d_name) == 0) {
        memset(&entry, 0, sizeof(SceIoDirent)); 
        continue;
    }
      
    u32 color = 0x00FFFFFF;
    if (i == currentFile)
        color = 0x0000FFFF;
    printTextScreen(0, 15 + i * 12, entry.d_name, color);
    i++;
    memset(&entry, 0, sizeof(SceIoDirent)); 
  }
  nbFiles = i;
  sceIoDclose(id); 
}

void setEboot() {
DEBUG_PRINT("MENU SET EBOOT", NULL, 0);
  SceUID id = sceIoDopen(currentPath);
  SceIoDirent entry;
    memset(&entry, 0, sizeof(SceIoDirent)); 
  int i = 0;
  while (sceIoDread(id, &entry) > 0)
  {
    if (strcmp(".", entry.d_name) == 0 || strcmp("..", entry.d_name) == 0) {
        memset(&entry, 0, sizeof(SceIoDirent)); 
        continue;
    }  
    if (i == currentFile)
        break;
    i++;
    memset(&entry, 0, sizeof(SceIoDirent)); 
  }
  sceIoDclose(id); 
  if (i != currentFile)
    return;
  strcpy(ebootPath, currentPath);
  strcat(ebootPath, &(entry.d_name));
  strcat(ebootPath, "/EBOOT.PBP");
  isSet[0] = 1;
}

void _start() __attribute__ ((section (".text.start")));
void _start()
{
    currentFile = 0;
    isSet = (u32 *) EBOOT_SET_ADDRESS;
    ebootPath = (void *) EBOOT_PATH_ADDRESS;
    DEBUG_PRINT("START MENU", NULL, 0);
    isSet[0] = 0;
    nbFiles = 0;

    DEBUG_PRINT("MENU Sets display", NULL, 0);
    sceDisplaySetFrameBuf(fb, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

    SetColor(0x00000000);

    refreshMenu();
    if (!nbFiles){
        printTextScreen(100, 200 , "Error, no Files found (sceIoDopen syscall estimate failure?)", 0x00FF0000);
    }     
    sceKernelDelayThread(100000);
    while (!isSet[0]) {    
        SceCtrlData pad; // variable to store the current state of the pad

        sceCtrlReadBufferPositive (&pad, 1); // check the input.
        if(pad.Buttons & PSP_CTRL_CROSS) { // if the cross button is pressed
            setEboot();
        }else if(pad.Buttons & PSP_CTRL_DOWN && currentFile < nbFiles - 1) {
            currentFile++;
            refreshMenu();
        }else if(pad.Buttons & PSP_CTRL_UP && currentFile > 0) {
            currentFile--;
            refreshMenu();
        }else if(pad.Buttons & PSP_CTRL_TRIANGLE) {
            sceKernelExitGame();
        }

        printTextScreen(220, 0 , "X to select, /\\ to quit", 0x00FFFFFF);
        printTextScreen(0, 236 , "HALF-Byte Loader by m0skit0, ab5000, and wololo", 0x00FFFFFF);
        printTextScreen(0, 248 , "Thanks to N00b81, Tyranid, devs of the PSPSDK, Hitmen", 0x00FFFFFF);
        printTextScreen(0, 260 , "GPL License: give the sources if you distribute binaries!!!", 0x00FFFFFF);
        
        sceKernelDelayThread(100000);
    }

    //This point is reached when the value is set
}



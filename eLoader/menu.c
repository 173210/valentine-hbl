/* Half Byte Loader is GPL
* Crappy Menu(tm) wololo (One day we'll get something better)
* thanks to N00b81 for the graphics library!
* 
*/
#include "sdk.h"
#include "debug.h"
#include "menu.h"

#define FOLDERNAME_SIZE 30
#define NB_FOLDERS 20
char folders[NB_FOLDERS][FOLDERNAME_SIZE] ;

char * currentPath = "ms0:/PSP/GAME/";
char * cacheFile = "ms0:/menu.cache";

int currentFile;;
u32 * isSet;
void * ebootPath;
int nbFiles;

void *frameBuffer = (void *)0x44000000;


void saveCache(){
    SceUID id = sceIoOpen(cacheFile, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
    if (id < 0) return;
    sceIoWrite(id, &folders, FOLDERNAME_SIZE * NB_FOLDERS * sizeof(char));
    sceIoClose(id);
}
/*Loads an alternate cached version of the folders
* if sceIoDopen failed
*/
void loadCache(){
    SceUID id = sceIoOpen(cacheFile, PSP_O_RDONLY, 0777);
    if (id < 0) return;
    sceIoRead(id, &folders, FOLDERNAME_SIZE * NB_FOLDERS * sizeof(char));
    sceIoClose(id);
    
    int i;
    nbFiles = 0;
    for (i = 0; i < NB_FOLDERS; ++i){
        if (folders[i][0] == 0) 
        {
            break;
        }
        nbFiles++;
    }    
}

void init(){
  int i;
  nbFiles = 0;
  for (i = 0; i < NB_FOLDERS; ++i){
    folders[i][0] = 0;
  }
    
  SceUID id = sceIoDopen(currentPath);
  if (id <=0) {
    DEBUG_PRINT("FATAL: Menu can't open directory ", NULL, 0);
    loadCache();
    return;
  }
  SceIoDirent entry;
  memset(&entry, 0, sizeof(SceIoDirent)); 
  while (sceIoDread(id, &entry) > 0 && nbFiles < NB_FOLDERS)
  {
    if (strcmp(".", entry.d_name) == 0 || strcmp("..", entry.d_name) == 0) {
        memset(&entry, 0, sizeof(SceIoDirent)); 
        continue;
    }
      
    strcpy(folders[nbFiles], entry.d_name);
    nbFiles++;
    memset(&entry, 0, sizeof(SceIoDirent)); 
  }
  sceIoDclose(id); 
  if (!nbFiles) {
    loadCache();
  }
    
  saveCache();
}

void refreshMenu() {
  int i;
  for (i = 0; i < nbFiles; ++i) {
    u32 color = 0x00FFFFFF;
    if (i == currentFile)
        color = 0x0000FFFF;
    printTextScreen(0, 15 + i * 12, folders[i], color);
  }
}

void setEboot() {
  strcpy(ebootPath, currentPath);
  strcat(ebootPath, folders[currentFile]);
  strcat(ebootPath, "/EBOOT.PBP");
  DEBUG_PRINT("MENU SET EBOOT", &ebootPath, strlen(ebootPath));
  isSet[0] = 1;
}

void _start() __attribute__ ((section (".text.start")));
void _start()
{
    init();
    currentFile = 0;
    isSet = (u32 *) EBOOT_SET_ADDRESS;
    ebootPath = (void *) EBOOT_PATH_ADDRESS;
    DEBUG_PRINT("START MENU", NULL, 0);
    isSet[0] = 0;

    DEBUG_PRINT("MENU Sets display", NULL, 0);
    sceDisplaySetFrameBuf(frameBuffer, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

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
            DEBUG_PRINT("MENU SET EBOOT", NULL, 0);
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
    sceKernelExitDeleteThread(0);
}



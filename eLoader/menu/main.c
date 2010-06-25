/* Half Byte Loader is GPL
* Crappy Menu(tm) wololo (One day we'll get something better)
* thanks to N00b81 for the graphics library!
* 
*/

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psppower.h>
#include <string.h>
#include <stdio.h>
#include "graphics.h"

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

//initially from eMenu--
typedef struct
{
	unsigned long        APIVersion;
	char       Credits[32];
	char       VersionName[32];
	char       *BackgroundFilename;   // set to NULL to let menu choose.
    char        * filename;   // The menu will write the selected filename there
}	tMenuApi;

PSP_MODULE_INFO("HBL Menu", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(256);

char folders[NB_FOLDERS][FOLDERNAME_SIZE] ;

char * cacheFile = "menu.cache";

int currentFile;
int isSet;
void * ebootPath;
int nbFiles;

void *frameBuffer = (void *)0x44000000;


void readEbootName(const char * filename, const char * backup, char* name) {

	typedef struct {
		char signature[4];
		int version;
		int offset[8];
	}PBPHeader;

	typedef struct {
		char signature[4];
		int version;
		int offset_key_table;
		int offset_values_table;
		int num_entries; 
	}PSFHeader;

	int fid; // file id
	unsigned char index_table[MAX_INDEX_TABLE_SIZE][16];
	PBPHeader pbpHeader;	
	PSFHeader psfHeader;

	fprintf(stderr, "File is [%s]\n",filename);
	if(!(fid = sceIoOpen(filename, PSP_O_RDONLY, 0777)))
	{
		//LOGSTR0("File is NULL");
        return;
	} 

    /* PBP SECTION */
    sceIoRead(fid, &pbpHeader, sizeof(pbpHeader)); // read in pbp header
    if(!(pbpHeader.signature[1] == 'P' && pbpHeader.signature[2] == 'B' && pbpHeader.signature[3] == 'P')) 
    { 
        strcpy(name, backup);
        strcat(name," (Corrupt or invalid PBP)");
        //LOGSTR0("File not a PBP\n");
        sceIoClose(fid);
        return;
    } 

    /* PSF SECTION */
    sceIoLseek(fid, pbpHeader.offset[0] , PSP_SEEK_SET ); // seeks to psf section - first entry in pbp offset table
    sceIoRead(fid,&psfHeader, sizeof(psfHeader));											// reads in psf header

    //LOGSTR1("PSF number of Entries [%d]\n", psfHeader.num_entries);
    if (psfHeader.num_entries > MAX_INDEX_TABLE_SIZE)
    { 
        strcpy(name, backup);
        strcat(name," (Corrupt or invalid PBP)");
        //LOGSTR0("File not a PBP\n");
        sceIoClose(fid);
        return;
    }    
    sceIoRead(fid,&index_table,((sizeof(unsigned char) * 16 ) * psfHeader.num_entries));
    
    // builds offset [offset_key + Offset_data_value + (data_align * 2)]
    int seek_offset = index_table[(psfHeader.num_entries - 1)][OFFSET_KEY_NAME] + index_table[(psfHeader.num_entries - 1)][OFFSET_DATA_VALUE] + 	(index_table[(psfHeader.num_entries - 1)][DATA_ALIGN] * 2);

    // some offset are not even numbers so they read in some off the null padding
    if (seek_offset % 2 > 0)
    {
        seek_offset++;
    }

    //LOGSTR1("PSF using offset [%d]\n", seek_offset);

    sceIoLseek(fid, seek_offset, PSP_SEEK_CUR);
    sceIoRead(fid,name, (sizeof(char) * index_table[(psfHeader.num_entries - 1)][SIZE_OF_VALUE]));
    
    if (!name[0]) {
        strcpy(name, backup);
    }
    
    //LOGSTR1("Name is [%s]\n",(u32)name);


	sceIoClose(fid);
}

/**
* C++ version 0.4 char* style "itoa":
* Written by Lukás Chmela
* Released under GPLv3.
*/
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }
	
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;
	
	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );
	
	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

void saveCache()
{
    SceUID id = sceIoOpen(cacheFile, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
    if (id < 0) 
        return;
    sceIoWrite(id, &folders, FOLDERNAME_SIZE * NB_FOLDERS * sizeof(char));
    sceIoClose(id);
}

/*Loads an alternate cached version of the folders
* if sceIoDopen failed
*/
void loadCache()
{
	int i;

    printTextScreen(0, 216 , "->Using a cached version of the folder's contents", 0x000000FF);
    SceUID id = sceIoOpen(cacheFile, PSP_O_RDONLY, 0777);
	
    if (id < 0) 
		return;
	
    sceIoRead(id, &folders, FOLDERNAME_SIZE * NB_FOLDERS * sizeof(char));
    sceIoClose(id);    
    
    nbFiles = 0;
    for (i = 0; i < NB_FOLDERS; ++i)
	{
        if (folders[i][0] == 0) 
        {
            break;
        }
        nbFiles++;
    }    
}

/*
 * Function to filter out "official" games
 * these games are usually in the form XXXX12345
*/
int not_homebrew(const char * name)
{
    int i;
    
    if (strlen(name) == 0) return 1;
    //official games are in the form: XXXX12345
    if (strlen(name) != 9) return 0;
    for (i = 4; i < 9; ++i)
    {
        if (name[i] < '0' || name[i] > '9')
            return 0;
    }
    
    return 1;
}

int init(char* menuData)
{

 	int i;
	SceUID id;
	SceIoDirent entry;
	nbFiles = 0;
	for (i = 0; i < NB_FOLDERS; ++i)
		folders[i][0] = 0;
  	id = sceIoDopen(ebootPath);

  	if (id < 0) 
	{
    	//LOGSTR1("FATAL: Menu can't open directory %s \n", (u32)ebootPath);
        printTextScreen(0, 205 , "Unable to open GAME folder (syscall issue?)", 0x000000FF);
    	loadCache();
    	return 1;
  	}	
  
 	memset(&entry, 0, sizeof(SceIoDirent));
	
  	while (sceIoDread(id, &entry) > 0 && nbFiles < NB_FOLDERS)
  	{
    	if (strcmp(".", entry.d_name) == 0 || strcmp("..", entry.d_name) == 0 || not_homebrew(entry.d_name)) 
		{
        	memset(&entry, 0, sizeof(SceIoDirent)); 
        	continue;
    	}
         
    	strcpy(folders[nbFiles], entry.d_name);
    	nbFiles++;
    	memset(&entry, 0, sizeof(SceIoDirent)); 
  	}
	
  	sceIoDclose(id);
	
	char fileBuffer[FILE_BUFFER_SIZE];
	char EbootName[MAX_NAME_SIZE];

	// clear the array, seems to be alot of trash left in memory
	for(i = 0; i < NB_FOLDERS; i++ ) 
	{
		memset(&menuData[i * MAX_NAME_SIZE],'\0',MAX_NAME_SIZE);
	}

	for(i = 0; i < nbFiles; i++) 
	{
		memset(fileBuffer,'\0',FILE_BUFFER_SIZE);
		memset(EbootName,'\0',MAX_NAME_SIZE);

		// builds path to eboot	
		strcat(fileBuffer, ebootPath);
		strcat(fileBuffer, folders[i]);
		strcat(fileBuffer, "/EBOOT.PBP");

		readEbootName(fileBuffer, folders[i], EbootName);
		strcpy(&menuData[MAX_NAME_SIZE*i],EbootName);

	}

  	if (!nbFiles) 
	{
        printTextScreen(0, 205 , "No files found in GAME folder (syscall issue?)", 0x000000FF);
    	loadCache();
  	}
	
	saveCache();
	return 0;
}
 
 
void refreshMenu(int offSet, char* menuData, int loadedCache)
{
  	int i;
	u32 color;
	cls();
	for(i = offSet; i < (MAXMENUSIZE + offSet); ++i)
	{
    	color = 0x00FFFFFF;
    	if (i == currentFile)
        	color = 0x0000FFFF;

		if (loadedCache == 1)
		{
    		printTextScreen(0, 15 + (i - offSet) * 12, folders[i], color);
		}
		else
		{
			printTextScreen(0, 15 + (i - offSet) * 12, &menuData[MAX_NAME_SIZE*i], color);
		}
  	}
}

void setEboot() 
{
  	strcat(ebootPath, folders[currentFile]);
  	strcat(ebootPath, "/EBOOT.PBP");
  	//LOGSTR0(ebootPath);
  	isSet = 1;
}
/*
 * hexascii to integer conversion
 */
static int xstrtoi(char *str, int len) {
	int val;
	int c;
	int i;

	val = 0;
    for (i = 0; i < len; i++){
        c = *(str + i);
        fprintf(stderr, "character: %c", c);
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		} else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} else {
			return 0;
		}
		val *= 16;
		val += c;
	}
	return (val);
}

int main(int argc, char *argv[])
{

	SceCtrlData pad; // variable to store the current state of the pad
	
	char menuEntry[NB_FOLDERS][MAX_NAME_SIZE];
	int menuOffSet = 0;
	char buffer[20];
    char dummy_filename[256];
    strcpy(dummy_filename, "ms0:/PSP/GAME");
    currentFile = 0;

    int settingsAddr = 0;

    if (argc > 1) {
        char * hex = argv[1];
        *(hex + 8 ) = 0; //bug in HBL ?
        fprintf(stderr, "Location: 0x %s\n", hex);
        settingsAddr = xstrtoi(hex, 8);
    }
    fprintf(stderr, " settingsAddr : %d\n", settingsAddr);
    if (settingsAddr) {
        tMenuApi * settings = (tMenuApi *) settingsAddr;
        ebootPath = (void *) settings->filename;
    } else {
        ebootPath = dummy_filename;
    }
 
    //LOGSTR0("Start menu\n");
    isSet = 0;

    //LOGSTR0("MENU Sets display\n");
    
    //init screen
    sceDisplaySetFrameBuf(frameBuffer, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
    SetColor(0x00000000);

    //load folder's contents
	// if it loads the cache we want it to display folders[*], menuEntry would be empty
    int loadSucess = init(&menuEntry[0][0]);
    
    refreshMenu(0, &menuEntry[0][0], loadSucess);

    sceKernelDelayThread(100000);
    do
	{
        sceCtrlReadBufferPositive (&pad, 1); // check the input.
		
        if ((pad.Buttons & PSP_CTRL_CROSS) ||  (pad.Buttons & PSP_CTRL_CIRCLE))
		{ // if the cross or circle button is pressed
            //LOGSTR0("Menu sets EBOOT path: ");
            setEboot();			
        }
		else if ((pad.Buttons & PSP_CTRL_DOWN) && (currentFile < nbFiles - 1))
		{
            currentFile++;
			if(currentFile >= (MAXMENUSIZE + menuOffSet) && (nbFiles - 1) >= (MAXMENUSIZE + menuOffSet))
				menuOffSet++;
            refreshMenu(menuOffSet, &menuEntry[0][0], loadSucess);

        }
		else if ((pad.Buttons & PSP_CTRL_UP) && (currentFile > 0) )
		{
            currentFile--;
			if(currentFile ==  menuOffSet && menuOffSet != 0)
				menuOffSet--;
			refreshMenu(menuOffSet, &menuEntry[0][0], loadSucess);
        }
		else if(pad.Buttons & PSP_CTRL_TRIANGLE) 
		{
            strcpy(ebootPath, "quit");
            sceKernelExitGame();
        }

		itoa((currentFile + 1),buffer,10);
		printTextScreen(5, 0 , buffer, 0x00FFFFFF);
		itoa(nbFiles,buffer,10);
		printTextScreen(30, 0 , buffer, 0x00FFFFFF);
		printTextScreen(21, 0 ,"/", 0x00FFFFFF);
		
        printTextScreen(220, 0 , "X or O to select, /\\ to quit", 0x00FFFFFF);
        printTextScreen(0, 227 , "Half Byte Loader BETA by m0skit0, ab5000, wololo, davee", 0x00FFFFFF);
        printTextScreen(0, 238 , "Thanks to n00b81, Tyranid, devs of the PSPSDK, Hitmen,", 0x00FFFFFF);
        printTextScreen(0, 249 , "Fanjita & Noobz, psp-hacks.com", 0x00FFFFFF);
        printTextScreen(0, 260 , "GPL License: give the sources if you distribute binaries!!!", 0x00FFFFFF);
        sceKernelDelayThread(100000);
    } while (!isSet);

    //This point is reached when the value is set
    sceKernelExitGame();
    return 0;
}



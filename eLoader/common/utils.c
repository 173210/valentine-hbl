#include <common/utils.h>
#include <common/debug.h>
#include <exploit_config.h>
#ifndef VITA
#include <common/globals.h>

// "cache" for the firmware version
// 1 means: not set yet
// 0 means unknown

// cache for the psp model
// 0 means unknown
// 1 means not set yet
// 2 is PSPGO
// 3 is other


// New method by neur0n to get the firmware version from the
// module_sdk_version export of sceKernelLibrary
// http://wololo.net/talk/viewtopic.php?f=4&t=128
u32 get_fw_ver()
{
   
   if (globals->fw_ver != 1)
      return globals->fw_ver;

   globals->fw_ver = 0;

   u8 cnt;
   u32 version = 0;
   u8 i;
   u32* addr = (u32 *)memfindsz("sceKernelLibrary", (char*)0x08800300, 0x00001000);

   SceLibraryEntryTable *Entry = (SceLibraryEntryTable *) addr[8];

   cnt = Entry->vstubcount + Entry->stubcount;
   u32** pointer =(u32**) Entry->entrytable;

//   LOGSTR1("Entry is 0x%08lX \n",(u32)Entry);
//   LOGSTR1("cnt is 0x%08lX \n",(u32)cnt);
//   LOGSTR1("pointer is 0x%08lX \n",(u32)pointer);

   for(i=0;i< cnt;i++)
   {
      if( (u32)pointer[i]== 0x11B97506)
      {
         version = *(pointer[i + cnt]);
         break;
      }
   }
   LOGSTR1("Detected firmware version is 0x%08lX\n", (u32)version);

   if(version)
   {
      globals->fw_ver = ((version >> 24) * 100)
         + (((version & 0x00FF0000) >> 16) * 10)
         + ((version & 0x0000FF00) >> 8);
   }
   else
   {
      LOGSTR0("Warning: Cannot find module_sdk_version function \n");
   }

    return globals->fw_ver;
}


u32 getPSPModel()
{
    
    if (globals->psp_model!= 1)
		return globals->psp_model;

    globals->psp_model = 0;

	// This call will always fail, but with a different error code depending on the model
	SceUID result = sceIoOpen("ef0:/", 1, 0777);

	// Check for "No such device" error
	globals->psp_model = (result == (int)0x80020321) ? PSP_OTHER : PSP_GO;

    return globals->psp_model;
}
#endif

int valid_umem_pointer(u32 addr)
{
    return (addr >= 0x08800000) &&
    (addr < 0x0A000000);
}

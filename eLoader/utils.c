#include "utils.h"
#include "debug.h"

// "cache" for the firmware version
// 1 means: not set yet
// 0 means unknown
u32 firmware_version = 1;

/* Attempt to get the firmware version by checking a specific address in Ram
 * The address and values are based on a series of memdump that can mostly be found here:
 * http://advancedpsp.tk/foro_es/viewtopic.php?f=37&t=293
 *
 * It is important to call this function (once) early, before that point in memory gets erased
 * This code is experimental, it hasn't been run on "enough" psps to be sure it works
 */
u32 getFirmwareVersion()
{
    if (firmware_version != 1) return firmware_version;

    firmware_version = 0;
    u32 value = *(u32*)0x09E7b68c;

    switch (value) 
    {
    case 0xB533E9FC:
        firmware_version = 500;
        break;            
    case 0xA67D3F99:
        firmware_version = 550;
        break;
    case 0x67D3F99F:
        firmware_version = 550; //actually 5.51
        break;
    case 0xCFA7F33F:
        firmware_version = 550; //actually 5.55
        break;   
    case 0xC5B13597:
        firmware_version = 570;
        break;   
    case 0x22B5CE0D:
        firmware_version = 600;
        break;
    case 0x32A80E1B  :
        firmware_version = 610;
        break;     
    case 0x73880F1B  :
        firmware_version = 620;
        break;            
    }
    return firmware_version; 
}

// Returns 1 if a given file exists, 0 otherwise
int file_exists(const char * filename)
{
    SceUID id = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    if (id < 0) 
		return 0;
    sceIoClose(id);
    return 1;
}


/*
 * Basic Sprintf functions thanks to Fanjita and Noobz
 */ 

void numtohex8(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=0; i<8; i++)
   {
      dst[i]=hex[(n>>((7-i)*4))&15];
   }
}

void numtohex4(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=4; i<8; i++)
   {
      dst[i-4]=hex[(n>>((7-i)*4))&15];
   }
}

void numtohex2(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=6; i<8; i++)
   {
      dst[i-6]=hex[(n>>((7-i)*4))&15];
   }
}



// limited sprintf function - avoids pulling in large library
int writeFormat(char *xibuff, const char *xifmt, ULONG xidata)
{
  // check for empty format
  if (xifmt[0] == '\0')
  {
    *xibuff = '?';
    return(1);
  }
  else
  {
    if ((xifmt[0] == '0') &&
        (xifmt[1] == '8') &&
        (xifmt[2] == 'l') &&
        (xifmt[3] == 'X'))
    {
      numtohex8(xibuff, xidata);
      return(8);
    }
    else if ((xifmt[0] == '0') &&
             (xifmt[1] == '4') &&
             (xifmt[2] == 'X'))
    {
      numtohex4(xibuff, xidata);
      return(4);
    }
    else if ((xifmt[0] == '0') &&
             (xifmt[1] == '2') &&
             (xifmt[2] == 'X'))
    {
      numtohex2(xibuff, xidata);
      return(2);
    }
    else if (xifmt[0] == 'c')
    {
      *xibuff = (unsigned char)xidata;
      return(1);
    }
    else if (xifmt[0] == 's')
    {
      const char *lptr   = (const char *)xidata;
      int         lcount = 0;

      /***********************************************************************/
      /* Artificially limit %s format to 150 bytes, as a cheap way to        */
      /* avoid log buffer overflow.                                          */
      /***********************************************************************/
      while ((*lptr != 0) && (lcount < 150))
      {
        *xibuff++ = *lptr++;
        lcount++;
      }
      return(lcount);
    }
    else if (xifmt[0] == 'd')
    {
      char lbuff[15];
      int  lnumbuff = 0;
      int  lcount = 0;
      int  lchar;
      int lnum   = (int)xidata;
      if (lnum < 0)
      {
        *xibuff++ = '-';
        lcount++;
        lnum = 0 - lnum;
      }

      lchar = lnum % 10;
      lbuff[0] = lchar + 48;
      lnumbuff++;
      lnum -= lchar;

      while (lnum > 0)
      {
        lnum  = lnum / 10;
        lchar = lnum % 10;
        lbuff[lnumbuff++] = lchar + 48;
        lnum -= lchar;
      }

      while (lnumbuff > 0)
      {
        *xibuff++ = lbuff[--lnumbuff];
        lcount++;
      }

      return(lcount);
    }
    else if ((xifmt[0] == 'p'))
    {
      numtohex8(xibuff, xidata);
      return(8);
    }

    return(0);
  }
}

void mysprintf11(char *xobuff, const char *xifmt,
   ULONG xidata,
   ULONG xidata2,
   ULONG xidata3,
   ULONG xidata4,
   ULONG xidata5,
   ULONG xidata6,
   ULONG xidata7,
   ULONG xidata8,
   ULONG xidata9,
   ULONG xidata10,
   ULONG xidata11)
{
  int  loffs = 0;
  int  lparam = 0;
  char lfmt[10];
  char *lfmtptr;

  while (*xifmt != '\0')
  {
    if (*xifmt != '%')
    {
      *xobuff++ = *xifmt++;
    }
    else
    {
      xifmt++;  // skip the %
      lfmtptr = lfmt;
      while ((*xifmt == '0')
          || (*xifmt == '2')
          || (*xifmt == '4')
          || (*xifmt == '8')
          || (*xifmt == 'l')
          || (*xifmt == 'X')
          || (*xifmt == 'd')
          || (*xifmt == 'p')
          || (*xifmt == 's')
          || (*xifmt == 'c'))
      {
        *lfmtptr ++ = *xifmt++;
      }
      *lfmtptr = '\0';

      switch (lparam)
      {
        case 0:
          xobuff += writeFormat(xobuff, lfmt, xidata);
          break;
        case 1:
          xobuff += writeFormat(xobuff, lfmt, xidata2);
          break;
        case 2:
          xobuff += writeFormat(xobuff, lfmt, xidata3);
          break;
        case 3:
          xobuff += writeFormat(xobuff, lfmt, xidata4);
          break;
        case 4:
          xobuff += writeFormat(xobuff, lfmt, xidata5);
          break;
        case 5:
          xobuff += writeFormat(xobuff, lfmt, xidata6);
          break;
        case 6:
          xobuff += writeFormat(xobuff, lfmt, xidata7);
          break;
        case 7:
          xobuff += writeFormat(xobuff, lfmt, xidata8);
          break;
        case 8:
          xobuff += writeFormat(xobuff, lfmt, xidata9);
          break;
        case 9:
          xobuff += writeFormat(xobuff, lfmt, xidata10);
          break;
        case 10:
          xobuff += writeFormat(xobuff, lfmt, xidata11);
          break;
      }
      lparam++;
    }
  }

  *xobuff = '\0';
}

void mysprintf8(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2, ULONG xidata3, ULONG xidata4, ULONG xidata5, ULONG xidata6, ULONG xidata7, ULONG xidata8)
{
  mysprintf11(xobuff, xifmt, xidata, xidata2, xidata3, xidata4, xidata5, xidata6, xidata7, xidata8, 0, 0, 0);
}

void mysprintf4(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2, ULONG xidata3, ULONG xidata4)
{
  mysprintf8(xobuff, xifmt, xidata, xidata2, xidata3, xidata4, 0, 0, 0, 0);
}

void mysprintf3(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2, ULONG xidata3)
{
  mysprintf11(xobuff, xifmt, xidata, xidata2, xidata3, 0, 0, 0, 0, 0, 0, 0,0);
}

void mysprintf2(char *xobuff, const char *xifmt, ULONG xidata, ULONG xidata2)
{
  mysprintf3(xobuff, xifmt, xidata, xidata2, 0);
}

void mysprintf1(char *xobuff, const char *xifmt, ULONG xidata)
{
  mysprintf3(xobuff, xifmt, xidata, 0, 0);
}

void mysprintf0(char *xobuff, const char *xifmt)
{
  while (*xifmt != '\0')
  {
    *xobuff++ = *xifmt++;
  }
  *xobuff = '\0';
}
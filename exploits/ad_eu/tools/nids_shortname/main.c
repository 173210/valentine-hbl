#include<stdio.h>

unsigned long hash_djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        //We convert to lowercase here because it matches our requirements.
       // This, of course, completely ruins the hash algorithm for case sensitive strings, don't do this at home!!!
        if ( c >= 'A' && c <= 'Z' )
        {
            c+= 'a' - 'A';
        }

        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int toBase36 (char * dest,const unsigned long src)
{
       char base_digits[36] =
	 {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

   int converted_number[64];
   int base, index=0;

   base = 36;
   unsigned long number_to_convert = src;

   /* convert to the indicated base */
   while (number_to_convert != 0)
   {
	 converted_number[index] = number_to_convert % base;
	 number_to_convert = number_to_convert / base;
	 ++index;
   }

   /* now print the result in reverse order */
   --index;  /* back up to last entry in the array */
   for(  ; index>=0; index--) /* go backward through array */
   {
	 *dest = base_digits[converted_number[index]];
     dest++;
   }
   *dest = 0; //terminate string
   return 1;
}

const char * files[] = {"interruptmanager", "iofilemgrforuser", "loadexecforuser", "modulemgrforuser", "sceatrac3plus", "sceaudio", "scectrl", "scedisplay", "scege_user", "scehprm", "sceimpose", "scempeg", "scenet", "scenetadhoc", "scenetadhocctl", "scenetapctl", "scenetinet", "scenetresolver", "scepower", "sceReg", "scertc", "scesascore", "scesuspendforuser", "sceumduser", "sceutility", "scewlandrv", "stdioforuser", "sysmemuserforuser", "threadmanforuser", "utilsforuser"};

int main()
{
    int i;
    for (i = 0; i < 30; ++i) {
        unsigned long hash = hash_djb2(files[i]);
        char lib[9];
        toBase36 (lib,hash);
        printf("%s becomes %s\n", files[i], lib);

        char src[512];
        char dest [512];

        sprintf(src, "nid/%s%s", files[i], ".nids");
        sprintf(dest, "nid/%s%s", lib, ".NID");
        printf("%s becomes %s\n", src, dest);
        rename(src, dest);
    }

    return 1;
}
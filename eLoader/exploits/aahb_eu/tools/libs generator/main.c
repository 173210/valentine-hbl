/* NIDSPRX

Desarrollado por Arisma para advancedpsp.tk Abril 2010
   www.advancedpsp.tk
   www.daxhordes.org
   code.google.com/p/valentine-hbl
   (siempre me acordaré de consoleswap.net)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void recuperaNombre(char buf[], char* pFich, char* pExt)
{
   char *pos;
   char car;

   pos = strstr(buf, "Name ");
   pos+= 5;
   car = *pos;
   memset(pFich, '\0', 20);
   while(car != ',')
   {   
      *pFich = car;
      pFich++;
      pos++;
      car = *pos;
   }
   strcat(pFich, ".nids");
/*   while (*pExt != '\0')
   {
      *pFich = *pExt;
      pFich++;
      pExt++;
   }*/
}

//No me gusta hexAAscci :P
char hexToAscii(char pri, char sec)
{
   char hex[5], *stop;
   hex[0] = '0';
   hex[1] = 'x';
   hex[2] = pri;
   hex[3] = sec;
   hex[4] = 0;
   return strtol(hex, &stop, 16);
}

int main (int argc, char* argv[])
{
   FILE *fp, *fpout;   
   char fichero[30]; //Incrementado el tamaño, ya que con 20 es pequeño en algunos casos
   char extension[] = ".nids";
   unsigned char buffer[200];
   unsigned char dir[8];
   int i,j;
   char* pFichero;
   char* pExtension;

   pFichero = fichero;
   pExtension = extension;

   if (argc <= 1) printf("Debes pasar como parámetro el fichero txt a tratar\n");
   else
   {
      fp = fopen(argv[1], "r");
      if (fp == NULL )
      {
         printf("El fichero no existe o error al abrirlo\n");
         exit(1);
      }
      else{
         fgets(buffer, 200, fp);
         while (!feof(fp))
         {
            if(strncmp(buffer, "Export ", 7) == 0)
            {
               printf("%s\n", buffer);
               recuperaNombre(buffer, pFichero, pExtension);
               printf("Fichero a generar: %s\n", pFichero);
               fpout = fopen(fichero, "wb");
               if (fpout == NULL)
               {
                  printf("Error creando fichero de salida %s", fichero);
                  exit(1);   
               }
               //Las 2 siguientes lineas no contienen información que necesitemos
               fgets(buffer, 200, fp);
               fgets(buffer, 200, fp);
               //Leemos lineas mientras no cambie de sección o \u0144umero de export
               while(buffer[0] == '0')
               {
                  //Comenta la sig linea para que no se visualice las lineas con funciones
                  printf("%s\n", buffer);
                  j = 0;
                  //Almacenamos en dir la dirección de la función exportada
                  for (i = 2; i < 10; i++)
                  {
                     dir[j] = (buffer[i]);
                     j++;
                  }
                  //Escribimos en el archivo la dirección en ascii extendido(little-endian)
                  fputc(hexToAscii(dir[6], dir[7]),fpout);
                  fputc(hexToAscii(dir[4], dir[5]),fpout);
                  fputc(hexToAscii(dir[2], dir[3]),fpout);
                  fputc(hexToAscii(dir[0], dir[1]),fpout);
                  
                  fgets(buffer, 200, fp);
               }
               printf("\n");
            }
            else fgets(buffer, 200, fp);
         }
      }
   }
   return 0;
}

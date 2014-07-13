#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psptypes.h>

#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

typedef struct
{
	int textureWidth;  // the real width of data, 2^n with n>=0
	int textureHeight;  // the real height of data, 2^n with n>=0
	int imageWidth;  // the image width
	int imageHeight;
	Color* data;
} Image;


extern void clearScreen(Color color);

extern void putPixelScreen(Color color, int x, int y);

/**
 * Set a pixel in an image to the specified color.
 *
 * @pre x >= 0 && x < image->imageWidth && y >= 0 && y < image->imageHeight && image != NULL
 * @param color - new color for the pixels
 * @param x - left position of the pixel
 * @param y - top position of the pixel
 */

extern Color getPixelScreen(int x, int y);

/**
 * Get the color of a pixel of an image.
 *
 * @pre x >= 0 && x < image->imageWidth && y >= 0 && y < image->imageHeight && image != NULL
 * @param x - left position of the pixel
 * @param y - top position of the pixel
 * @return the color of the pixel
 */

extern void printTextScreen(int x, int y, const char * text, u32 color);

void puts_scr(const char * text);
extern void puts_scr_color(const char * text, u32 color);
void PRTSTR8(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I);
void PRTSTR4(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E);
void PRTSTR3(const char* A, unsigned long B, unsigned long C, unsigned long D);
void PRTSTR2(const char* A, unsigned long B, unsigned long C);
void PRTSTR1(const char* A, unsigned long B);
void PRTSTR0(const char* A);

/**
 * Print a text (pixels out of the screen or image are clipped).
 *
 * @param x - left position of text
 * @param y - top position of text
 * @param text - the text to print
 * @param color - new color for the pixels
 * @param image - image
 */

extern Color* getVramDrawBuffer();

/**
 * Get the current display buffer for fast unchecked access.
 *
 * @return the start address of the current display buffer
 */
extern Color* getVramDisplayBuffer();

//clears screen with a given color
void SetColor(int col);

//clear screen
void cls();

#endif

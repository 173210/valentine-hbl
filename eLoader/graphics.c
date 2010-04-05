
#include "graphics.h"

#define IS_ALPHA(color) (((color)&0xff000000)==0xff000000?0:1)
#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCREEN_HEIGHT*4)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

Color* g_vram_base = (Color*) (0x40000000 | 0x04000000);

typedef struct
{
	unsigned short u, v;
	short x, y, z;
} Vertex;

extern u8 msx[];

unsigned int __attribute__((aligned(16))) list[262144];
static int dispBufferNumber;
static int initialized = 0;


static int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

Color* getVramDrawBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 0) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}

Color* getVramDisplayBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 1) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}


void putPixelScreen(Color color, int x, int y)
{
	Color* vram = getVramDrawBuffer();
	vram[PSP_LINE_SIZE * y + x] = color;
}

Color getPixelScreen(int x, int y)
{
	Color* vram = getVramDrawBuffer();
	return vram[PSP_LINE_SIZE * y + x];
}

Color getPixelImage(int x, int y, Image* image)
{
	return image->data[x + y * image->textureWidth];
}

void printTextScreen(int x, int y, char text[], u32 color)
{
	int c, i, j, l;
	u8 *font;
	Color *vram_ptr;
	Color *vram;
	
	// if (!initialized) return;

	for (c = 0; c < strlen(text); c++) {
		if (x < 0 || x + 8 > SCREEN_WIDTH || y < 0 || y + 8 > SCREEN_HEIGHT) break;
		char ch = text[c];
		vram = getVramDrawBuffer() + x + y * PSP_LINE_SIZE;
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) {
			vram_ptr  = vram;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *vram_ptr = color;
				vram_ptr++;
			}
			vram += PSP_LINE_SIZE;
		}
		x += 8;
	}
}


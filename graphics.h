#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <ddraw.h>

BOOL InitGraphics();

BOOL CreateSurfaces();
BOOL ClearSurface(LPDIRECTDRAWSURFACE pSurface, BYTE bColor);
BOOL FlipPrimarySurface();
BOOL RestoreSurfaces();
void DeInitGraphics();
BOOL DrawTextOnSurf(LPDIRECTDRAWSURFACE pSurf, const char* text,
              int x, int y, BYTE color, int len = 0);
BOOL DrawInt(LPDIRECTDRAWSURFACE pSurf, int value, int len,
              int x, int y, BYTE color);



// TRUE if DDraw was correctly initialized
extern BOOL isDDrawOK;

// pointer to DirectDraw object
extern LPDIRECTDRAW pDDrawObj;

// surfaces
extern LPDIRECTDRAWSURFACE pPrimarySurface;
extern LPDIRECTDRAWSURFACE pBackBuffer;
extern LPDIRECTDRAWSURFACE pFireSurface;
extern LPDIRECTDRAWPALETTE pSpritesPal;
extern LPDIRECTDRAWSURFACE pSpritesSurf;



// color table
const int BLACK = 0;
const int BLUE = 1;
const int RED = 2;
const int MAGENTA = 3;
const int GREEN = 4;
const int CYAN = 5;
const int YELLOW = 6;
const int WHITE = 7;

const int LIGHTBLUE = 1 + 8;
const int LIGHTRED = 2 + 8;
const int LIGHTMAGENTA = 3 + 8;
const int LIGHTGREEN = 4 + 8;
const int LIGHTCYAN = 5 + 8;
const int LIGHTYELLOW = 6 + 8;
const int LIGHTWHITE = 7 + 8;



#endif
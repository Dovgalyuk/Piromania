#include <stdio.h>
#include "graphics.h"
#include "Piromania.h"
#include "cdib.h"


#pragma comment(lib, "ddraw.lib")



// local functions prototypes
BOOL InitSprites();
//BOOL InitMenu();
BOOL CreateOffScreenSurface(LPDIRECTDRAWSURFACE &pSurf, 
                            UINT width, UINT height);

// TRUE if DDraw was correctly initialized
BOOL isDDrawOK = FALSE;

// pointer to DirectDraw object
LPDIRECTDRAW pDDrawObj = NULL;

// surfaces
LPDIRECTDRAWSURFACE pPrimarySurface = NULL;
LPDIRECTDRAWSURFACE pBackBuffer = NULL;
LPDIRECTDRAWSURFACE pFireSurface = NULL;

// sprites variables
CDib dibSprites;
LPDIRECTDRAWPALETTE pSpritesPal = NULL;
LPDIRECTDRAWSURFACE pSpritesSurf = NULL;
//LPDIRECTDRAWSURFACE pBackgroundSurf = NULL;





// инициализация DirectDraw
BOOL InitGraphics()
{
    HANDLE hFile = CreateFile("sprites.bmp", GENERIC_READ, 
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dibSprites.LoadBitmapFile(hFile);
        CloseHandle(hFile);
    }
    
    // инициализация DirectDraw  
    isDDrawOK = FALSE;
    pPrimarySurface = NULL;
    pBackBuffer = NULL;
    
    // создать объект
    if (DirectDrawCreate(NULL, (LPDIRECTDRAW*)&pDDrawObj, NULL) != DD_OK)
        return FALSE;
    
    // установить для приложения исключительный режим
    if (pDDrawObj->SetCooperativeLevel(hWnd, 
        DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
        return FALSE;
    
    // установить разрешение экрана
    if (pDDrawObj->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8) != DD_OK)
        return FALSE;
    
    // создать первичную и вторичную поверхности
    if (!CreateSurfaces())
        return FALSE;
    
    // очистить вторичную поверхность
    isDDrawOK = ClearSurface(pBackBuffer, 0);
    
    return isDDrawOK
        && InitSprites();
//        && InitMenu();
}





// создает поверхности, необходимые для отображения
BOOL CreateSurfaces()
{
  DDSURFACEDESC ddSurfaceDesc;
  DDSCAPS ddsCaps;
  BOOL success = FALSE;
  
  // Initialize the surface description structure
  // for a primary surface with one back buffer.
  memset(&ddSurfaceDesc, 0, sizeof(DDSURFACEDESC));
  ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
  ddSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
  ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
    DDSCAPS_FLIP | DDSCAPS_COMPLEX;
  ddSurfaceDesc.dwBackBufferCount = 1;
  
  // Create the primary surface.
  // If the primary surface was created okay...
  if (pDDrawObj->CreateSurface(&ddSurfaceDesc, 
    &pPrimarySurface, NULL) == DD_OK)
  {
    // Retrieve a pointer to the back buffer.
    ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    // If we get the back buffer pointer okay,
    // the DirectDraw surfaces were created okay.
    if (pPrimarySurface->GetAttachedSurface(&ddsCaps, 
      &pBackBuffer) == DD_OK)
      success = TRUE;
  }

  success = success && CreateOffScreenSurface(pFireSurface, SPECCY_WIDTH, SPECCY_HEIGHT);
  
  return success;
}




// очищает поверхность pSurface
BOOL ClearSurface(LPDIRECTDRAWSURFACE pSurface, BYTE bColor)
{
    DDSURFACEDESC ddSurfaceDesc;
    BOOL success = FALSE;
    HRESULT result;
    
    // Initialize the surface description structure.
    memset(&ddSurfaceDesc, 0, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
    
    BOOL exitLoop = FALSE;
    do
    {
        // Attempt to lock the surface.
        result = pSurface->Lock(NULL, &ddSurfaceDesc,
            DDLOCK_SURFACEMEMORYPTR, NULL);
        
        // If the lock failed due to the loss
        // of the surface, restore the surface
        // and try again.
        if (result == DDERR_SURFACELOST)
            RestoreSurfaces();
        else if (result != DDERR_WASSTILLDRAWING)
            exitLoop = TRUE;
    }
    while (!exitLoop);
    
    // If the surface locked okay...
    if (result == DD_OK)
    {
        UINT surfaceWidth = ddSurfaceDesc.lPitch;
        UINT surfaceHeight = ddSurfaceDesc.dwHeight;
        
        // Clear the buffer's area in memory.
        memset(ddSurfaceDesc.lpSurface, bColor, surfaceWidth * surfaceHeight);
        
        // Unlock the back buffer.
        pSurface->Unlock(ddSurfaceDesc.lpSurface);
        success = TRUE;
    }
    
    return success;
}





// рисует текст на поверхности
//   pSurf - поверхность
//   text - текст
//   x, y - координаты на поверхности
BOOL DrawTextOnSurf(LPDIRECTDRAWSURFACE pSurf, const char* text,
              int x, int y, BYTE color, int len)
{
  if (len == 0) len = strlen(text);
  int xx;
  RECT r;
  xx = x;
  for (const char* c = text ; *c != '\0' && c - text < len ; c++)
  {
    bool draw = false;
    r.top = 400 + color * 2 * CHR;
    r.left = 0;
    if (*c >= 'A' && *c <= 'Z')
    {
        r.top += 0;
        r.left = (*c - 'A') * CHR;
        draw = true;
    }
    else if (*c >= 'a' && *c <= 'z')
    {
        r.top += 0;
        r.left = (*c - 'a') * CHR;
        draw = true;
    }
    else if (*c >= '0' && *c <= '9')
    {
        r.top += CHR;
        r.left = (*c - '0') * CHR;
        draw = true;
    }
    else switch (*c)
    {
        case '*':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + CHR;
            draw = true;
            break;
        case '.':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + 2 * CHR;
            draw = true;
            break;
        case ',':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + 3 * CHR;
            draw = true;
            break;
        case '(':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + 4 * CHR;
            draw = true;
            break;
        case ')':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + 5 * CHR;
            draw = true;
            break;
        case '!':
            r.top += 0;
            r.left = ('Z' - 'A') * CHR + 6 * CHR;
            draw = true;
            break;
    }
    if (draw)
    {
        r.bottom = r.top + CHR;
        r.right = r.left + CHR;
        pSurf->BltFast(xx, y, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    xx += CHR;
  }

  return TRUE;
}


// печатает целое число
//   len - длина числа (для добавления ведущих нулей)
BOOL DrawInt(LPDIRECTDRAWSURFACE pSurf, int value, int len,
              int x, int y, BYTE color)
{
  char s[100], fmt[10];
  sprintf(fmt, "%%0%dd", len);
  sprintf(s, fmt, value);
  
  return DrawTextOnSurf(pSurf, s, x, y, color);
}




// инициализация спрайтов
BOOL InitSprites()
{
    // создать палитру
    dibSprites.CreatePalette(pSpritesPal);
    
    // создать поверхность
    if (!CreateOffScreenSurface(pSpritesSurf, 
        dibSprites.GetDibWidth(),
        dibSprites.GetDibHeight()))
        pSpritesSurf = NULL;
    
    RECT r;
    r.right = dibSprites.GetDibWidth();
    r.bottom = dibSprites.GetDibHeight();
    
    DDCOLORKEY ddColorKey;
    ddColorKey.dwColorSpaceLowValue = 255;
    ddColorKey.dwColorSpaceHighValue = 255;
    
    return pSpritesPal != NULL
        && pSpritesSurf != NULL
        && dibSprites.CopyDibToSurface(pSpritesSurf, &r)
        && pSpritesSurf->SetColorKey(DDCKEY_SRCBLT, &ddColorKey) == DD_OK
        && pFireSurface->SetColorKey(DDCKEY_SRCBLT, &ddColorKey) == DD_OK
        && pPrimarySurface->SetPalette(pSpritesPal) == DD_OK;
}




// переключает первичную и вторичную поверхности
BOOL FlipPrimarySurface()
{
  HRESULT result;
  BOOL exitLoop = FALSE;
  do
  {
    result = pPrimarySurface->Flip(NULL, DDFLIP_WAIT);
    if (result == DDERR_SURFACELOST)
      RestoreSurfaces();
    else
      exitLoop = TRUE;
  }
  while (!exitLoop);

  return result == DD_OK;
}





// восстанавливает поверхности
BOOL RestoreSurfaces()
{
  BOOL r = pPrimarySurface->Restore() == DD_OK;
  if (pSpritesSurf)
  {
    RECT rect;
    rect.right = dibSprites.GetDibWidth();
    rect.bottom = dibSprites.GetDibHeight();
    r = r && pSpritesSurf->Restore() == DD_OK
        && pFireSurface->Restore() == DD_OK
        && dibSprites.CopyDibToSurface(pSpritesSurf, &rect);
  }
  
  return r;
}







// освобождение памяти
void DeInitGraphics()
{
  // release memory

  if (pSpritesPal)
    pSpritesPal->Release();

  if (pPrimarySurface)
    pPrimarySurface->Release();

  if (pSpritesSurf)
    pSpritesSurf->Release();

  if (pFireSurface)
    pFireSurface->Release();

  if (pDDrawObj)
    pDDrawObj->Release();
}






// создать внеэкранную поверхность
BOOL CreateOffScreenSurface(LPDIRECTDRAWSURFACE &pSurf, 
                            UINT width, UINT height)
{
  // Initialize the DDSURFACEDESC structure.
  DDSURFACEDESC ddSurfaceDesc;
  memset(&ddSurfaceDesc, 0, sizeof(DDSURFACEDESC));
  ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
  ddSurfaceDesc.dwFlags = DDSD_CAPS |
    DDSD_HEIGHT | DDSD_WIDTH;
  ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddSurfaceDesc.dwWidth = width;
  ddSurfaceDesc.dwHeight = height;
  
  // Create the new surface.
  return pDDrawObj->CreateSurface(&ddSurfaceDesc,&pSurf, NULL) == DD_OK;
}


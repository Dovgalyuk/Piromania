#include "Menu.h"
#include "States.h"
#include "graphics.h"
#include "Cdib.h"
#include "Piromania.h"

// main menu variables
//CDib dibMenu;
//LPDIRECTDRAWPALETTE pMenuPal = NULL;


// инициализировать переменные
void CreateMainMenu()
{
    /*
    // загрузить спрайты
    HANDLE hFile = CreateFile("menu.bmp", GENERIC_READ, 
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dibMenu.LoadBitmapFile(hFile);
        CloseHandle(hFile);
    }

    // создать палитру
    dibMenu.CreatePalette(pMenuPal);
    */
}

// освободить память
void ReleaseMainMenu()
{
    /*
    if (pMenuPal)
        pMenuPal->Release();
        */
}


// нарисовать главное меню
void DrawMainMenu()
{
    RECT r;
    // очистить экран
    ClearSurface(pBackBuffer, BLACK);

    // (C)
    r.left = 0;
    r.top = 496;
    r.right = 192;
    r.bottom = 592;
    pBackBuffer->BltFast(X0, Y0 + 272,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    r.left = 196;
    r.top = 580;
    r.right = 370;
    r.bottom = 591;
    pBackBuffer->BltFast(464, 469,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // нарисовать главное меню
    r.left = 0;
    r.top = 592;
    r.right = 376;
    r.bottom = 874;
    pBackBuffer->BltFast(X0 + 68, Y0 - 20,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    r.left = 378;
    r.top = 482;
    r.right = 512;
    r.bottom = 626;
    pBackBuffer->BltFast(X0 + 378, Y0 + 200,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // выбрать палитру
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // переключиться между поверхностями
    FlipPrimarySurface();
}

// обработка сообщений
bool MainMenuMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case '0':
            EnterState(ST_GAME);
            break;
        case '1':
            EnterState(ST_SCORES);
            break;
        case '2':
            EnterState(ST_KEYS);
            break;
        case '3':
            OnClose();
            break;
        }
        return true;
    }
    return false;
}

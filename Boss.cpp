#include "Boss.h"
#include "Graphics.h"
#include "States.h"
#include "Game.h"

namespace Boss
{
int percent = 0;
int timeout = 1000 / DELAY;
};

// нарисовать
void DrawBoss()
{
    RECT r;
    char s[10], *ss;
    // очистить экран
    ClearSurface(pBackBuffer, 0);

    r.top = 1154;
    r.left = 256;
    r.bottom = r.top + 18;
    r.right = r.left + 100;
    pBackBuffer->BltFast(X0, Y0 + SPECCY_HEIGHT - 18 * 3,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    r.top = 1154;
    r.left = 256;
    r.bottom = r.top + 18;
    r.right = r.left + 20;
    pBackBuffer->BltFast(X0 + 110, Y0 + SPECCY_HEIGHT - 18 * 3,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    r.top = 1136;
    r.left = 256;
    r.bottom = r.top + 18;
    r.right = r.left + 140;
    pBackBuffer->BltFast(X0, Y0 + SPECCY_HEIGHT - 18,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    sprintf(s, "%d", Boss::percent);
    ss = s;
    int x = 0;
    while (*ss)
    {
        r.left = 396 + ((*ss) - '0') * 10;
        r.right = r.left + 10;
        pBackBuffer->BltFast(X0 + 140 + x, Y0 + SPECCY_HEIGHT - 18,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        ss++;
        x += 10;
    }
    r.left = 396 + 10 * 10;
    r.right = r.left + 10;
    pBackBuffer->BltFast(X0 + 140 + x, Y0 + SPECCY_HEIGHT - 18,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // выбрать палитру
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // переключиться между поверхностями
    FlipPrimarySurface();
}



// обработка сообщений
bool BossMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        SafeEnterState(ST_GAME);
        ResumeGameSounds();

        return true;
    }
    return false;
}


// такт
void BossTick()
{
    if (Boss::timeout)
        Boss::timeout--;
    else
    {
        Boss::timeout = 1000 / DELAY;
        Boss::percent = (Boss::percent + 1) % 101;
    }
}

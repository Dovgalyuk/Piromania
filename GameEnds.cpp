#include "GameEnds.h"
#include "Graphics.h"
#include "Piromania.h"
#include "States.h"


const char gameOver[] = "GAME OVER!";
const char youWon[] = "YOU WIN!";

const char* outputStr;
int timeout;

// конец игры
void InitGameOver()
{
    outputStr = gameOver;
    timeout = 2000 / DELAY;
}

// пройдены все уровни
void InitGameWon()
{
    outputStr = youWon;
    timeout = 2000 / DELAY;
}

// нарисовать
void DrawGameEnd()
{
    RECT r;
    // очистить экран
    ClearSurface(pBackBuffer, 0);

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

    // нарисовать заголовок
    r.left = 0;
    r.top = 592;
    r.right = 376;
    r.bottom = 673;
    pBackBuffer->BltFast(X0 + 68, Y0 - 20,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    DrawTextOnSurf(pBackBuffer, outputStr, 
        X0 + (32 - strlen(outputStr)) * CHR / 2, Y0 + 8 * CHR, 0);

    // выбрать палитру
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // переключиться между поверхностями
    FlipPrimarySurface();
}




// tick
void GameEndTick()
{
    if (timeout)
        timeout--;
    else
        EnterState(ST_ENTERNAME);
}

#include <time.h>
#include <windows.h>
#include "Piromania.h"
#include "graphics.h"
#include "States.h"
#include "Sounds.h"

// глобальные переменные
HWND hWnd;
HINSTANCE hInst;

// имя класса
const char szClass[] = "PiromaniaClass";

// состояние программы
BOOL bPaused = FALSE;
BOOL bActive = FALSE;

void OnInit()
{
    srand( (unsigned)time( NULL ) );
    
    InitGraphics();
    InitSounds();
    InitStates();

    EnterState(ST_MENU);
}



// активизация или деактивизация приложения
void OnActivateApp(BOOL fActive)
{
    if (fActive)
    {
        SetCursor(NULL);
        if (pPrimarySurface)
            ClearSurface(pPrimarySurface, 0);
        if (pBackBuffer)
            ClearSurface(pBackBuffer, 0);
    }
    bActive = fActive;
}




// выйти из программы
void OnClose()
{
    DeInitStates();
    DeInitGraphics();
    DeInitSounds();
    
    PostQuitMessage(0);
}




void MainCycle()
{
    static DWORD lastTickCount = 0;
    DWORD thisTickCount;
    
    thisTickCount = GetTickCount();
    if ((thisTickCount - lastTickCount) < DELAY)
        return;
    
    lastTickCount = thisTickCount;

    TickSound();
    TickState();
    DrawState();
}




BOOL CALLBACK WndProc(HWND hWnd, UINT uMsg,
                      WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_ACTIVATEAPP:
        OnActivateApp(wParam);
        return FALSE;
    case WM_DESTROY:
    case WM_CLOSE:
        OnClose();
        return 0;
    default:
        if (MessageState(uMsg, wParam, lParam))
            return 0;
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    hInst = hInstance;
    
    WNDCLASS wndClass;
    MSG msg;
    
    // зарегистрировать класс окна
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = (WNDPROC)WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInst;
    wndClass.hIcon = NULL;
    wndClass.hCursor = NULL;
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = szClass;
    RegisterClass(&wndClass);
    
    // создать окно
    hWnd = CreateWindow(szClass, "Piromania", 
        WS_POPUP,
        0, 0, 40, 80, NULL, NULL, hInst, NULL);
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    OnInit();
    
    // цикл сообщений
    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                return msg.wParam;
            }
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
        else if (!bPaused && bActive)
        {
            MainCycle();
        }
        else
        {
            WaitMessage();
        }
    }
    
    return msg.wParam;
}




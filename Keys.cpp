#include "Keys.h"
#include "Piromania.h"
#include "Graphics.h"
#include "States.h"


int keyLeft = VK_LEFT;
int keyRight = VK_RIGHT;
int keyUp = VK_UP;
int keyDown = VK_DOWN;
int keyFire = VK_CONTROL;
int keyDrop = ' ';
int keyDoor = VK_SHIFT;
int keyPause = 'P';
int keyBoss = 'B';


const int NUMOFKEYS = 9;
int* keys[NUMOFKEYS] = {&keyLeft, &keyRight, &keyUp, &keyDown, &keyFire, &keyDrop, &keyDoor, &keyPause, &keyBoss};
const char* names[NUMOFKEYS] = {"LEFT", "RIGHT", "UP", "DOWN", "USE", "DROP", "DOOR", "PAUSE", "BOSS MODE"};
// следующая клавиша
int nextKey;


// пауза перед выходом из данного режима
int pause;



void myGetKeyNameText(int nVirtKey, char* buffer)
{
    switch (nVirtKey)
    {
    case VK_BACK:
        strcpy(buffer, "BACKSPACE");
        break;
    case VK_TAB:
        strcpy(buffer, "TAB");
        break;
    case VK_RETURN:
        strcpy(buffer, "ENTER");
        break;
    case VK_SHIFT:
        strcpy(buffer, "SHIFT");
        break;
    case VK_CONTROL:
        strcpy(buffer, "CONTROL");
        break;
    case VK_MENU:
        strcpy(buffer, "ALT");
        break;
    case VK_SPACE:
        strcpy(buffer, "SPACE");
        break;
    case VK_PRIOR:
        strcpy(buffer, "PAGE UP");
        break;
    case VK_NEXT:
        strcpy(buffer, "PAGE DOWN");
        break;
    case VK_END:
        strcpy(buffer, "END");
        break;
    case VK_HOME:
        strcpy(buffer, "HOME");
        break;
    case VK_LEFT:
        strcpy(buffer, "LEFT ARROW");
        break;
    case VK_RIGHT:
        strcpy(buffer, "RIGHT ARROW");
        break;
    case VK_UP:
        strcpy(buffer, "UP ARROW");
        break;
    case VK_DOWN:
        strcpy(buffer, "DOWN ARROW");
        break;
    case VK_INSERT:
        strcpy(buffer, "INSERT");
        break;
    case VK_DELETE:
        strcpy(buffer, "DELETE");
        break;
    case VK_LWIN:
        strcpy(buffer, "LEFT WINDOWS");
        break;
    case VK_RWIN:
        strcpy(buffer, "RIGHT WINDOWS");
        break;
    case VK_APPS:
        strcpy(buffer, "APPLICATIONS KEY");
        break;
    case VK_NUMPAD0:
        strcpy(buffer, "NUMPAD 0");
        break;
    case VK_NUMPAD1:
        strcpy(buffer, "NUMPAD 1");
        break;
    case VK_NUMPAD2:
        strcpy(buffer, "NUMPAD 2");
        break;
    case VK_NUMPAD3:
        strcpy(buffer, "NUMPAD 3");
        break;
    case VK_NUMPAD4:
        strcpy(buffer, "NUMPAD 4");
        break;
    case VK_NUMPAD5:
        strcpy(buffer, "NUMPAD 5");
        break;
    case VK_NUMPAD6:
        strcpy(buffer, "NUMPAD 6");
        break;
    case VK_NUMPAD7:
        strcpy(buffer, "NUMPAD 7");
        break;
    case VK_NUMPAD8:
        strcpy(buffer, "NUMPAD 8");
        break;
    case VK_NUMPAD9:
        strcpy(buffer, "NUMPAD 9");
        break;
    case VK_MULTIPLY:
        strcpy(buffer, "MULTIPLY");
        break;
    case VK_ADD:
        strcpy(buffer, "ADD");
        break;
    case VK_SEPARATOR:
        strcpy(buffer, "SEPARATOR");
        break;
    case VK_SUBTRACT:
        strcpy(buffer, "SUBTRACT");
        break;
    case VK_DECIMAL:
        strcpy(buffer, "DECIMAL");
        break;
    case VK_DIVIDE:
        strcpy(buffer, "DIVIDE");
        break;
    case VK_F1:
        strcpy(buffer, "F1");
        break;
    case VK_F2:
        strcpy(buffer, "F2");
        break;
    case VK_F3:
        strcpy(buffer, "F3");
        break;
    case VK_F4:
        strcpy(buffer, "F4");
        break;
    case VK_F5:
        strcpy(buffer, "F5");
        break;
    case VK_F6:
        strcpy(buffer, "F6");
        break;
    case VK_F7:
        strcpy(buffer, "F7");
        break;
    case VK_F8:
        strcpy(buffer, "F8");
        break;
    case VK_F9:
        strcpy(buffer, "F9");
        break;
    case VK_F10:
        strcpy(buffer, "F10");
        break;
    case VK_F11:
        strcpy(buffer, "F11");
        break;
    case VK_F12:
        strcpy(buffer, "F12");
        break;
    case VK_NUMLOCK:
        strcpy(buffer, "NUM LOCK");
        break;
    case VK_SCROLL:
        strcpy(buffer, "SCROLL LOCK");
        break;
    default:
        if ((nVirtKey >= '0' && nVirtKey <= '9')
            || (nVirtKey >= 'A' && nVirtKey <= 'Z'))
        {
            buffer[0] = nVirtKey;
            buffer[1] = 0;
        }
        else
            strcpy(buffer, "UNKNOWN KEY");
        break;
  }
}






// инициализирует процесс выбора управления
void EnterKeys()
{
    nextKey = 0;
    pause = 1000 / DELAY;
}




// рисует экран выбора управления
void DrawKeys()
{
    ClearSurface(pBackBuffer, 0);
    
    DrawTextOnSurf(pBackBuffer, "REDEFINE KEYS", 
        X0 + 8 * CHR, Y0 + CHR, 0);
    
    for (int i = 0 ; i < NUMOFKEYS ; i++)
    {
        DrawTextOnSurf(pBackBuffer, names[i], 
            X0 + 5 * CHR, Y0 + (i + 1) * 2 * CHR + CHR, 2);
    }
    
    for (i = 0 ; i < NUMOFKEYS/* nextKey */; i++)
    {
        char buffer[20];
        myGetKeyNameText(*keys[i], buffer);
        DrawTextOnSurf(pBackBuffer, buffer, X0 + 15 * CHR, 
            Y0 + (i + 1) * 2 * CHR + CHR, (i < nextKey ? 1 : 0));
    }
    
    // выбрать палитру
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // переключиться между поверхностями
    FlipPrimarySurface();
}



// такт работы
void TickKeys()
{
    if (nextKey >= NUMOFKEYS)
    {
        if (pause)
            pause--;
        else
            EnterState(ST_MENU);
    }
}



// обработка сообщений
bool KeysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        if (nextKey < NUMOFKEYS)
        {
            if (wParam == VK_ESCAPE)
            {
                EnterState(ST_MENU);
                return true;
            }

            for (int i = 0 ; i < nextKey ; i++)
                if (wParam == *keys[i])
                    return true;

            *keys[nextKey] = wParam;
            
            nextKey++;
        }
        return true;
    }
    return false;
}









// инициализирует переменные
void CreateKeys()
{
    HANDLE hFile = CreateFile("keys.bin", GENERIC_READ, 0, NULL, 
        OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes;
        for (int i = 0 ; i < NUMOFKEYS ; i++)
        {
            ReadFile(hFile, keys[i], sizeof(int), &bytes, NULL);
        }
        
        CloseHandle(hFile);
    }
}




// освободить память
void ReleaseKeys()
{
    HANDLE hFile = CreateFile("keys.bin", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
        FILE_ATTRIBUTE_HIDDEN, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes;
        for (int i = 0 ; i < NUMOFKEYS ; i++)
        {
            WriteFile(hFile, keys[i], sizeof(int), &bytes, NULL);
        }
        
        CloseHandle(hFile);
    }
}



#include "Scores.h"
#include "Graphics.h"
#include "Piromania.h"
#include "Game.h"
#include "States.h"


// ������������ ����� �����
const int MAX_NAME_LEN = 22;
// ����� �����
int nameLen;
// ������ �����
int name;
// ������� �������
int cursorTimeout;

// ������� ��������
const int RECORDS = 10;
char names[RECORDS][MAX_NAME_LEN];
int scores[RECORDS];




// ����� �� �������� ������ � ������� ��������
bool CanPushItem()
{
    return scores[RECORDS - 1] < score;
}





// �������� ������ � �������
int PushItem()
{
    for (int i = 0 ; i < RECORDS ; i++)
        if (scores[i] < score)
            break;
        
    if (i < RECORDS)
    {
        for (int j = RECORDS - 1 ; j > i ; j--)
        {
            scores[j] = scores[j-1];
            memcpy(names[j], names[j-1], MAX_NAME_LEN);
        }
        scores[i] = score;

        return i;
    }

    return -1;
}






// ������������� �������� ����� �����
void EnterName()
{
    if (CanPushItem())
    {
        nameLen = 0;
        name = PushItem();
        memset(names[name], ' ', MAX_NAME_LEN);
        cursorTimeout = 0;
    }
    else
        EnterState(ST_SCORES);
}


// ��������� ������� ��������
void CreateScores()
{
    HANDLE hFile = CreateFile("scores.bin", GENERIC_READ, 0, NULL, 
        OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes;
        ReadFile(hFile, names, RECORDS * MAX_NAME_LEN, &bytes, NULL);
        ReadFile(hFile, scores, RECORDS * sizeof(int), &bytes, NULL);
        
        CloseHandle(hFile);
    }
    else
        // ������ �������
    {
        for (int i = 0 ; i < RECORDS ; i++)
        {
            scores[i] = 0;
            memcpy(names[i], ". . . . . . . . . . . ", MAX_NAME_LEN);
        }
        scores[0] = 10;
        memcpy(names[0], "Pi, there!            ", MAX_NAME_LEN);
    }
}



// ��������� ������� ��������
void ReleaseScores()
{
    HANDLE hFile = CreateFile("scores.bin", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
        FILE_ATTRIBUTE_HIDDEN, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes;
        WriteFile(hFile, names, RECORDS * MAX_NAME_LEN, &bytes, NULL);
        WriteFile(hFile, scores, RECORDS * sizeof(int), &bytes, NULL);
        
        CloseHandle(hFile);
    }
}







// ���������� ������� ��������
void DrawScores()
{
    RECT r;
    // �������� �����
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

    // ���������� ���������
    r.left = 0;
    r.top = 592;
    r.right = 376;
    r.bottom = 673;
    pBackBuffer->BltFast(X0 + 68, Y0 - 20,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    r.left = 0;
    r.top = 874;
    r.right = 512;
    r.bottom = 890;
    pBackBuffer->BltFast(X0, Y0 + 64,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    for (int i = 0 ; i < RECORDS ; i++)
    {
        DrawInt(pBackBuffer, scores[i], 6, X0, Y0 + (6 + i) * CHR, 0);
        DrawTextOnSurf(pBackBuffer, names[i], X0 + 8 * CHR, Y0 + (6 + i) * CHR, 0, MAX_NAME_LEN);
    }

    // ������� �������
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // ������������� ����� �������������
    FlipPrimarySurface();
}




// ���������� ������������
void EnterNameTick()
{
    cursorTimeout--;
    if (cursorTimeout < -250 / DELAY)
        cursorTimeout = 250 / DELAY;
}



// ���������� ������� ����� �����
void DrawEnterName()
{
    RECT r;
    // �������� �����
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

    // ���������� ���������
    r.left = 0;
    r.top = 592;
    r.right = 376;
    r.bottom = 673;
    pBackBuffer->BltFast(X0 + 68, Y0 - 20,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    r.left = 0;
    r.top = 874;
    r.right = 512;
    r.bottom = 890;
    pBackBuffer->BltFast(X0, Y0 + 64,
        pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    for (int i = 0 ; i < RECORDS ; i++)
    {
        DrawInt(pBackBuffer, scores[i], 6, X0, Y0 + (6 + i) * CHR, 0);
        DrawTextOnSurf(pBackBuffer, names[i], X0 + 8 * CHR, Y0 + (6 + i) * CHR, 0, MAX_NAME_LEN);
    }

    // ���������� ������
    if (cursorTimeout > 0)
    {
        r.left = 160;
        r.top = 416;
        r.right = 176;
        r.bottom = 432;
        pBackBuffer->BltFast(X0 + (8 + nameLen) * CHR, Y0 + (6 + name) * CHR,
            pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
    }

    // ������� �������
//    pPrimarySurface->SetPalette(pSpritesPal);
    
    // ������������� ����� �������������
    FlipPrimarySurface();
}


// ��������� ���������
bool EnterNameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
        case VK_ESCAPE:
            EnterState(ST_SCORES);
            break;
        case VK_BACK:
            if (nameLen)
            {
                nameLen--;
                names[name][nameLen] = ' ';
            }
            break;
        default:
            if (wParam >= ' ' && wParam <= 0x7f
                && nameLen < MAX_NAME_LEN)
                names[name][nameLen++] = wParam;
            break;
        }
        return true;
    }
    return false;
}


// ��������� ���������
bool ScoresMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        EnterState(ST_MENU);
        return true;
    }
    return false;
}

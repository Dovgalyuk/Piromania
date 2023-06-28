#include "States.h"
#include "graphics.h"
#include "Cdib.h"
#include "Game.h"
#include "Scores.h"
#include "Keys.h"
#include "Sounds.h"

// ������� �������
int level;
// ������� ����
int score;
// ���������� ������
int lives;
// ���������� �������� �� ������ �����
int deadMen;
// ���������� ��� �� ��������� �����
int menToCreate;
// ����
CFloor floors[FLOORS_NUM - 1][FLOORS_WIDTH];
// ����������� �������
CStaticObject* staticObjects[FLOORS_NUM][CHARS_NUM];
// ������������ �������
typedef list<CSprite*> TObjList;
TObjList dynamicObjects;

// ������� �������
bool won = false;
// ���������� ������������ �������
int arrowX, arrowY;
// �������� �����
bool gamePaused;

// ���� �������� ����
LPDIRECTSOUNDBUFFER pFireSnd = NULL;
LPDIRECTSOUNDBUFFER pWinSnd = NULL;


// ����� �������
BYTE levels[MAPS_NUM][FLOORS_NUM][2][FLOORS_WIDTH];



// ������� ����� �������������� ��������, ���� ��� ��� �� �����
void NewLadder()
{
    bool ok = true;
    TObjList::iterator obj;
    for (obj = dynamicObjects.begin() ; ok && obj != dynamicObjects.end() ; obj++)
    {
        ok = (*obj)->GetType() != CSprite::LADDERKIT;
        if ((*obj)->GetType() == CSprite::PLAYER
            && ((CPlayer*)(*obj))->objInHands
            && ((CPlayer*)(*obj))->objInHands->GetType() == CSprite::LADDERKIT)
            ok = false;
    }

    if (ok)
        dynamicObjects.push_back(new CLadderKit(28 * CHR, 21 * CHR));
}




// ������� ����� bonus
void NewBonus()
{
    int cnt = 0;
    TObjList::iterator obj;
    for (obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; obj++)
    {
        if ((*obj)->GetType() == CSprite::BONUS)
            cnt++;
    }

    if (cnt < 8)
        dynamicObjects.push_back(new CBonusObject(
        FLOORS_X * CHR + rand() % ((FLOORS_WIDTH - 2) * CHR), 
        FLOORS_Y[rand() % FLOORS_NUM] - 2 * CHR, rand() & 7));
}


void NewElephant()
{
    if (CElephant::createTimeout == 0)
        dynamicObjects.push_back(new CElephant(SPECCY_WIDTH - 2 * CHR, FLOORS_Y[0] - 2 * CHR));
}



void NewMan()
{
    /*
    TObjList::iterator i;
    int cnt = 0;
    for (i = dynamicObjects.begin() ; i != dynamicObjects.end() ; i++)
        if ((*i)->GetType() == CSprite::MAN)
            cnt++;
            */
    if (menToCreate /*&& cnt < 3*/)
    {
        int x, y;
        int cnt = 10;
        do
        {
            x = FLOORS_X + (rand() % (FLOORS_WIDTH - 1));
            y = rand() % 3;//(FLOORS_NUM - 1);
            cnt--;
        } 
        while (cnt 
            && ((staticObjects[y][x] && staticObjects[y][x]->burning)
                || (staticObjects[y][x + 1] && staticObjects[y][x + 1]->burning)
                || floors[y][x - FLOORS_X].type != CFloor::FLOOR
                || floors[y][x - FLOORS_X + 1].type != CFloor::FLOOR));
        if (cnt)
        {
            dynamicObjects.push_front(new CMan(x * CHR, FLOORS_Y[y] - 2 * CHR, rand() & 1));
            menToCreate--;
        }
    }
}


void NewEnemy()
{
    dynamicObjects.push_front(
        new CEnemy((FLOORS_X + rand() % (FLOORS_WIDTH - 1)) * CHR, 
                   FLOORS_Y[1 + rand() % (FLOORS_NUM - 2)] - 2 * CHR));
}


// ���������������� �������
void InitLevel()
{
    int f, x;

    won = false;

    deadMen = 0;

    ClearSurface(pFireSurface, 0xff);

    for (f = 0 ; f < FLOORS_NUM ; f++)
        for (x = 0 ; x < FLOORS_WIDTH ; x++)
        {
            // ���������������� ����
            if (f < FLOORS_NUM - 1)
            {
                switch (levels[level & (MAPS_NUM - 1)][f][1][x] & 7)
                {
                case 3:
                    floors[f][x].type = CFloor::LADDER;
                    break;
                case 4:
                    floors[f][x].type = CFloor::FLOOR;
                    break;
                default:
                    floors[f][x].type = CFloor::EMPTY;
                }
                floors[f][x].x = (x + FLOORS_X) * CHR;
                floors[f][x].y = FLOORS_Y[f];
                floors[f][x].offset = x;
                floors[f][x].floor = f;
                floors[f][x].hp = 15;
                floors[f][x].burning = false;
            }

            // ���������������� ����������� �������
            if (staticObjects[f][x + FLOORS_X])
                delete staticObjects[f][x + FLOORS_X];
            staticObjects[f][x + FLOORS_X] = NULL;

            switch (levels[level & (MAPS_NUM - 1)][f][0][x] & 7)
            {
            case 1:
                staticObjects[f][x + FLOORS_X] = new CDoor(f, x + FLOORS_X, true);
                break;
            case 2:
                staticObjects[f][x + FLOORS_X] = new CDoor(f, x + FLOORS_X, false);
                break;
            case 3:
                staticObjects[f][x + FLOORS_X] = new CLadder(f, x + FLOORS_X);
                break;
            case 5:
                staticObjects[f][x + FLOORS_X] = new CTap(f, x + FLOORS_X);
                break;
            default:
                staticObjects[f][x + FLOORS_X] = new CNothing(f, x + FLOORS_X);
                break;
            }

            if (staticObjects[f][x + FLOORS_X])
                staticObjects[f][x + FLOORS_X]->UpdateFire();
        }

    // ������� ������������ �������
    for (TObjList::iterator obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; )
    {
        delete *obj;
        obj = dynamicObjects.erase(obj);
    }

    // ������� ���
    int i = ((level + 4) < 0x1f) ? level + 4 : 0x1f;
    for (i = (level > 1) ? i * 2 : i ; i >= 0 ; i--)
    {
        f = rand() % (FLOORS_NUM - 1);
        x = rand() % FLOORS_WIDTH;

        if (staticObjects[f][x + FLOORS_X]
            && staticObjects[f][x + FLOORS_X]->GetType() == CSprite::NOTHING
            && floors[f][x].type == CFloor::FLOOR)
            staticObjects[f][x + FLOORS_X]->burning = true;
    }

    // ������������ �������
    // �����
    dynamicObjects.push_back(new CPlayer(2 * CHR, FLOORS_Y[FLOORS_NUM - 1] - 2 * CHR));
    // �����
    dynamicObjects.push_back(new CBucket(11 * CHR - CHR / 2, 21 * CHR));
    // ������������
    const int extY[3] = {18 * CHR, 6 * CHR, 12 * CHR};
    int min = (7 - level) / 2 - 1;
    for (i = 0 ; i < 3 ; i++)
        if (i <= min || (rand() & 0xff) < 0x10)
            dynamicObjects.push_back(new CExtinguisher(6 * CHR, extY[i]));
    // �������������� ��������
    NewLadder();
    // �������
    if (level >= 5 && (level & 1) == 0)
        dynamicObjects.push_back(new CDynamite(
        FLOORS_X * CHR + rand() % ((FLOORS_WIDTH - 2) * CHR), 
        FLOORS_Y[rand() % (FLOORS_NUM - 1)] - 2 * CHR));

    // ����
    menToCreate = 3 + ((level < 7) ? 7 : ((level > 17) ? 17 : level));
    for (i = 0 ; i < 3 ; i++)
        NewMan();

    // ����
    if (level >= 4)
        NewEnemy();
}


// ���������������� ����������
void CreateGame()
{
    // ��������� ������
    HANDLE hFile = CreateFile("levels.bin", GENERIC_READ, 
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes;
        for (int l = 0 ; l < MAPS_NUM ; l++)
            for (int f = 0 ; f < FLOORS_NUM ; f++)
            {
                ReadFile(hFile, levels[l][f][0], FLOORS_WIDTH, &bytes, NULL);
                if (f < FLOORS_NUM - 1)
                    ReadFile(hFile, levels[l][f][1], FLOORS_WIDTH, &bytes, NULL);
            }
        CloseHandle(hFile);
    }

    // �������� ������ ����������� ��������
    for (int x = 0 ; x < CHARS_NUM ; x++)
        for (int y = 0 ; y < FLOORS_NUM ; y++)
            staticObjects[y][x] = NULL;

    // �������� ���� ��� ����
    staticObjects[FLOORS_NUM - 1][1] = new CTap(FLOORS_NUM - 1, 1);
}

// ���������� ������
void ReleaseGame()
{
    // ������ ����������� ��������
    for (int x = 0 ; x < CHARS_NUM ; x++)
        for (int y = 0 ; y < FLOORS_NUM ; y++)
            if (staticObjects[y][x]) delete staticObjects[y][x];

    // ������� ������������ �������
    for (TObjList::iterator obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; )
    {
        delete *obj;
        obj = dynamicObjects.erase(obj);
    }
}


// ���������� ������� ��������
void DrawGame()
{
    TObjList::iterator obj;
    RECT r;
    // �������� �����
    ClearSurface(pBackBuffer, BLACK);

    // ���������� ���
    r.left = (level & 7) * CHR;
    r.top = 0;
    r.right = (level & 7) * CHR + CHR;
    r.bottom = SPECCY_HEIGHT;
    for (int x = 0 ; x < FLOORS_X ; x++)
    {
        pBackBuffer->BltFast(X0 + x * CHR, Y0,
            pSpritesSurf, &r, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
    }

    // ���������� �����
    r.left = 144 + CHR;
    r.top = 0;
    r.right = 144 + 2 * CHR;
    r.bottom = 2 * CHR;
    for (x = 5 * CHR ; x < SPECCY_WIDTH ; x += CHR)
    {
        pBackBuffer->BltFast(X0 + x, Y0 + CHR,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    r.left = 144;
    r.top = 0;
    r.right = 144 + CHR;
    r.bottom = CHR;
    pBackBuffer->BltFast(X0 + 4 * CHR, Y0 + CHR,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    r.left = 144;
    r.top = CHR;
    r.right = 144 + 2 * CHR;
    r.bottom = 2 * CHR;
    pBackBuffer->BltFast(X0 + 3 * CHR, Y0 + CHR * 2,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // ���������� �����
    r.left = 128;
    r.top = 0;
    r.right = 128 + CHR;
    r.bottom = CHR;
    for (int y = 3 * CHR ; y < SPECCY_HEIGHT - 3 * CHR ; y += CHR)
        pBackBuffer->BltFast(X0 + 4 * CHR, Y0 + y,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // ���������� ���
    r.left = 128;
    r.top = CHR;
    r.right = 128 + CHR;
    r.bottom = 2 * CHR;
    for (x = 0 ; x < SPECCY_WIDTH ; x += CHR)
        pBackBuffer->BltFast(X0 + x, Y0 + SPECCY_HEIGHT - CHR,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // ���������� ����������
    for (int f = 0 ; f < FLOORS_NUM - 1 ; f++)
        for (x = 0 ; x < FLOORS_WIDTH ; x++)
            floors[f][x].CopyToSurface(pBackBuffer);

    // ���������� ����������� �������
    for (f = 0 ; f < FLOORS_NUM ; f++)
        for (x = 0 ; x < CHARS_NUM ; x++)
            if (staticObjects[f][x])
                staticObjects[f][x]->CopyToSurface(pBackBuffer);

    // ���������� �����
    r.left = 0;
    r.top = 0;
    r.right = SPECCY_WIDTH;
    r.bottom = SPECCY_HEIGHT;
    pBackBuffer->BltFast(X0, Y0, pFireSurface, &r,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    // ���������� ������������ �������
    for (obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; obj++)
    {
        (*obj)->CopyToSurface(pBackBuffer);
    }

    // ����������� ������� ������
    r.left = 0;
    r.top = 384;
    r.right = 512;
    r.bottom = 384 + CHR;
    pBackBuffer->BltFast(X0, Y0,
        pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    // ���������� ����� ������
    DrawInt(pBackBuffer, level, 2, 3 * CHR + X0, Y0, 0);
    // ���������� ���������� ������
    DrawInt(pBackBuffer, lives, 2, 9 * CHR + X0, Y0, 0);
    // ���������� ����
    DrawInt(pBackBuffer, score, 6, 15 * CHR + X0, Y0, 0);
    // ���������� ������������ ����
    DrawInt(pBackBuffer, scores[0], 6, 26 * CHR + X0, Y0, 0);

    // ���������� �������
    if (won)
    {
        r.left = 224;
        r.top = 496;
        r.right = 224 + CHR;
        r.bottom = 496 + CHR;
        pBackBuffer->BltFast(X0 + (FLOORS_X + arrowX) * CHR, Y0 + (3 + arrowY) * CHR,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }

    // �����
    if (gamePaused)
    {
        r.left = 256;
        r.top = 960;
        r.right = 512;
        r.bottom = 960 + 176;
        pBackBuffer->BltFast(X0 + 128, Y0 + 104,
            pSpritesSurf, &r, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }

    // ������� �������
//    pPrimarySurface->SetPalette(pSpritesPal);

    // ������������� ����� �������������
    FlipPrimarySurface();
}



void StopGameSounds()
{
    StopSounds();
    if (pFireSnd)
        pFireSnd->SetVolume(DSBVOLUME_MIN);

    if (pWinSnd)
        pWinSnd->Stop();
 
    for (TObjList::iterator obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; obj++)
        (*obj)->StopSound();
}

void ResumeGameSounds()
{
    if (pWinSnd)
        pWinSnd->Play(0, 0, DSBPLAY_LOOPING);

    for (TObjList::iterator obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; obj++)
        (*obj)->ResumeSound();
}



// ��������� ���������
bool GameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            if (gamePaused)
            {
                ResumeGameSounds();
                gamePaused = false;
            }
            else
            {
                StopGameSounds();
                EnterState(ST_MENU);
            }
            break;
        default:
            if (wParam == keyPause)
            {
                gamePaused = !gamePaused;
                if (gamePaused)
                    StopGameSounds();
                else
                    ResumeGameSounds();
            }
            else if (wParam == keyBoss)
            {
                SafeEnterState(ST_BOSS);
                StopGameSounds();
            }
            break;
        }
        return true;
    }
    return false;
}


// ����� � ���������
void EnterGame()
{
    level = 1;
    score = 0;
    lives = 3;
    gamePaused = false;
    InitLevel();

    pFireSnd = PlayInfiniteSound(SND_FIRE);
    if (pFireSnd)
    {
        pFireSnd->SetVolume(DSBVOLUME_MIN);
        pFireSnd->Play(0, 0, DSBPLAY_LOOPING);
    }
}


// ����� �� ���������
void LeaveGame()
{
    if (pFireSnd)
    {
        pFireSnd->Stop();
        pFireSnd->Release();
        pFireSnd = NULL;
    }
    if (pWinSnd)
    {
        pWinSnd->Stop();
        pWinSnd->Release();
        pWinSnd = NULL;
    }
}


// �������� �����
void UpdateFire()
{
    static int f = 0;
    // �������� �����
    for (int i = 0 ; i < 1 ; i++, f = (f + 1) % FLOORS_NUM)
        for (int x = FLOORS_X ; x < CHARS_NUM ; x++)
            if (staticObjects[f][x])
                staticObjects[f][x]->UpdateFire();
}



// ���� ����
void TickGame()
{
    if (gamePaused)
    {
        if (pFireSnd)
            pFireSnd->SetVolume(DSBVOLUME_MIN);
        return;
    }

    TObjList::iterator obj;
    static int f = 0;
    static bool floor = true;
    static int timeout = 0;
    int x;
    int c;
    int manCnt;
    static int manTimeout = 0;
    bool dead = true;
    bool elephant = false;
    CPlayer* player = NULL;

    // ������� �������
    if (won)
    {
        for (int i = 0 ; i < 3/* 2*/ && arrowY < (FLOORS_NUM - 1) * 3 ; i++)
        {
            // ��������� ����
            if (arrowY % 3 == 2)
            {
                // floor
                if (floors[arrowY / 3][arrowX].hp >= 12)
                    score += 2;
            }
            else
            {
                // static obj
                CStaticObject* st = staticObjects[arrowY / 3][arrowX + FLOORS_X];
                if (st && st->hp >= 12)
                    score += 2;
            }
            
            arrowX++;
            if (arrowX >= FLOORS_WIDTH)
            {
                arrowX = 0;
                arrowY++;
            }
        }
        // ������� �� ��������� �������
        if (arrowY >= (FLOORS_NUM - 1) * 3)
        {
            if (level < 99)
            {
                // �� ���� �� ����� �� �����
                if (deadMen == 0 && lives < 10)
                    lives++;
                // ��������� �������
                level++;
                if (level & 8 == 0)
                    lives++;
                InitLevel();
                if (pWinSnd)
                {
                    pWinSnd->Stop();
                    pWinSnd->Release();
                    pWinSnd = NULL;
                }
            }
            else
                EnterState(ST_WON);
        }

        return;
    }

    manCnt = 0;
    // ���� ������������
    for (obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; )
    {
        if (/*(*obj)->GetType() == CSprite::PLAYER
            || */(*obj)->GetType() == CSprite::ANGEL
            || (*obj)->GetType() == CSprite::FALLINGPLAYER)
            dead = false;

        if ((*obj)->GetType() == CSprite::PLAYER)
        {
            player = (CPlayer*)(*obj);
            dead = false;
        }

        if ((*obj)->GetType() == CSprite::ELEPHANT)
            elephant = true;

        if ((*obj)->GetType() == CSprite::MAN)
            manCnt++;

        if ((*obj)->GetType() != CSprite::MAN
            || (timeout & 1) 
            || ((CMan*)*obj)->dir == UP
            || ((CMan*)*obj)->dir == DOWN)
        {
            if ((*obj)->GetType() != CSprite::ENEMY
                || (timeout & 1) 
                || ((CEnemy*)*obj)->dir == UP
                || ((CEnemy*)*obj)->dir == DOWN)
                (*obj)->Move();
        }

        if ((*obj)->ToBeDestroyed())
        {
            (*obj)->Destroy();
            delete *obj;
            obj = dynamicObjects.erase(obj);
        }
        else obj++;
    }

    // ������� ������
    if (player 
        && !manCnt
        && !menToCreate
        && !player->objInHands
        && player->x < (FLOORS_X - 2) * CHR)
    {
        StopGameSounds();
        InitLevel();
        return;
    }

    // �������� �����
    UpdateFire();

    if (timeout)
    {
        timeout--;
        return;
    }

    if (CElephant::createTimeout)
        CElephant::createTimeout--;

    if (manTimeout)
        manTimeout--;

    // ������� ��������
    if (!manTimeout
        && (!manCnt || (manCnt < 6 && (rand() & 0xff) <= 0 /* 0 */)))
    {
        NewMan();
        manTimeout = 256;
    }

    // ����� �����
    c = 0;
    
    for (x = 0 ; x < FLOORS_WIDTH ; x++)
    {
        if (floor)
            floors[f][x].Move();
        else
            if (staticObjects[f][FLOORS_X + x] != NULL)
            {
                // ��������� �����
                if (staticObjects[f][FLOORS_X + x]->burning)
                    c++;
                // ��������
                staticObjects[f][FLOORS_X + x]->Move();
                if (staticObjects[f][FLOORS_X + x]->ToBeDestroyed())
                {
                    staticObjects[f][FLOORS_X + x]->Destroy();
                    delete staticObjects[f][FLOORS_X + x];
                    staticObjects[f][FLOORS_X + x] = NULL;
                }
            }
    }

    // �������������� �����������
    if (f == 0 && !floor)
    {
        if (FLOORS_WIDTH - c >= 10)
        {
            if ((rand() & 0xff) <= 3 /* 0 */)
            {
                int i = (rand() & 0xff);
                // ���������� ������� �������������� ��������
                if (i < 0x10)
                    NewLadder();
                // ������� bonus
                else if (i < 0xf0)
                    NewBonus();
                // ������� �����
                else if (!elephant)
                    NewElephant();
            }
        }
        else
        // ����
        {
            if (!elephant)
                NewElephant();
        }
    }

    floor = !floor;
    if (floor)
        f = (f + 1) % (FLOORS_NUM - 1);
    timeout = 3; // 2, 3, 4

    // ��������� �����
    c = 0;
    for (int y = 0 ; y < FLOORS_NUM - 1 ; y++)
        for (x = 0 ; x < FLOORS_WIDTH ; x++)
        {
            if (staticObjects[y][FLOORS_X + x] != NULL
                && staticObjects[y][FLOORS_X + x]->burning) 
                c++;
            if (floors[y][x].burning)
                c++;
        }

    // ��������� �����
    if (pFireSnd)
    {
        if (c == 0)
            pFireSnd->SetVolume(DSBVOLUME_MIN);
        else
            pFireSnd->SetVolume((2000 * c) / (27 * 12) - 2000);
    }

    // ���� ����� �������
    if (c == 0)
    {
        // �������� bonus
        const int bonus[8] = {10, 20, 25, 50, 75, 100, 125, 150};
        score += bonus[(level & 0xe) / 2] * menToCreate;

        arrowX = 0;
        arrowY = 0;

        won = true;

        StopGameSounds();

        pWinSnd = PlayInfiniteSound(SND_WIN);
        if (pWinSnd)
        {
            pWinSnd->Play(0, 0, DSBPLAY_LOOPING);
        }
    }

    // ����� ����
    if (dead)
    {
        if (--lives)
        {
            InitLevel();
        }
        else
            EnterState(ST_GAMEOVER);
    }
}

#include "Objects.h"
#include "Game.h"
#include "Keys.h"
//#include "Sounds.h"


int GetCurrentFloor(int y)
{
    for (int i = 0 ; i < FLOORS_NUM && y > FLOORS_Y[i] ; i++);

    return i;
}


CStaticObject* GetStaticObjectAtXY(int x, int y)
{
    int f = GetCurrentFloor(y);

    if (f < FLOORS_NUM && x >= 0 && x < SPECCY_WIDTH)
        return staticObjects[f][x / CHR];
    else
        return NULL;
}

CFloor* GetFloorAtXY(int x, int y)
{
    int f = GetCurrentFloor(y);
    int xx = x / CHR - FLOORS_X;

    if (f < FLOORS_NUM - 1 && xx >= 0)
        return &floors[f][xx];

    return NULL;
}



/////////////////////////////////////////////////////////////////////
// CSprite
/////////////////////////////////////////////////////////////////////

// ����������� �������
void CSprite::CopyToSurface(LPDIRECTDRAWSURFACE pSurf)
{
    RECT r;
    r.left = GetSprX();
    r.top = GetSprY();
    r.right = r.left + GetSprW();
    r.bottom = r.top + GetSprH();
    
    pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}


// ������������ �� ������ � ���������������
bool CSprite::Intersects(LPRECT r, LPRECT dr)
{
    RECT r1, r2;
    r1.left = x;
    r1.top = y;
    r1.right = x + GetSprW();
    r1.bottom = y + GetSprH();
    if (!dr) 
        dr = &r2;
    return IntersectRect(dr, &r1, r) != 0;
}


// �������� �����
void CSprite::Extinguish()
{
    burning = false;
}



/////////////////////////////////////////////////////////////////////
// CFloor
/////////////////////////////////////////////////////////////////////
// ��������� �������
int CFloor::GetSprX()
{
    switch (type)
    {
    case EXPLODED:
        return 192;
    case LADDER:
        return (burning && hp) ? 240 : 192;
    case EMPTY:
        return 208;
    }

    return 176;
}

int CFloor::GetSprY()
{
    switch (type)
    {
    case EXPLODED:
        return (burning && hp) ? 560 : 544;
    case LADDER:
        return 0;
    case EMPTY:
        return 0;
    }

    return (burning && hp) ? CHR : 0;
}


// ��������� ������� � ��������� �����
void CFloor::Move()
{
    if (burning)
    {
        if (!hp) 
        {
            burning = false;
            return;
        }
        CStaticObject* obj;

        if ((rand() & 0xff) < 150 || level + (rand() & 0x3f) < 0x1f)
            return;

        if ((rand() & 0xff) >= 200)
        {
            if (hp) hp--;
            if (!hp) 
            {
                if (type != EXPLODED)
                    type = EMPTY;
                burning = false;
                return;
            }
        }

        // ���������� ������, ������� �� ����
        obj = staticObjects[floor][offset + FLOORS_X];
        if (obj)
            if (obj->GetType() == CLOSEDDOOR)
            {
                if ((rand() & 0x7f) < 0x1e)
                    obj->burning = true;
            }
            else
                obj->burning = true;

        if (hp < 4 || hp >= 12 || (rand() & 0x7f) + level < 0x1e)
            return;

        // ����� ���� ����
        if ((rand() & 0xff) < 0x28)
        {
            if ((rand() & 0xff) >= 0x64 || floor == FLOORS_NUM - 2)
                return;

            obj = staticObjects[floor + 1][offset + FLOORS_X];
            if (obj)
            {
                if (obj->GetType() == NOTHING 
                    || obj->GetType() == OPENDOOR)
                {
                    if ((rand() & 0xff) < 0xa0)
                        obj->burning = true;
                }
                else if (obj->GetType() == CLOSEDDOOR)
                {
                    if ((rand() & 0xff) < 0x4)
                        obj->burning = true;
                }
                else if (obj->GetType() == LADDER)
                {
                    if ((rand() & 0xff) < 0x64)
                        obj->burning = true;
                }
            }
        }
        // ����� ���� ����
        else
        {
            CFloor* fl;
            int dx = (rand() & 2) - 1;
            if (offset + dx >= 0 && offset + dx < FLOORS_WIDTH)
            {
                fl = &floors[floor][offset + dx];
                if (fl->type == LADDER)
                    fl->burning = true;
                else if (fl->type != NOTHING)
                {
                    obj = staticObjects[floor][offset + dx + FLOORS_X];
                    if (obj && obj->GetType() == CLOSEDDOOR)
                    {
                        if ((rand() & 0x7f) < 0x4)
                            fl->burning = true;
                    }
                    else
                    {
                        if ((rand() & 0x7f) < 0x50)
                            fl->burning = true;
                    }
                }
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////
// CPlayer
/////////////////////////////////////////////////////////////////////
// �����������
CPlayer::CPlayer(int xx, int yy)
{
    x = xx;
    y = yy;
    dx = 0;
    dy = 0;
    dir = RIGHT;
    slower = 0;
    faster = 0;
    step = 0;
    objInHands = NULL;
    droppedX = -4 * CHR;
    useTimeout = 0;
    fillTimeout = 0;
    doorTimeout = 0;
    burning = false;
}


// ����������
CPlayer::~CPlayer()
{
    if (objInHands != NULL)
        delete objInHands;
}


// ��������� �������
int CPlayer::GetSprX()
{
    // ����� �� ��������
    if (dy)
        return 448 + (step / 16) * 2 * CHR;

    return (dir == LEFT) ? 128 : 160;
}

int CPlayer::GetSprY()
{
    int s;

    if (dy)
    {
        s = step % 16;
        return 32 + (s >= 8 ? 15 - s : s) * 2 * CHR;
    }

    return 0;
}


// ����������� �������
void CPlayer::CopyToSurface(LPDIRECTDRAWSURFACE pSurf)
{
    // �������, ��������������� �����
    const int spr[17] = {0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 9, 9};
    const int spr4[17] = {0, 1, 2, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
    // �������, ��������������� ����� � �������� ������
    const int sprWithObject[17] = {0, 1, 2, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 8, 8, 8, 8};

    RECT r;
    // ����� �� ��������
    if (dy != 0)
    {
        // ���������� ������
        r.left = GetSprX();
        r.top = 32;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 12;
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // ���������� ��� ���������
        r.left = GetSprX();
        r.top = GetSprY();
        r.right = r.left + GetSprW();
        r.bottom = r.top + GetSprH();
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        // ���������� ������
        r.left = (dir == LEFT) ? 128 : 160;
        r.top = 32;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 13;

        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // ���������� ����
        r.left = (dir == LEFT) ? 128 : 160;
        // �������� ��������
        int m = (abs(dx) * CHR) / 4;
        int s = (step <= m) ? step : (2 * m - step);
        
        if (useTimeout)
            r.top = 175;
        // � ����� ���� �������
        else if (objInHands)
            r.top = 45 + sprWithObject[s] * 13;
        else if (abs(dx) == 4)
            r.top = 45 + spr4[s] * 13;
        else
            r.top = 45 + spr[s] * 13;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 13;
        
        pSurf->BltFast(x + X0, y + Y0 + 19, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // ���������� ��������
//        r.left = (dir == LEFT) ? 192 : 224;
        r.left = (dir == LEFT) ? 128 : 160;
        int n = 0;
        if (objInHands)
        {
            // ������ � ����� �����
            if (objInHands->GetType() == BUCKET)
                n = 1;
            
            // ������ � ����� ������������
            if (objInHands->GetType() == EXTINGUISHER)
                n = 2;
            
            // ������ � ����� ��������
            if (objInHands->GetType() == LADDERKIT)
                n = 3;
            
            // ������ � ����� �������
            if (objInHands->GetType() == DYNAMITE)
                n = 4;
        }
        if (useTimeout)
            r.left += 64;
        r.top = 188 + 14 * n;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 14;
        
        pSurf->BltFast(x + X0, y + Y0 + 11, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}



// ��������� ������� � ��������� �����
void CPlayer::Move()
{
    const int MAX_FASTER = CHR / 2;
    const int MAX_SLOWER = CHR;

    int i;
    // ������ ��� ������������ �������� �� ���������
    bool stopped = false;
    
    TObjList::iterator obj;

    SHORT up = GetAsyncKeyState(keyUp) & 0x8000;
    SHORT down = GetAsyncKeyState(keyDown) & 0x8000;
    SHORT left = GetKeyState(keyLeft) & 0x8000;
    SHORT right = GetKeyState(keyRight) & 0x8000;
    SHORT fire = GetKeyState(keyFire) & 0x8000;
    SHORT drop = GetKeyState(keyDrop) & 0x8000;
    SHORT door = GetKeyState(keyDoor) & 0x8000;

    // ������ ���������� ��������
    if (slower) slower--;
    // ������ ���������
    if (faster) faster--;
    // ������ �������������
    if (useTimeout) useTimeout--;
    // ������ ���������� �����
    if (fillTimeout) fillTimeout--;
    // ������ ��������/�������� �����
    if (doorTimeout) doorTimeout--;

    // �������� ���
    if (dx && !useTimeout)
        step = (step + abs(dx)) % ((abs(dx) * CHR) / 2);
    // ������������, ���� ����� �� ������ ���� � �� ������ �������
    else if (dy)
    {
        step = (step + 1) % (8 * 4);
        for (i = 0 ; i < FLOORS_NUM ; i++)
            if (y + GetSprH() == FLOORS_Y[i])
            {
                dy = 0;
                stopped = true;
            }
    }
    else
        step = 0;

    // �������� ��������
    if (!faster && !step && !dy && !useTimeout /*&& !fire*/)
    {
        if (left)
        {
            dx--;
            if (dx < 0) dir = LEFT;
            if (objInHands == NULL && dx < -CHR / 4) 
                dx = - CHR / 4;
            else if (objInHands != NULL && dx < -CHR / 4 + 1)
                dx = - CHR / 4 + 1;
            slower = MAX_SLOWER;
            faster = MAX_FASTER;
        }
        if (right)
        {
            dx++;
            if (dx > 0) dir = RIGHT;
            if (objInHands == NULL && dx > CHR / 4) 
                dx = CHR / 4;
            else if (objInHands != NULL && dx > CHR / 4 - 1)
                dx = CHR / 4 - 1;
            slower = MAX_SLOWER;
            faster = MAX_FASTER;
        }
    }
    if ((up || down) && !useTimeout)
    {
        // �������� ��� ����������
        if (dy != 0)
        {
            if (up)
                dy = -1;
            else
                dy = 1;
        }
        // ������ ��������
        else
        {
            int dsy, dfy;
            if (up)
            {
                dsy = 0;
                dfy = -2 * CHR;
            }
            else
            {
                dsy = GetSprH() + CHR;
                dfy = 0;
            }
            // ����� ��������
            CStaticObject* obj = GetStaticObjectAtXY(x, y + dsy);
            CFloor* floor = GetFloorAtXY(x, y + dfy);
            if (!(obj != NULL
                && obj->GetType() == LADDER
                && abs((x / CHR) * CHR - x - CHR / 2) <= CHR / 4
                && floor != NULL
                && floor->type == CFloor::LADDER
                ))
                obj = NULL;

            obj = GetStaticObjectAtXY(x + CHR, y + dsy);
            floor = GetFloorAtXY(x + CHR, y + dfy);
            if (!(obj != NULL
                && obj->GetType() == LADDER
                && abs((x / CHR + 1) * CHR - x - CHR / 2) <= CHR / 4
                && floor != NULL
                && floor->type == CFloor::LADDER
                ))
                obj = NULL;

            // ���� �������� �������
            if (obj)
            {
                x = obj->x - CHR / 2;
                faster = 0;
                slower = 0;
                dx = 0;
                if (up)
                {
                    dy = -1;
                    if (!stopped)
                        step = 8;
                }
                else
                {
                    dy = 1;
                    if (!stopped)
                        step = 0;
                }
                stopped = false;
            }
        }
    }

    if (stopped)
        step = 0;

    // ��������� ����������
    if (dx 
        && ((x + dx < 0 && y + GetSprH() == FLOORS_Y[FLOORS_NUM - 1])
           || (x + dx < FLOORS_X * CHR && y + GetSprH() != FLOORS_Y[FLOORS_NUM - 1])))
    {
        dx = CHR / 8;
        step = 0;
        dir = RIGHT;
    }
    if (dx && x + dx + GetSprW() > SPECCY_WIDTH)
    {
        dx = -CHR / 8;
        step = 0;
        dir = LEFT;
    }

    // �����������
    if (((!left && !right) /*|| useTimeout*/) && !slower && !step && dx)
    {
        slower = useTimeout ? MAX_SLOWER / 2 : MAX_SLOWER;
        if (dx > 0) dx--;
        else if (dx < 0) dx++;
    }

    // ��������� �� ����� �� ����� ����� ������
    RECT r;
    r.left = x + CHR / 2;
    r.top = y;
    r.bottom = r.top + GetSprH();
    r.right = r.left + CHR;
    for (obj = dynamicObjects.begin() ; obj != dynamicObjects.end() ; obj++)
    {
        // ������� ������
        if (objInHands == NULL 
            && ((*obj)->GetType() == BUCKET
                || (*obj)->GetType() == EXTINGUISHER
                || (*obj)->GetType() == DYNAMITE
                || (*obj)->GetType() == LADDERKIT)
            && abs(x - droppedX) > 2 * CHR
            && (*obj)->Intersects(&r)
            )
        {
            objInHands = (CUsableObject*)*obj;
            dynamicObjects.erase(obj);

            if ((*obj)->GetType() == LADDERKIT)
                PlayDublicateSound(SND_PICKUPLADDER);
            else
                PlayDublicateSound(SND_PICKUP);
            break;
        }
        // bonus
        if ((*obj)->GetType() == BONUS
            && (*obj)->Intersects(&r)
            )
        {
            ((CBonusObject*)(*obj))->PickUp();

            PlayDublicateSound(SND_PICKUP);
        }
    }

    // ������� �������
    if (drop && objInHands != NULL && !dy && !useTimeout)
    {
        int i, j = (x + objInHands->GetSprW() - 1) / CHR;
        CFloor* fl;
        CSprite* st;
        for (i = x / CHR ; i <= j ; i++)
        {
            fl = GetFloorAtXY(i * CHR, y);
            st = GetStaticObjectAtXY(i * CHR, y);
            if ((fl && fl->type != CFloor::FLOOR)
                 || (st && st->GetType() == LADDER))
            {
                break;
            }
        }
        if (i > j)
        {
            if (objInHands->GetType() == LADDERKIT)
                PlayDublicateSound(SND_DROPLADDER);
            else if (objInHands->GetType() == BUCKET)
                PlayDublicateSound(SND_DROPBUCKET);
            else if (objInHands->GetType() == EXTINGUISHER)
                PlayDublicateSound(SND_DROPEXTINGUISHER);
            else
                PlayDublicateSound(SND_DROP);

            objInHands->x = x;
            objInHands->y = y;
            objInHands->dir = dir;
            dynamicObjects.push_back(objInHands);
            objInHands = NULL;
            droppedX = x;
        }
    }

    // ������������ �������
    if (fire && !dy && objInHands && !useTimeout)
    {
        bool result;
        if (dir == LEFT)
            result = objInHands->Use(x, y, dir);
        else
            result = objInHands->Use(x + GetSprW(), y, dir);

        if (result)
        {
            dx = 0;
            step = 0;
            useTimeout = objInHands->GetUseTimeout();
        }
    }
    // ���������� ������ � �����
    if (!useTimeout && objInHands && objInHands->ToBeDestroyed())
    {
        delete objInHands;
        objInHands = NULL;
    }

    // ����������� ��������� �����
    if (!dy && !fillTimeout 
        && objInHands && objInHands->GetType() == BUCKET
        && ((CBucket*)objInHands)->water < 4)
    {
        CSprite* st1 = GetStaticObjectAtXY(x, y);
        CSprite* st2 = GetStaticObjectAtXY(x + CHR, y);
        if ((st1 && st1->GetType() == TAP)
            || (st2 && st2->GetType() == TAP))
        {
            ((CBucket*)objInHands)->water++;
            fillTimeout = 16;
            PlayDublicateSound(SND_WATER);
        }
    }

    // ����� ��������� ������ ������ - ������� ��� ��������� �����
    {
        CStaticObject* st = GetStaticObjectAtXY(x + CHR / 2, y);
        if (st)
        {
            st->PassThru();
            if (st->burning && st->hp)
                burning = true;
        }
        st = GetStaticObjectAtXY(x + (3 * CHR) / 2 - 1, y);
        if (st)
        {
            st->PassThru();
            if (st->burning && st->hp)
                burning = true;
        }
        if (burning)
            dynamicObjects.push_back(new CAngel(x, y));
    }
    // ������� ����� ������
    if (!burning && GetCurrentFloor(y) < FLOORS_NUM - 1)
    {
        CFloor* fl1 = GetFloorAtXY(x + CHR, y);
        if (fl1->type != CFloor::LADDER
            && fl1->type != CFloor::FLOOR
            )
        {
            burning = true;
            dynamicObjects.push_back(new CFallingPlayer(x, y));
        }
    }

    // ������ ������ "�����"
    if (!dy && door && !doorTimeout)
    {
        CStaticObject* st = GetStaticObjectAtXY(x + (dir == LEFT ? -CHR/2 : 2 * CHR + CHR/2), y);
        if (st && (st->GetType() == OPENDOOR || st->GetType() == CLOSEDDOOR))
        {
            ((CDoor*)st)->Toggle();
            doorTimeout = 8;
        }
        else
        {
            st = GetStaticObjectAtXY(x + (dir == LEFT ? CHR / 2 - 1 : 2 * CHR - CHR / 2 + 1), y);
            if (st && (st->GetType() == OPENDOOR || st->GetType() == CLOSEDDOOR))
            {
                ((CDoor*)st)->Toggle();
                doorTimeout = 8;
            }
        }
    }

    // �������� ����������
    x += dx;
    y += dy;

    if (abs(x - droppedX) > 2 * CHR)
        droppedX = -4 * CHR;
}







/////////////////////////////////////////////////////////////////////
// CNothing
/////////////////////////////////////////////////////////////////////
CNothing::CNothing(int fl, int offs) 
{ 
    floor = fl;
    offset = offs; 
    x = offs * CHR;
    y = FLOORS_Y[fl] - 2 * CHR;
    hp = 15;
    burning = false;

//    CStaticObject::CStaticObject();
}


/////////////////////////////////////////////////////////////////////
// CLadder
/////////////////////////////////////////////////////////////////////
CLadder::CLadder(int fl, int offs)
{ 
    floor = fl;
    offset = offs;
    x = offs * CHR;
    y = FLOORS_Y[fl] - 2 * CHR;
    hp = 15;
    burning = false;

//    CStaticObject::CStaticObject();
}



/////////////////////////////////////////////////////////////////////
// CStaticObject
/////////////////////////////////////////////////////////////////////

CStaticObject::CStaticObject()
{
    for (int i = 0 ; i < 256 ; i++)
    {
        fireRandom[i] = 192 + (rand() & 63);
        switch (rand() & 7)
        {
        case 0:
        case 1:
            fireDx[i] = -1;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            fireDx[i] = 0;
            break;
        case 6:
        case 7:
            fireDx[i] = 1;
            break;
        }
    }
    iRand = 0;
    iDx = 0;

    for (i = 0 ; i < CHR * 2 ; i++)
        for (int j = 0 ; j < CHR ; j++)
            fire[i][j] = 0;
}

    /*
// ����������� �������
void CStaticObject::CopyToSurface(LPDIRECTDRAWSURFACE pSurf)
{
    CSprite::CopyToSurface(pSurf);

    // ���������� �����
    RECT r;
    r.left = x;
    r.top = y;
    r.right = x + CHR;
    r.bottom = y + 2 * CHR;
    
    pSurf->BltFast(x + X0, y + Y0, pFireSurface, &r,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}
        */


// ��������� ������� � ��������� �����
void CStaticObject::Move()
{
    // ����������� ����
    if (burning)
    {
        CFloor* fl;
        CStaticObject* obj;

        if ((rand() & 0xff) < 150 || level + (rand() & 0x3f) < 0x1f)
            return;

        if ((rand() & 0xff) >= 200)
        {
            if (hp) hp--;
            if (!hp) return;
        }

        if (hp < 4 || hp > 12 || (rand() & 0x7f) + level < 0x1e)
            return;

        fl = NULL;
        obj = NULL;
        if ((rand() & 0xff) >= 0xa0)
        {
            // ����� ���� �����
            if (floor > 0)
            {
                obj = staticObjects[floor-1][offset];
                fl = &floors[floor - 1][offset - FLOORS_X];
            }
        }
        else if ((rand() & 0xff) < 0x14)
        {
            // ����� ����� �� �����
            fl = &floors[floor][offset - FLOORS_X];
        }

        // ����� ���� ����� ��� ����� �� �����
        if (fl)
        {
            if (fl->type == CFloor::LADDER)
            {
                fl->burning = true;
            }
            else if (fl->type != CFloor::EMPTY)
            {
                if (!obj || obj->GetType() != CLOSEDDOOR)
                    fl->burning = true;
                else if ((rand() & 0x7f) < 4)
                    fl->burning = true;
            }
        }
        // ����� ���� ����
        else
        {
            int dx = (rand() & 2) - 1;
            if (offset + dx >= FLOORS_X && offset + dx < CHARS_NUM)
            {
                obj = staticObjects[floor][offset + dx];
                if (obj && floors[floor][offset + dx - FLOORS_X].hp > 0)
                {
                    if (obj->GetType() == NOTHING
                        || obj->GetType() == OPENDOOR)
                    {
                        if ((rand() & 0xff) < 0xa0)
                            obj->burning = true;
                    }
                    else if (obj->GetType() == LADDER)
                    {
                        if ((rand() & 0xff) < 0x64)
                            obj->burning = true;
                    }
                    else if (obj->GetType() == CLOSEDDOOR)
                    {
                        if ((rand() & 0xff) < 0x4)
                            obj->burning = true;
                    }
                }
            }
        }
    }
}



// �������� ������� ����
void CStaticObject::UpdateFire()
{
    const int W = CHR;
    const int H = 2 * CHR;

    int xx, yy;
    // �������� �����
    if (burning && hp)
    {
        // �����
        for (xx = W / 8 ; xx < W - W / 8 ; xx++)
        {
            fire[H - 1][xx] = fireRandom[iRand];
            iRand = (iRand + 1) & 0xff;
        }
    }
    else
    {
        for (xx = 0 ; xx < W ; xx++)
            fire[H - 1][xx] = 0;
    }
    for (yy = 0 ; yy < H - 1 ; yy++)
    {
        for (xx = 1 ; xx < W - 1 ; xx++)
        {
            const int df[16] = {7,6,5,4,3,2,2,1,1,2,2,3,4,5,6,7};
            fire[yy][xx + fireDx[iDx]] = fire[yy + 1][xx] - df[xx];
            iDx = (iDx + 1) & 0xff;
        }
    }
    
    DDSURFACEDESC ddSurfaceDesc;
    HRESULT result;

    // Initialize the surface description structure.
    memset(&ddSurfaceDesc, 0, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
    
    BOOL exitLoop = FALSE;
    do
    {
        // Attempt to lock the surface.
        result = pFireSurface->Lock(NULL, &ddSurfaceDesc,
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
        // Clear the buffer's area in memory.
        BYTE* pSurf = (BYTE*)ddSurfaceDesc.lpSurface + y * surfaceWidth + x;
        for (yy = 0 ; yy < H ; yy++, pSurf += (surfaceWidth - W))
            for (xx = 0 ; xx < W ; xx++, pSurf++)
                *pSurf = (fire[yy][xx] > 160) ? 32 + (fire[yy][xx] >> 4) : 255;

        // Unlock the buffer.
        pFireSurface->Unlock(ddSurfaceDesc.lpSurface);
    }
}



// �������� �����
void CStaticObject::Extinguish()
{
    if (burning)
        dynamicObjects.push_back(new CSmoke(x, y));

    for (int i = 0 ; i < CHR * 2 ; i++)
        for (int j = 0 ; j < CHR ; j++)
            fire[i][j] = 0;

    CSprite::Extinguish();

    UpdateFire();
}


// ����������� ����������
CStaticObject::~CStaticObject()
{
    for (int i = 0 ; i < CHR * 2 ; i++)
        for (int j = 0 ; j < CHR ; j++)
            fire[i][j] = 0;

    // ??????????????????????
//    if (y >= 0 && y <= SPECCY_HEIGHT - CHR && x >= 0 && x <= SPECCY_WIDTH - CHR)
        UpdateFire();
}


/////////////////////////////////////////////////////////////////////
// CPickupableObject
/////////////////////////////////////////////////////////////////////
// ������, ������� ����� �����
// ��������� ������� � ��������� �����
void CPickupableObject::Move()
{
    const int FALLING_SPEED = 1;
    int yy = FLOORS_Y[GetCurrentFloor(y)] - GetSprH();
    int j = (x + GetSprW() - 1) / CHR;
    int i;

    // ������ ������ �� ���
    if (y + FALLING_SPEED <= yy)
        y += FALLING_SPEED;
    else if (y < yy)
        y = yy;
    else 
    {
        // ������ ������ ������ ���
        bool floor = false;
        bool ladder = false;
        for (i = x / CHR ; i <= j ; i++)
        {
            CFloor* fl = GetFloorAtXY(i * CHR, y);
            if (!fl || fl->type == CFloor::FLOOR)
                floor = true;
            else if (fl->type == CFloor::LADDER)
                ladder = true;
        }
        if (ladder || !floor)
            y += FALLING_SPEED;
    }

    // �� ������ �� ������
    for (i = x / CHR ; i <= j ; i++)
    {
        CStaticObject* st = GetStaticObjectAtXY(i * CHR, y);
        if (st && st->burning)
        {
            burning = true;
            break;
        }
    }
}



/////////////////////////////////////////////////////////////////////
// CExtinguishingObject
/////////////////////////////////////////////////////////////////////
// ������������ ������ - ���������� true, ���� ������������� ���� ��������
bool CExtinguishingObject::Use(int xx, int yy, enumDir d)
{
    if (water)
    {
        LPDIRECTSOUNDBUFFER sound = NULL;
        // �������� ��� � ����������� �������
        int f = GetCurrentFloor(yy);
        int dx = (d == RIGHT) ? 1 : -1;
        int end = xx / CHR + HowFar() * dx + dx;
        CSprite* enemy = NULL;
        TObjList::iterator i;
        for (i = dynamicObjects.begin() ; i != dynamicObjects.end() ; i++)
            if ((*i)->GetType() == ENEMY)
            {
                enemy = *i;
                break;
            }
        for (int offs = xx / CHR ; offs != end ; offs += dx)
        {
            // �����������
            if (enemy
                && enemy->y == FLOORS_Y[f] - 2 * CHR
                && enemy->x >= (offs - 2) * CHR
                && enemy->x < (offs + 1) * CHR)
            {
                enemy->Extinguish();
            }
            // ���
            if (f < FLOORS_NUM - 1 && offs >= FLOORS_X && offs < CHARS_NUM)
                floors[f][offs - FLOORS_X].Extinguish();
            // ����������� �������
            if (offs >= 0 && offs < CHARS_NUM
                && staticObjects[f][offs])
            {
                if (!sound && staticObjects[f][offs]->burning)
                    sound = PlayDublicateSound(SND_STEAM);

                staticObjects[f][offs]->Extinguish();
                if (staticObjects[f][offs]->GetType() == CLOSEDDOOR)
                    break;
            }
        }

        // ������ ������������ �������

        // ���� ���������
        water--;

        return true;
    }

    return false;
}



/////////////////////////////////////////////////////////////////////
// CBucket
/////////////////////////////////////////////////////////////////////

// �����������
CBucket::CBucket(int xx, int yy)
{ 
    x = xx; 
    y = yy; 
    water = 5; 
    dir = RIGHT;
    burning = false;
//    useTimeout = 0;
}

// ������������ ������ - ���������� true, ���� ������������� ���� ��������
bool CBucket::Use(int xx, int yy, enumDir d)
{
    if (CExtinguishingObject::Use(xx, yy, d))
    {
        // �������� ������ "����"
        dynamicObjects.push_back(new CWater(xx, yy, d));

        PlayDublicateSound(SND_BUCKET);

        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////
// CWater
/////////////////////////////////////////////////////////////////////
// �����������
CWater::CWater(int xx, int yy, enumDir dir)
{ 
    x = xx - (dir == RIGHT ? 0 : GetSprW());
    y = yy + CHR / 2;
    timeout = 6 * 4;
}



/////////////////////////////////////////////////////////////////////
// CTap
/////////////////////////////////////////////////////////////////////
CTap::CTap(int fl, int offs)
{ 
    floor = fl;
    offset = offs;
    x = offs * CHR;
    y = FLOORS_Y[fl] - 2 * CHR;
    hp = 15;
    burning = false;

//    CStaticObject::CStaticObject();
}


/////////////////////////////////////////////////////////////////////
// CExtinguisher
/////////////////////////////////////////////////////////////////////

// �����������
CExtinguisher::CExtinguisher(int xx, int yy)
{
    x = xx;
    y = yy;
    water = 16;
    dir = LEFT;
    burning = false;
}

// ������������ ������ - ���������� true, ���� ������������� ���� ��������
bool CExtinguisher::Use(int xx, int yy, enumDir d)
{
    if (CExtinguishingObject::Use(xx, yy, d))
    {
        // �������� ������ "����"
        dynamicObjects.push_back(new CFoam(xx, yy, d));

        PlayDublicateSound(SND_EXTINGUISHER);
        
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////
// CFoam
/////////////////////////////////////////////////////////////////////
// �����������
CFoam::CFoam(int xx, int yy, enumDir d)
{ 
    x = xx - (d == RIGHT ? 0 : GetSprW());
    y = yy + CHR / 4;
    timeout = 7 * 4;
    dir = d;
}



/////////////////////////////////////////////////////////////////////
// CLadderKit
/////////////////////////////////////////////////////////////////////

// �����������
CLadderKit::CLadderKit(int xx, int yy)
{
    x = xx;
    y = yy;
    dir = LEFT;
    used = false;
    burning = false;
}

// ������������ ������ - ���������� true, ���� ������������� ���� ��������
bool CLadderKit::Use(int xx, int yy, enumDir d)
{
    int f = GetCurrentFloor(yy);
    if (f == 0)
        return false;

    int offs = (xx + (d == LEFT ? CHR / 2 : -CHR)) / CHR;
    
    if (offs < FLOORS_X || offs >= CHARS_NUM)
        return false;

    used = true;
    int f0 = f;

    while (f >= 0)
    {
        if (f != f0)
        {
            floors[f][offs - FLOORS_X].type = CFloor::LADDER;
            floors[f][offs - FLOORS_X].hp = 15;
        }
        if (f > 0
            && (!staticObjects[f][offs]
                || staticObjects[f][offs]->GetType() == NOTHING
                || staticObjects[f][offs]->GetType() == EXPLODEDNOTHING))
        {
            bool burn = false;
            if (staticObjects[f][offs])
            {
                burn = staticObjects[f][offs]->burning;
                delete staticObjects[f][offs];
            }
            staticObjects[f][offs] = new CLadder(f, offs);
            staticObjects[f][offs]->burning = burn;
        }
        f--;
    }

    return true;
}



/////////////////////////////////////////////////////////////////////
// CBonusObject
/////////////////////////////////////////////////////////////////////

// �����������
CBonusObject::CBonusObject(int xx, int yy, int t)
{
    x = xx;
    y = yy;
    type = t;
    burning = false;
    picked = false;
}


// ����� ���� �������
void CBonusObject::PickUp()
{
    const int scores[8] = {10, 20, 25, 50, 75, 100, 125, 150};
    
    picked = true;

    // ���� ���� ����� ����
    if (false)
        score += scores[0];
    // ���� ��� ����� �����, ��
    else
        score += scores[rand() & 7];
}


/////////////////////////////////////////////////////////////////////
// CDoor
/////////////////////////////////////////////////////////////////////
CDoor::CDoor(int fl, int offs, bool op)
{ 
    floor = fl;
    offset = offs;
    x = offs * CHR;
    y = FLOORS_Y[fl] - 2 * CHR;
    hp = 15;
    burning = false;
    open = op;

//    CStaticObject::CStaticObject();
}




/////////////////////////////////////////////////////////////////////
// CAngel
/////////////////////////////////////////////////////////////////////
// �����
// �����������
CAngel::CAngel(int xx, int yy)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    faster = 16;
    stepTimeout = 5;
    maxStepTimeout = 5;
    dy = -1;
    deathTimeout = 0;

    sound = PlayInfiniteSound(SND_ANGEL);
    if (sound)
    {
        sound->Play(0, 0, 0);
    }
}

// ��������� ������� � ��������� �����
void CAngel::Move()
{
/*    if (won && sound)
    {
        sound->Stop();
    }*/
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step = (step + 1) & 3;
        stepTimeout = maxStepTimeout;
    }

    if (faster)
        faster--;
    else
    {
        if (maxStepTimeout > 1)
            maxStepTimeout--;
        if (dy > -CHR/2)
            dy--;
        faster = maxStepTimeout * 3;
    }

    if (y + dy > FLOORS_Y[0] - 2 * CHR)
        y += dy;
    else if (y > FLOORS_Y[0] - 2 * CHR)
        y = FLOORS_Y[0] - 2 * CHR;
    else
        deathTimeout++;
}

// ������ �� ������ ���� ���������
bool CAngel::ToBeDestroyed()
{
    DWORD status;
    if (sound)
        sound->GetStatus(&status);
    else
        status = 0;

    return deathTimeout > 100
            && !(status & DSBSTATUS_PLAYING);
}

// ����������
CAngel::~CAngel()
{
    if (sound)
    {
        sound->Stop();
        sound->Release();
    }
}



/////////////////////////////////////////////////////////////////////
// CDynamite
/////////////////////////////////////////////////////////////////////
CDynamite::CDynamite(int xx, int yy)
{
    x = xx;
    y = yy;
    burning = false;
    dir = RIGHT;

    // �� ������ �� ������ ��� ���������
    int j = (x + GetSprW() - 1) / CHR;
    for (int i = x / CHR ; i <= j ; i++)
    {
        CStaticObject* st = GetStaticObjectAtXY(i * CHR, y);
        CFloor* fl = GetFloorAtXY(i * CHR, y);
        if ((st && st->burning)
            || (fl && fl->type != CFloor::FLOOR))
        {
            burning = true;
            break;
        }
    }
}


void CDynamite::Move()
{
    const int FALLING_SPEED = 1;
    int yy = FLOORS_Y[GetCurrentFloor(y)] - GetSprH();
    int i, j = (x + GetSprW() - 1) / CHR;

    // ������ ������ �� ���
    if (y + FALLING_SPEED <= yy)
    {
        y += FALLING_SPEED;
        return;
    }
    else if (y < yy)
    {
        y = yy;
        return;
    }
    else 
    {
        // ������ ������ ������ ���
        for (i = x / CHR ; i <= j ; i++)
        {
            CFloor* fl = GetFloorAtXY(i * CHR, y);
            if (fl && fl->type != CFloor::FLOOR)
            {
                y += FALLING_SPEED;
                break;
            }
        }
    }

    // �� ������ �� ������
    for (i = x / CHR ; i <= j ; i++)
    {
        CStaticObject* st = GetStaticObjectAtXY(i * CHR, y);
        if (st && st->burning)
        {
            // ������� �����
            int xx = x - CHR;
            if (x > SPECCY_WIDTH - 4 * CHR)
                x = SPECCY_WIDTH - 4 * CHR;
            dynamicObjects.push_back(new CExplosion(x - CHR, FLOORS_Y[GetCurrentFloor(y)] - 3 * CHR));
            burning = true;

            PlayDublicateSound(SND_DYNAMITE);
            break;
        }
    }
}





/////////////////////////////////////////////////////////////////////
// CExplosion
/////////////////////////////////////////////////////////////////////
// �����������
CExplosion::CExplosion(int xx, int yy)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;
}

// ��������� ������� � ��������� �����
void CExplosion::Move()
{
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step++;
        stepTimeout = 8;
    }
}

// ������ �� ������ ���� ���������
bool CExplosion::ToBeDestroyed()
{
    return step >= 8;
}

// ���������� ������
void CExplosion::Destroy()
{
    int f = GetCurrentFloor(y + 2 * CHR);
    for (int i = x / CHR - 2 ; i <= x / CHR + 4 ; i++)
    {
        if (i >= FLOORS_X && i < CHARS_NUM)
        {
            floors[f][i - FLOORS_X].type = CFloor::EXPLODED;
            floors[f][i - FLOORS_X].burning = true;
            if (f > 0)
            {
                floors[f - 1][i - FLOORS_X].type = CFloor::EXPLODED;
                floors[f - 1][i - FLOORS_X].burning = true;
            }

            if (staticObjects[f][i])
                delete staticObjects[f][i];
            staticObjects[f][i] = new CExplodedNothing(f, i);
        }
    }
}




/////////////////////////////////////////////////////////////////////
// CExplodedNothing
/////////////////////////////////////////////////////////////////////
CExplodedNothing::CExplodedNothing(int fl, int offs) 
{ 
    floor = fl;
    offset = offs; 
    x = offs * CHR;
    y = FLOORS_Y[fl] - 2 * CHR;
    hp = 15;
    burning = true;

//    CStaticObject::CStaticObject();
}


// ��������� ������� � ��������� �����
void CExplodedNothing::Move()
{
    if (hp)
        CStaticObject::Move();
    else
        burning = false;
}




/////////////////////////////////////////////////////////////////////
// CFallingPlayer
/////////////////////////////////////////////////////////////////////
// �����������
CFallingPlayer::CFallingPlayer(int xx, int yy)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;
}

// ��������� ������� � ��������� �����
void CFallingPlayer::Move()
{
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step = (step + 1) & 1;
        stepTimeout = 8;
    }
    y += 2;
}

// ������ �� ������ ���� ���������
bool CFallingPlayer::ToBeDestroyed()
{
    int f = GetCurrentFloor(y);
    // ������ �� 1� ����
    if (f == FLOORS_NUM - 1)
        return y + 2 * CHR >= FLOORS_Y[FLOORS_NUM - 1];

    CFloor* fl1 = GetFloorAtXY(x + CHR /* / 2*/, y);
//    CFloor* fl2 = GetFloorAtXY(x + (3 * CHR) / 2, y);
    // ����� ������
    if ((fl1->type != CFloor::FLOOR
         && fl1->type != CFloor::LADDER)
/*        || (fl2->type != CFloor::FLOOR
            && fl2->type != CFloor::LADDER)*/)
        return false;

    // ������ �� ����� ����
    return y + 2 * CHR >= FLOORS_Y[f];
}

// ���������� ������
void CFallingPlayer::Destroy()
{
    dynamicObjects.push_back(new CAngel(x, y));
}




/////////////////////////////////////////////////////////////////////
// CElephant
/////////////////////////////////////////////////////////////////////
// ���������, ����� ����� timeout
int CElephant::createTimeout;
// �����������
CElephant::CElephant(int xx, int yy)
{
    LPDIRECTSOUNDBUFFER snd = NULL;

    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;
    createTimeout = 64 * 4;
    water = 0xffff;

    for (int i = 0 ; i < FLOORS_WIDTH ; i++)
    {
        floors[0][i].Extinguish();
        if (staticObjects[0][i + FLOORS_X])
        {
            if (!snd && staticObjects[0][i + FLOORS_X]->burning)
                snd = PlayDublicateSound(SND_STEAM);

            staticObjects[0][i + FLOORS_X]->Extinguish();
        }
    }

    sound = PlayInfiniteSound(SND_ELEPHANT);
    if (sound)
    {
        sound->Play(0, 0, DSBPLAY_LOOPING);
    }
}

// ��������� ������� � ��������� �����
void CElephant::Move()
{
/*    if (won && sound)
    {
        sound->Stop();
    }*/
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step = (step + 1) % 6;
        stepTimeout = 8;
        x -= 6;
        Use(x, y, LEFT);
    }
}

// ������ �� ������ ���� ���������
bool CElephant::ToBeDestroyed()
{
    return x <= FLOORS_X * CHR;
}


// ����������
CElephant::~CElephant()
{
    if (sound)
    {
        sound->Stop();
        sound->Release();
    }
}


/////////////////////////////////////////////////////////////////////
// CMan
/////////////////////////////////////////////////////////////////////
// �����������
CMan::CMan(int xx, int yy, int n)
{
    dx = 0;
    dy = 0;
    cnt = 3;
    x = xx;
    y = yy;
    burning = false;
    fasterTimeout = 0;
    step = 0;
    dir = LEFT;
    num = n;
}



// ����������� �������
void CMan::CopyToSurface(LPDIRECTDRAWSURFACE pSurf)
{
    // �������, ��������������� �����
    const int spr[17] = {0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 9, 9};
    const int spr4[17] = {0, 1, 2, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9};

    RECT r;
    // ����� �� ��������
    if (dy != 0)
    {
        int s = step % 16;
        r.left = MAN_X[num] + (step / 16) * 2 * CHR;
        r.top = MAN_Y[num] + (s >= 8 ? 15 - s : s) * 2 * CHR;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 32;
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // ���������� ������
        r.left = MAN_X[num] + (step / 16) * 2 * CHR;
        r.top = MAN_Y[num];
        r.right = r.left + GetSprW();
        r.bottom = r.top + 10;
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        // ���������� ������ � ��������
        r.left = MAN_X[num] + ((dir == LEFT) ? 64 : 64 + 32);
        r.top = MAN_Y[num];
        r.right = r.left + GetSprW();
        r.bottom = r.top + 32 - 13;

        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // �������� ��������
        int m = (abs(dx) * CHR) / 4;
        int s = (step <= m) ? step : (2 * m - step);
        if (s < 0)
            s = 0;
        
        if (abs(dx) == 4)
            r.top = MAN_Y[num] + 32 - 13 + spr4[s] * 13;
        else
            r.top = MAN_Y[num] + 32 - 13 + spr[s] * 13;
        r.bottom = r.top + 13;
        
        pSurf->BltFast(x + X0, y + Y0 + 32 - 13, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}



// ��������� ������� � ��������� �����
void CMan::Move()
{
    // changeSpeed
    const int MAX_FASTER = CHR / 2;
    if (fasterTimeout)
        fasterTimeout--;

    CStaticObject* st;
    CFloor* fl;
//    CFloor* fl1;

    // �������� ���
    if (dx)
        step = (step + abs(dx)) % ((abs(dx) * CHR) / 2);
    else if (dy)
        step = (step + 1) % 32;
    else
        step = 0;

    // ����� �� ��������
    if (dy && y != FLOORS_Y[GetCurrentFloor(y)] - GetSprH())
    {
        y += dy;
        return;
    }

    // �� ������ �� �������
    st = GetStaticObjectAtXY(x + CHR / 2, y);
    if (st)
    {
        st->PassThru();
        if (st->burning)
            burning = true;
    }
    st = GetStaticObjectAtXY(x + 3 * CHR / 2 - 1, y);
    if (st)
    {
        st->PassThru();
        if (st->burning)
        {
            burning = true;
        }
    }
    if (burning)
    {
        dynamicObjects.push_back(new CBurningMan(x, y, num));

        PlayDublicateSound(SND_MANDIE[num]);
    }
    // �� ���������� �� �������
    fl = GetFloorAtXY(x + CHR /*/ 2*/, y);
//    fl1 = GetFloorAtXY(x + 3 * CHR / 2 - 1, y);
    if (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED)
       /* && fl1 && (fl1->type == CFloor::EMPTY || fl1->type == CFloor::EXPLODED) */ )
    {
        burning = true;
        dynamicObjects.push_back(new CFallingMan(x, y, num));

        PlayDublicateSound(SND_MANDIE[num]);
    }

    // �� ����� �� ����� ���-����
    st = GetStaticObjectAtXY(x - CHR / 2, y);
    fl = GetFloorAtXY(x - CHR / 2, y);
    enumDir runDir = NONE;
    if ((st && st->burning)
        || (st && st->GetType() == CLOSEDDOOR && (rand() & 0xff) < 0x40)
        || (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED)))
    {
        runDir = RIGHT;
    }
    //else
    if (runDir == NONE || (rand() & 0xff) < 0x80)
    {
        st = GetStaticObjectAtXY(x + GetSprW() + CHR / 2 - 1, y);
        fl = GetFloorAtXY(x + GetSprW() + CHR / 2 - 1, y);
        if ((st && st->burning)
            || (st && st->GetType() == CLOSEDDOOR && (rand() & 0xff) < 0x40)
            || (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED)))
        {
            runDir = LEFT;
        }
    }
    if (runDir == RIGHT)
    {
        dy = 0;
        if (dir == LEFT || (!fasterTimeout && dx < CHR / 4))
        {
            dx++;
            fasterTimeout = MAX_FASTER;
        }
    }
    else if (runDir == LEFT)
    {
        dy = 0;
        if (dir == RIGHT || (!fasterTimeout && dx > -CHR / 4))
        {
            dx--;
            fasterTimeout = MAX_FASTER;
        }
    }
    else 
    {
        // ��� �� ����� ��������
        fl = GetFloorAtXY(x + CHR, y);
        st = GetStaticObjectAtXY(x + CHR, y);
        if (fl && fl->type == CFloor::LADDER && cnt >= 0)
        {
            // ���� �������� ����� � ��� �� �����
            st = GetStaticObjectAtXY(x + CHR, y + 3 * CHR);
            if (st && !st->burning && st->GetType() == LADDER)
            {
                if (!dy)
                    step = 0;
                x = fl->x - CHR / 2;//((x + CHR / 2) / CHR) * CHR - CHR / 2;
                dx = 0;
                dy = 1;
                dir = DOWN;
                cnt = 4;
            }
        }
        else if (st && st->GetType() == LADDER && !cnt)
        {
            // ���� �������� ������
            fl = GetFloorAtXY(x + CHR, y - 3 * CHR);
            st = GetStaticObjectAtXY(x + CHR, y - 3 * CHR);
            if ((fl && fl->type == CFloor::LADDER)
                && (!st || !st->burning))
            {
                step = 8;
                x = st->x - CHR / 2;
                dx = 0;
                dy = -1;
                dir = UP;
                cnt = -4;
            }
        }
        else
        {
            dy = 0;
            // �����
            if (!fasterTimeout)
            {
                if (dx == 0 && (dir == UP || dir == DOWN))
                {
                    if (dir == DOWN)
                        dx--;
                    else // if (dir == UP)
                        dx++;
                }
                else if ((dx > 0 || dir == RIGHT) && dx < CHR / 4)
                    dx++;
                else if ((dx < 0 || dir == LEFT) && dx > -CHR / 4)
                    dx--;
                
                fasterTimeout = MAX_FASTER;
            }
        }
    }

    if (dx)
    {
        if (GetCurrentFloor(y) < FLOORS_NUM - 1 && x + dx < FLOORS_X * CHR)
        {
//            step = 0;
            dx = CHR / 8;
//            runDir = RIGHT;
            fasterTimeout = MAX_FASTER;
        }
        else if (x + dx > SPECCY_WIDTH - GetSprW())
        {
//            step = 0;
            dx = -CHR / 8;
//            runDir = LEFT;
            fasterTimeout = MAX_FASTER;
        }
    }
/*
    if (runDir != NONE)
    {
        if (dir != runDir)
            step = 0;
        dir = runDir;
    }
*/
    runDir = dx < 0 ? LEFT : (dx > 0 ? RIGHT : NONE);
    if (runDir != NONE)
    {
        if (dir != runDir)
        {
            step = 0;
            if (cnt > 0)
                cnt--;
            else if (cnt < 0)
                cnt++;
            // ����������� ��������
            if (abs(dx) == 1)
                dx = 0;
        }
        dir = runDir;
    }

    x += dx;
    y += dy;
}


// ���������� ������
void CMan::Destroy()
{
    const int bonus[8] = {10, 20, 25, 50, 75, 100, 125, 150};
    if (!burning)
    {
        score += bonus[(level & 0xe) >> 1];
    }
}



/////////////////////////////////////////////////////////////////////
// CFallingMan
/////////////////////////////////////////////////////////////////////
// �����������
CFallingMan::CFallingMan(int xx, int yy, int n)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;
    num = n;
}

// ��������� ������� � ��������� �����
void CFallingMan::Move()
{
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step = (step + 1) & 1;
        stepTimeout = 8;
    }
    y += 2;
}

// ������ �� ������ ���� ���������
bool CFallingMan::ToBeDestroyed()
{
    int f = GetCurrentFloor(y);
    // ������ �� 1� ����
    if (f == FLOORS_NUM - 1)
        return y + 2 * CHR >= FLOORS_Y[FLOORS_NUM - 1];

    CFloor* fl1 = GetFloorAtXY(x + CHR /*/ 2*/, y);
//    CFloor* fl2 = GetFloorAtXY(x + (3 * CHR) / 2, y);
    // ����� ������
    if (fl1 && fl1->type != CFloor::FLOOR
            && fl1->type != CFloor::LADDER)
/*        && fl2->type != CFloor::FLOOR
        && fl2->type != CFloor::LADDER)*/
        return false;

    // ������ �� ����� ����
    return y + 2 * CHR >= FLOORS_Y[f];
}

// ���������� ������
void CFallingMan::Destroy()
{
    // �������� ����
    deadMen++;
}



/////////////////////////////////////////////////////////////////////
// CBurningMan
/////////////////////////////////////////////////////////////////////
// �����������
CBurningMan::CBurningMan(int xx, int yy, int n)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;
    num = n;
}

// ��������� ������� � ��������� �����
void CBurningMan::Move()
{
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step++;
        stepTimeout = 8;
    }
}

// ������ �� ������ ���� ���������
bool CBurningMan::ToBeDestroyed()
{
    return step >= 4;
}

// ���������� ������
void CBurningMan::Destroy()
{
    // �������� ����
    deadMen++;
}



/////////////////////////////////////////////////////////////////////
// CEnemy
/////////////////////////////////////////////////////////////////////
// �����������
CEnemy::CEnemy(int xx, int yy)
{
    dx = 0;
    dy = 0;
    x = xx;
    y = yy;
    burning = false;
    fasterTimeout = 0;
    step = 0;
    dir = LEFT;
    useTimeout = 0;
    extinguished = false;
}



// ����������� �������
void CEnemy::CopyToSurface(LPDIRECTDRAWSURFACE pSurf)
{
    // �������, ��������������� �����
    const int spr[13] = {0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8};

    RECT r;
    // ����� �� ��������
    if (dy != 0)
    {
        int s = step % 16;
        r.left = GetSprX() + (step / 16) * 2 * CHR;
        r.top = GetSprY() + (s >= 8 ? 15 - s : s) * 2 * CHR;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 32;
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
/*
        // ���������� ������
        r.left = GetSprX() + (step / 16) * 2 * CHR;
        r.top = GetSprY();
        r.right = r.left + GetSprW();
        r.bottom = r.top + 10;
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            */
    }
    else if (useTimeout)
    {
        r.left = GetSprX() + (dir == LEFT ? 64 : 64 + 32);
        r.top = 1074;
        r.right = r.left + GetSprW();
        r.bottom = r.top + GetSprH();
        
        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        // �������� ��������
        int m = (abs(dx) * CHR) / 4;
        int s = (step <= m) ? step : (2 * m - step);
        if (s < 0)
            s = 0;
        // ���������� ������
        r.left = GetSprX() + ((dir == LEFT) ? 64 : 64 + 32);
        r.top = GetSprY() + 13 + (s / 4) * 14;
        r.right = r.left + GetSprW();
        r.bottom = r.top + 14;

        pSurf->BltFast(x + X0, y + Y0, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        // ���������� ��������
        r.left = GetSprX() + ((dir == LEFT) ? 64 : 64 + 32);
        r.top = GetSprY();
        r.right = r.left + GetSprW();
        r.bottom = r.top + 13;

        pSurf->BltFast(x + X0, y + Y0 + 14, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

        r.top = GetSprY() + 13 + 14 * 4 + spr[s] * 5;
        r.bottom = r.top + 5;
        
        pSurf->BltFast(x + X0, y + Y0 + 13 + 14, pSpritesSurf, &r, 
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}



// ��������� ������� � ��������� �����
void CEnemy::Move()
{
    // changeSpeed
    const int MAX_FASTER = CHR / 2;
    if (fasterTimeout)
        fasterTimeout--;

    CStaticObject* st;
    CFloor* fl;

    if (useTimeout)
    {
        useTimeout--;
        return;
    }

    // �������� ���
    if (dx)
        step = (step + abs(dx)) % ((abs(dx) * CHR) / 2);
    else if (dy)
        step = (step + 1) % 32;
    else
        step = 0;

    // ����� �� ��������
    if (dy && y != FLOORS_Y[GetCurrentFloor(y)] - GetSprH())
    {
        y += dy;
        return;
    }

    // ������� �����, etc
    st = GetStaticObjectAtXY(x, y);
    if (st)
        st->PassThru();
    // ������� �����, etc
    st = GetStaticObjectAtXY(x + GetSprW() - 1, y);
    if (st)
        st->PassThru();

    // �� ���������� ��
    fl = GetFloorAtXY(x + CHR /*/ 2*/, y);
    if (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED))
    {
        burning = true;
        dynamicObjects.push_back(new CFallingEnemy(x, y));
    }

    // ������ ��� � �������� �����
    st = GetStaticObjectAtXY(x - CHR / 2, y);
    fl = GetFloorAtXY(x - CHR / 2, y);
    enumDir runDir = NONE;

    if ((st && st->GetType() == CLOSEDDOOR && (rand() & 0xff) < 0xc0)
        || (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED)))
    {
        runDir = RIGHT;
    }
    //else
    if (runDir == NONE || (rand() & 0xff) < 0x80)
    {
        st = GetStaticObjectAtXY(x + GetSprW() + CHR / 2 - 1, y);
        fl = GetFloorAtXY(x + GetSprW() + CHR / 2 - 1, y);
        if ((st && st->GetType() == CLOSEDDOOR && (rand() & 0xff) < 0xc0)
            || (fl && (fl->type == CFloor::EMPTY || fl->type == CFloor::EXPLODED)))
        {
            runDir = LEFT;
        }
    }
    if (runDir == RIGHT)
    {
        dy = 0;
        if (dir == LEFT || (!fasterTimeout && dx < 3))
        {
            dx++;
            fasterTimeout = MAX_FASTER;
        }
    }
    else if (runDir == LEFT)
    {
        dy = 0;
        if (dir == RIGHT || (!fasterTimeout && dx > -3))
        {
            dx--;
            fasterTimeout = MAX_FASTER;
        }
    }
    else 
    {
        // ��� �� ����� ��������
        fl = GetFloorAtXY(x + CHR, y);
        st = GetStaticObjectAtXY(x + CHR, y);

        bool canDown = fl && fl->type == CFloor::LADDER;
        bool canUp = st && st->GetType() == LADDER;

        if (GetCurrentFloor(y) < FLOORS_NUM - 2
            && canDown
            && (rand() & 0xff) < abs(dx) * 10)
        {
            // ���� �������� �����
            st = GetStaticObjectAtXY(x + CHR, y + 3 * CHR);
            if (st && st->GetType() == LADDER)
            {
                if (!dy)
                    step = 0;
                x = fl->x - CHR / 2;//((x + CHR / 2) / CHR) * CHR - CHR / 2;
                dx = 0;
                dy = 1;
                dir = DOWN;
            }
        }
        else if (GetCurrentFloor(y) > 1
            && canUp
            && (rand() & 0xff) < abs(dx) * 11)
        {
            // ���� �������� ������
            fl = GetFloorAtXY(x + CHR, y - 3 * CHR);
            if (fl && fl->type == CFloor::LADDER)
            {
                step = 8;
                x = st->x - CHR / 2;
                dx = 0;
                dy = -1;
                dir = UP;
            }
        }
        else
        {
            dy = 0;
            // �����
            if (!fasterTimeout)
            {
                if (dx == 0 && (dir == UP || dir == DOWN 
                                || (rand() & 0xff) < 0x40))
                {
                    dx = (rand() % 3) - 1;
                }
                else if ((dx > 0 || dir == RIGHT) && dx < 3)
                    dx++;
                else if ((dx < 0 || dir == LEFT) && dx > -3)
                    dx--;
                
                fasterTimeout = MAX_FASTER;
            }
        }

        if (!canUp && !canDown && (rand() & 0xff) < 3)
        {
            st = GetStaticObjectAtXY(x + (dir == LEFT ? 0 : GetSprW() - 1), y);
            if (st)
            {
                dy = 0;
                dx = 0;
                st->burning = true;
                useTimeout = CHR;
            }
        }
    }

    if (dx)
    {
        if (x + dx < FLOORS_X * CHR)
        {
            dx = CHR / 8;
            fasterTimeout = MAX_FASTER;
        }
        else if (x + dx > SPECCY_WIDTH - GetSprW())
        {
            dx = -CHR / 8;
            fasterTimeout = MAX_FASTER;
        }
    }

    runDir = dx < 0 ? LEFT : (dx > 0 ? RIGHT : NONE);
    if (runDir != NONE)
    {
        if (dir != runDir)
        {
            step = 0;
            // ����������� ��������
            if (abs(dx) == 1)
                dx = 0;
        }
        dir = runDir;
    }

    x += dx;
    y += dy;
}


// ���������� ������
void CEnemy::Destroy()
{
    if (extinguished)
    {
        int f = GetCurrentFloor(y);
        for (int i = x / CHR ; i < x / CHR + 5 ; i++)
            if (i >= FLOORS_X && i < CHARS_NUM)
                floors[f][i - FLOORS_X].burning = true;
    }
}


// �������� �����
void CEnemy::Extinguish()
{
    score += 200;
    extinguished = true;
    dynamicObjects.push_back(new CFallingEnemy(x, y));
}




/////////////////////////////////////////////////////////////////////
// CFallingEnemy
/////////////////////////////////////////////////////////////////////
// �����������
CFallingEnemy::CFallingEnemy(int xx, int yy)
{
    x = xx;
    y = yy;
    burning = false;
    step = 0;
    stepTimeout = 8;

    sound = PlayInfiniteSound(SND_ENEMY);
    if (sound)
    {
        sound->Play(0, 0, 0);
    }
}

// ��������� ������� � ��������� �����
void CFallingEnemy::Move()
{
/*    if (won && sound)
    {
        sound->Stop();
    }*/
    if (stepTimeout) 
        stepTimeout--;
    else
    {
        step = (step + 1) & 1;
        stepTimeout = 8;
    }
    y += 2;
}

// ������ �� ������ ���� ���������
bool CFallingEnemy::ToBeDestroyed()
{
    return y >= FLOORS_Y[FLOORS_NUM - 1] - 2 * CHR;
}


// ���������� ������ 
void CFallingEnemy::Destroy()
{
    PlayDublicateSound(SND_ENEMYDIE);
}


// ����������
CFallingEnemy::~CFallingEnemy()
{
    if (sound)
    {
        sound->Stop();
        sound->Release();
    }
}
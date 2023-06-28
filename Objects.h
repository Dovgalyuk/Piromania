#ifndef OBJECTS_H
#define OBJECTS_H

#include <windows.h>
#include "Piromania.h"
#include "graphics.h"
#include "Sounds.h"


// ����������� ��������
enum enumDir {LEFT, RIGHT, NONE, UP, DOWN};


class CUsableObject;



// ����������� ����� ��� ���� ��������
class CSprite
{
public:
    // ����������
    int x, y;
    // ����� �� ������
    bool burning;
    
    enum sprType
    {
        NOTHING,
            FLOOR,
            LADDER,
            PLAYER,
            BUCKET,
            WATER,
            TAP,
            EXTINGUISHER,
            FOAM,
            SMOKE,
            LADDERKIT,
            BONUS,
            OPENDOOR,
            CLOSEDDOOR,
            ANGEL,
            DYNAMITE,
            EXPLOSION,
            EXPLODEDNOTHING,
            FALLINGPLAYER,
            ELEPHANT,
            MAN,
            FALLINGMAN,
            BURNINGMAN,
            ENEMY,
            FALLINGENEMY
    };
    
    // ��������� �������
    virtual int GetSprX() = 0;
    virtual int GetSprY() = 0;
    virtual int GetSprW() = 0;
    virtual int GetSprH() = 0;
    
    virtual ~CSprite() {};
    
    // ��� �������
    virtual sprType GetType() = 0;

    // ��������� ������� � ��������� �����
    virtual void Move() {};
    // ����������� �������
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // ������������ �� ������ � ���������������
    virtual bool Intersects(LPRECT r, LPRECT dr = NULL);
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return false; }
    // ���������� ������
    virtual void Destroy() {}
    // �������� �����
    virtual void Extinguish();
    // ���������� ����, ��������� � ��������
    virtual void StopSound() {}
    // ����������� ����
    virtual void ResumeSound() {}
};



// ���������� ����� �������
class CFloor : public CSprite
{
public:
    // ��� ����������
    enum {EMPTY, FLOOR, EXPLODED, LADDER} type;
    // ��������� ������ �������� (0xf - �����, 0 - ��������)
    int hp;
    // ���������� � �������
    int offset, floor;
/*
    // �����������
    CFloor(int xx, int yy);
*/
    // ��������� �������
    virtual int GetSprX();
    virtual int GetSprY();
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR; };

    // ��������� ������� � ��������� �����
    virtual void Move();
    // ��� �������
    virtual sprType GetType() { return CSprite::FLOOR; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return false; };
};



// ����������� ������
class CStaticObject : public CSprite
{
public:
    // ��������� ������ �������� (0xf - �����, 0 - ��������)
    int hp;
    // ���������� � �������
    int offset, floor;
    // �����
    int fire[CHR * 2][CHR];
    int fireRandom[256];
    int fireDx[256];
    int iRand;
    int iDx;

    CStaticObject();
    // ����������� �������
//    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return hp == 0; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // �������� �����
    virtual void Extinguish();
    // ����� �������� ����� ������
    virtual void PassThru() {};
    // �������� ������� ����
    void UpdateFire(/*DDSURFACEDESC &ddSurfaceDesc*/);
    // ����������� ����������
    virtual ~CStaticObject();
};



// ������ ������
class CNothing : public CStaticObject
{
public:
    // �����������
    CNothing(int fl, int offs);

    // ��������� �������
    virtual int GetSprX() { return 208; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return NOTHING; };
};




// ��������
class CLadder : public CStaticObject
{
public:
    // �����������
    CLadder(int fl, int offs);

    // ��������� �������
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return LADDER; };
};



// �����
class CPlayer : public CSprite
{
public:
    // ����������� ��������
    int dx, dy;
    // ������ ����������
    int slower;
    // ������ �������
    int faster;
    // ��� �������� �� �����������
    int step;
    // ������ � �����
    CUsableObject* objInHands;
    // ����� ��� ��� ������ ������
    int droppedX;
    // ����� �� ���������� ������������� ��������
    int useTimeout;
    // ����� ���������� �����
    int fillTimeout;
    // ����� �� ��������/�������� �����
    int doorTimeout;

    // ����������� ���������� ��������
    enumDir dir;

    // �����������
    CPlayer(int xx, int yy);
    // ����������
    ~CPlayer();

    // ��������� �������
    virtual int GetSprX();
    virtual int GetSprY();
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ����������� �������
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // ��� �������
    virtual sprType GetType() { return PLAYER; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return burning; };
    // ���������� ������
    virtual void Destroy() {};
};




// ������, ������� ����� ����� 
class CPickupableObject : public CSprite
{
public:
    // ������ �� ������
//    bool burned;
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return burning; };
};



// ������, ������� ����� ����� � ������������
class CUsableObject : public CPickupableObject
{
public:
    // ���������� � ������������
    enumDir dir;
    // ������������ ������ - ���������� true, ���� ������������� ���� ��������
    virtual bool Use(int xx, int yy, enumDir dir) { return false; };
    /*
    // ��������� ������� � ��������� �����
    virtual void Move();
    */
    // ���������� useTimeout
    virtual int GetUseTimeout() { return 0; };
    /*
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return false; };
    */
};



// ������, ������������ ��� ������� ������
class CExtinguishingObject : public CUsableObject
{
public:
    // ���������� ���� (����) � �������
    int water;

    // ��������� ��������
    virtual int HowFar() = 0;
    // ������������ ������ - ���������� true, ���� ������������� ���� ��������
    virtual bool Use(int xx, int yy, enumDir d);
};



// �����
class CBucket : public CExtinguishingObject
{
public:
    // �����������
    CBucket(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return dir == RIGHT ? 256 : 274; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 18; };
    virtual int GetSprH() { return 20; };

    // ��� �������
    virtual sprType GetType() { return BUCKET; };
    
    // ������������ ������ - ���������� true, ���� ������������� ���� ��������
    virtual bool Use(int xx, int yy, enumDir d);
    // ���������� useTimeout
    virtual int GetUseTimeout() { return 6 * 4; };
    // ��������� ��������
    virtual int HowFar() { return 4; };
};




// ������� ����
class CWater : public CSprite
{
public:
    // ����� �����
    int timeout;
    // �����������
    CWater(int xx, int yy, enumDir dir);

    // ��������� �������
    virtual int GetSprX() { return 292; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 16; };

    // ��� �������
    virtual sprType GetType() { return WATER; };
    // ��������� ������� � ��������� �����
    virtual void Move() { if (timeout) timeout--; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return !timeout; };
};



// ����
class CTap : public CStaticObject
{
public:
    // �����������
    CTap(int fl, int offs);

    // ��������� �������
    virtual int GetSprX() { return 324; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return TAP; };
};




// ������������
class CExtinguisher : public CExtinguishingObject
{
public:
    // �����������
    CExtinguisher(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return dir == LEFT ? 340 : 360; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 20; };
    virtual int GetSprH() { return 24; };

    // ��� �������
    virtual sprType GetType() { return EXTINGUISHER; };
    
    // ������������ ������ - ���������� true, ���� ������������� ���� ��������
    virtual bool Use(int xx, int yy, enumDir d);
    // ���������� useTimeout
    virtual int GetUseTimeout() { return 7 * 4; };
    // ��������� ��������
    virtual int HowFar() { return 15; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return !water || burning; };
};



// ���� �� ������������
class CFoam : public CSprite
{
public:
    // �����������
    enumDir dir;
    // ����� �����
    int timeout;
    // �����������
    CFoam(int xx, int yy, enumDir d);

    // ��������� �������
    virtual int GetSprX() { return dir == RIGHT ? 380 : 412; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 24; };

    // ��� �������
    virtual sprType GetType() { return FOAM; };
    // ��������� ������� � ��������� �����
    virtual void Move() { if (timeout) timeout--; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return !timeout; };
};



// ���
class CSmoke : public CSprite
{
public:
    // ����� �����
    int timeout;
    // �����������
    CSmoke(int xx, int yy) { x = xx; y = yy; timeout = 7 * 4; };

    // ��������� �������
    virtual int GetSprX() { return 128 + (7 - timeout / 4) * CHR; };
    virtual int GetSprY() { return 352; };
    virtual int GetSprW() { return 16; };
    virtual int GetSprH() { return 32; };

    // ��� �������
    virtual sprType GetType() { return SMOKE; };
    // ��������� ������� � ��������� �����
    virtual void Move() { if (timeout) timeout--; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return !timeout; };
};



// �������������� ��������
class CLadderKit : public CUsableObject
{
public:
    // ������������ �� ��������
    bool used;
    // �����������
    CLadderKit(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return 444; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 18; };

    // ��� �������
    virtual sprType GetType() { return LADDERKIT; };
    
    // ������������ ������ - ���������� true, ���� ������������� ���� ��������
    virtual bool Use(int xx, int yy, enumDir d);
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return used || burning; };
};




// ����
class CBonusObject : public CPickupableObject
{
public:
    // ��� �������
    int type;
    // ������ ������
    bool picked;
    // �����������
    CBonusObject(int xx, int yy, int t);

    // ��������� �������
    virtual int GetSprX() { return 256 + type * 32; };
    virtual int GetSprY() { return 352; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 24; };

    // ��� �������
    virtual sprType GetType() { return BONUS; };
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return burning || picked; };
    // ����� ���� �������
    virtual void PickUp();
};



// �����
class CDoor : public CStaticObject
{
public:
    // ������� �� �����
    bool open;
    // �����������
    CDoor(int fl, int offs, bool op);

    // ��������� �������
    virtual int GetSprX() { return 476 + (open ? 0 : 16); };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return open ? OPENDOOR : CLOSEDDOOR; };
    // ����� �������� ����� ������
    virtual void PassThru() { if (!open) PlayDublicateSound(SND_DOOR); open = true; };
    // ������� ��� ������� �����
    void Toggle() { open = !open; PlayDublicateSound(SND_DOOR); };
};



// �����
class CAngel : public CSprite
{
public:
    // ��������
    int dy;
    // ������ �������
    int faster;
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;
    int maxStepTimeout;
    // ����� ���������� �� ������� �����
    int deathTimeout;
    // ����
    LPDIRECTSOUNDBUFFER sound;

    // �����������
    CAngel(int xx, int yy);
    // ����������
    virtual ~CAngel();

    // ��������� �������
    virtual int GetSprX() { return 320 + 32 * step; };
    virtual int GetSprY() { return 320; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return ANGEL; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ����, ��������� � ��������
    virtual void StopSound() { if (sound) sound->Stop(); }
    // ����������� ����
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, 0); }
};



// �������
class CDynamite : public CUsableObject
{
public:
    // �����������
    CDynamite(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return (dir == RIGHT) ? 496 : 515; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 19; };

    // ��� �������
    virtual sprType GetType() { return DYNAMITE; };
    // ��������� ������� � ��������� �����
    virtual void Move();
};



// �����
class CExplosion : public CSprite
{
public:
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;

    // �����������
    CExplosion(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return 64 * step; };
    virtual int GetSprY() { return 890; };
    virtual int GetSprW() { return CHR * 4; };
    virtual int GetSprH() { return CHR * 4; };

    // ��� �������
    virtual sprType GetType() { return EXPLOSION; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ������
    virtual void Destroy();
};



// ������ ������
class CExplodedNothing : public CStaticObject
{
public:
    // �����������
    CExplodedNothing(int fl, int offs);

    // ��������� �������
    virtual int GetSprX() { return 208; };
    virtual int GetSprY() { return 544; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return EXPLODEDNOTHING; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return false; };
};




// �������� ��������
class CFallingPlayer : public CSprite
{
public:
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;

    // �����������
    CFallingPlayer(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return 448; };
    virtual int GetSprY() { return 288 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return FALLINGPLAYER; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ������
    virtual void Destroy();
};



// ����
class CElephant : public CExtinguishingObject
{
public:
    // ���������, ����� ����� timeout
    static int createTimeout;
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;
    // ����
    LPDIRECTSOUNDBUFFER sound;

    // �����������
    CElephant(int xx, int yy);
    // ����������
    ~CElephant();

    // ��������� �������
    virtual int GetSprX() { return 376; };
    virtual int GetSprY() { return 640 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return ELEPHANT; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ��������� ��������
    virtual int HowFar() { return 7; };
    // ���������� ����, ��������� � ��������
    virtual void StopSound() { if (sound) sound->Stop(); }
    // ����������� ����
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, DSBPLAY_LOOPING); }
};


const int MAN_NUM = 2;
const int MAN_X[MAN_NUM] = {272, 0};
const int MAN_Y[MAN_NUM] = {32, 960};

// �������
class CMan : public CSprite
{
public:
    // ���� ��������
    int step;
    /*
    // ��������� �������
    int stepTimeout;
    */
    // ����������� ��������
    int dx, dy;
    // ������� (������?)
    int cnt;
    // ������ ��������� ��������
    int fasterTimeout;
    // ����������� ��������
    enumDir dir;
    // ��� ��������
    int num;

    // �����������
    CMan(int xx, int yy, int n);

    // ��������� �������
    virtual int GetSprX() { return 336; };
    virtual int GetSprY() { return 32; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return MAN; };
    // ����������� �������
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return burning || x <= 0; };
    // ���������� ������
    virtual void Destroy();
};



// �������� �������
class CFallingMan : public CSprite
{
public:
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;
    // ��� ��������
    int num;

    // �����������
    CFallingMan(int xx, int yy, int n);

    // ��������� �������
    virtual int GetSprX() { return MAN_X[num] + 64; };
    virtual int GetSprY() { return MAN_Y[num] + 149 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return FALLINGMAN; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ������
    virtual void Destroy();
};


// ������� �������
class CBurningMan : public CSprite
{
public:
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;
    // ��� ��������
    int num;

    // �����������
    CBurningMan(int xx, int yy, int n);

    // ��������� �������
    virtual int GetSprX() { return MAN_X[num] + 96; };
    virtual int GetSprY() { return MAN_Y[num] + 149 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return BURNINGMAN; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ������
    virtual void Destroy();
};



// �����������
class CEnemy : public CSprite
{
public:
    // ���� ��������
    int step;
    // ����������� ��������
    int dx, dy;
    // ������ ��������� ��������
    int fasterTimeout;
    // ����������� ��������
    enumDir dir;
    // ������ ����������
    int useTimeout;
    // �������
    bool extinguished;

    // �����������
    CEnemy(int xx, int yy);

    // ��������� �������
    virtual int GetSprX() { return 128; };
    virtual int GetSprY() { return 960; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return ENEMY; };
    // ����������� �������
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed() { return burning || extinguished; };
    // ���������� ������
    virtual void Destroy();
    // ��������
    virtual void Extinguish();
};



// �������� �����������
class CFallingEnemy : public CSprite
{
public:
    // ���� ��������
    int step;
    // ��������� �������
    int stepTimeout;
    // ����
    LPDIRECTSOUNDBUFFER sound;

    // �����������
    CFallingEnemy(int xx, int yy);
    // ����������
    virtual ~CFallingEnemy();

    // ��������� �������
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return 1109 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // ��� �������
    virtual sprType GetType() { return FALLINGENEMY; };
    // ��������� ������� � ��������� �����
    virtual void Move();
    // ������ �� ������ ���� ���������
    virtual bool ToBeDestroyed();
    // ���������� ������
    virtual void Destroy();
    // ���������� ����, ��������� � ��������
    virtual void StopSound() { if (sound) sound->Stop(); }
    // ����������� ����
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, 0); }
};


#endif
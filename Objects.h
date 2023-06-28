#ifndef OBJECTS_H
#define OBJECTS_H

#include <windows.h>
#include "Piromania.h"
#include "graphics.h"
#include "Sounds.h"


// направления движения
enum enumDir {LEFT, RIGHT, NONE, UP, DOWN};


class CUsableObject;



// абстрактный класс для всех объектов
class CSprite
{
public:
    // координаты
    int x, y;
    // горит ли объект
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
    
    // параметры спрайта
    virtual int GetSprX() = 0;
    virtual int GetSprY() = 0;
    virtual int GetSprW() = 0;
    virtual int GetSprH() = 0;
    
    virtual ~CSprite() {};
    
    // тип объекта
    virtual sprType GetType() = 0;

    // поведение объекта в очередном цикле
    virtual void Move() {};
    // копирование спрайта
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // пересекается ли объект с прямоугольником
    virtual bool Intersects(LPRECT r, LPRECT dr = NULL);
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return false; }
    // уничтожить объект
    virtual void Destroy() {}
    // потушить огонь
    virtual void Extinguish();
    // остановить звук, связанный с объектом
    virtual void StopSound() {}
    // возобновить звук
    virtual void ResumeSound() {}
};



// перекрытия между этажами
class CFloor : public CSprite
{
public:
    // мип перекрытия
    enum {EMPTY, FLOOR, EXPLODED, LADDER} type;
    // насколько объект разрушен (0xf - целый, 0 - разрушен)
    int hp;
    // координаты в массиве
    int offset, floor;
/*
    // конструктор
    CFloor(int xx, int yy);
*/
    // параметры спрайта
    virtual int GetSprX();
    virtual int GetSprY();
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR; };

    // поведение объекта в очередном цикле
    virtual void Move();
    // тип объекта
    virtual sprType GetType() { return CSprite::FLOOR; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return false; };
};



// статический объект
class CStaticObject : public CSprite
{
public:
    // насколько объект разрушен (0xf - целый, 0 - разрушен)
    int hp;
    // координаты в массиве
    int offset, floor;
    // огонь
    int fire[CHR * 2][CHR];
    int fireRandom[256];
    int fireDx[256];
    int iRand;
    int iDx;

    CStaticObject();
    // копирование спрайта
//    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return hp == 0; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // потушить огонь
    virtual void Extinguish();
    // игрок проходит через клетку
    virtual void PassThru() {};
    // обновить картину огня
    void UpdateFire(/*DDSURFACEDESC &ddSurfaceDesc*/);
    // виртуальный деструктор
    virtual ~CStaticObject();
};



// пустой объект
class CNothing : public CStaticObject
{
public:
    // конструктор
    CNothing(int fl, int offs);

    // параметры спрайта
    virtual int GetSprX() { return 208; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return NOTHING; };
};




// лестница
class CLadder : public CStaticObject
{
public:
    // конструктор
    CLadder(int fl, int offs);

    // параметры спрайта
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return LADDER; };
};



// игрок
class CPlayer : public CSprite
{
public:
    // направление движения
    int dx, dy;
    // таймер торможения
    int slower;
    // таймер разгона
    int faster;
    // шаг движения по горизонтали
    int step;
    // объект в руках
    CUsableObject* objInHands;
    // место где был брошен объект
    int droppedX;
    // время до следующего использования предмета
    int useTimeout;
    // время наполнения ведра
    int fillTimeout;
    // время на открытие/закрытие двери
    int doorTimeout;

    // направление последнего движения
    enumDir dir;

    // конструктор
    CPlayer(int xx, int yy);
    // деструктор
    ~CPlayer();

    // параметры спрайта
    virtual int GetSprX();
    virtual int GetSprY();
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // копирование спрайта
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // тип объекта
    virtual sprType GetType() { return PLAYER; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return burning; };
    // уничтожить объект
    virtual void Destroy() {};
};




// объект, который можно брать 
class CPickupableObject : public CSprite
{
public:
    // сгорел ли объект
//    bool burned;
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return burning; };
};



// объект, который можно брать и использовать
class CUsableObject : public CPickupableObject
{
public:
    // ориентация в пространстве
    enumDir dir;
    // использовать объект - возвращает true, если использование было успешным
    virtual bool Use(int xx, int yy, enumDir dir) { return false; };
    /*
    // поведение объекта в очередном цикле
    virtual void Move();
    */
    // возвращает useTimeout
    virtual int GetUseTimeout() { return 0; };
    /*
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return false; };
    */
};



// объект, используемый для тушения пожара
class CExtinguishingObject : public CUsableObject
{
public:
    // количество воды (пены) в объекте
    int water;

    // дальность действия
    virtual int HowFar() = 0;
    // использовать объект - возвращает true, если использование было успешным
    virtual bool Use(int xx, int yy, enumDir d);
};



// ведро
class CBucket : public CExtinguishingObject
{
public:
    // конструктор
    CBucket(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return dir == RIGHT ? 256 : 274; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 18; };
    virtual int GetSprH() { return 20; };

    // тип объекта
    virtual sprType GetType() { return BUCKET; };
    
    // использовать объект - возвращает true, если использование было успешным
    virtual bool Use(int xx, int yy, enumDir d);
    // возвращает useTimeout
    virtual int GetUseTimeout() { return 6 * 4; };
    // дальность действия
    virtual int HowFar() { return 4; };
};




// вылитая вода
class CWater : public CSprite
{
public:
    // время жизни
    int timeout;
    // конструктор
    CWater(int xx, int yy, enumDir dir);

    // параметры спрайта
    virtual int GetSprX() { return 292; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 16; };

    // тип объекта
    virtual sprType GetType() { return WATER; };
    // поведение объекта в очередном цикле
    virtual void Move() { if (timeout) timeout--; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return !timeout; };
};



// кран
class CTap : public CStaticObject
{
public:
    // конструктор
    CTap(int fl, int offs);

    // параметры спрайта
    virtual int GetSprX() { return 324; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return TAP; };
};




// огнетушитель
class CExtinguisher : public CExtinguishingObject
{
public:
    // конструктор
    CExtinguisher(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return dir == LEFT ? 340 : 360; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 20; };
    virtual int GetSprH() { return 24; };

    // тип объекта
    virtual sprType GetType() { return EXTINGUISHER; };
    
    // использовать объект - возвращает true, если использование было успешным
    virtual bool Use(int xx, int yy, enumDir d);
    // возвращает useTimeout
    virtual int GetUseTimeout() { return 7 * 4; };
    // дальность действия
    virtual int HowFar() { return 15; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return !water || burning; };
};



// пена из огнетушителя
class CFoam : public CSprite
{
public:
    // направление
    enumDir dir;
    // время жизни
    int timeout;
    // конструктор
    CFoam(int xx, int yy, enumDir d);

    // параметры спрайта
    virtual int GetSprX() { return dir == RIGHT ? 380 : 412; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 24; };

    // тип объекта
    virtual sprType GetType() { return FOAM; };
    // поведение объекта в очередном цикле
    virtual void Move() { if (timeout) timeout--; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return !timeout; };
};



// дым
class CSmoke : public CSprite
{
public:
    // время жизни
    int timeout;
    // конструктор
    CSmoke(int xx, int yy) { x = xx; y = yy; timeout = 7 * 4; };

    // параметры спрайта
    virtual int GetSprX() { return 128 + (7 - timeout / 4) * CHR; };
    virtual int GetSprY() { return 352; };
    virtual int GetSprW() { return 16; };
    virtual int GetSprH() { return 32; };

    // тип объекта
    virtual sprType GetType() { return SMOKE; };
    // поведение объекта в очередном цикле
    virtual void Move() { if (timeout) timeout--; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return !timeout; };
};



// дополнительная лестница
class CLadderKit : public CUsableObject
{
public:
    // использована ли лестница
    bool used;
    // конструктор
    CLadderKit(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return 444; };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 18; };

    // тип объекта
    virtual sprType GetType() { return LADDERKIT; };
    
    // использовать объект - возвращает true, если использование было успешным
    virtual bool Use(int xx, int yy, enumDir d);
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return used || burning; };
};




// приз
class CBonusObject : public CPickupableObject
{
public:
    // тип объекта
    int type;
    // объект собран
    bool picked;
    // конструктор
    CBonusObject(int xx, int yy, int t);

    // параметры спрайта
    virtual int GetSprX() { return 256 + type * 32; };
    virtual int GetSprY() { return 352; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 24; };

    // тип объекта
    virtual sprType GetType() { return BONUS; };
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return burning || picked; };
    // игрок взял предмет
    virtual void PickUp();
};



// дверь
class CDoor : public CStaticObject
{
public:
    // открыта ли дверь
    bool open;
    // конструктор
    CDoor(int fl, int offs, bool op);

    // параметры спрайта
    virtual int GetSprX() { return 476 + (open ? 0 : 16); };
    virtual int GetSprY() { return 0; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return open ? OPENDOOR : CLOSEDDOOR; };
    // игрок проходит через клетку
    virtual void PassThru() { if (!open) PlayDublicateSound(SND_DOOR); open = true; };
    // открыть или закрыть дверь
    void Toggle() { open = !open; PlayDublicateSound(SND_DOOR); };
};



// ангел
class CAngel : public CSprite
{
public:
    // скорость
    int dy;
    // таймер разгона
    int faster;
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;
    int maxStepTimeout;
    // время нахождения на верхнем этаже
    int deathTimeout;
    // звук
    LPDIRECTSOUNDBUFFER sound;

    // конструктор
    CAngel(int xx, int yy);
    // деструктор
    virtual ~CAngel();

    // параметры спрайта
    virtual int GetSprX() { return 320 + 32 * step; };
    virtual int GetSprY() { return 320; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return ANGEL; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // остановить звук, связанный с объектом
    virtual void StopSound() { if (sound) sound->Stop(); }
    // возобновить звук
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, 0); }
};



// динамит
class CDynamite : public CUsableObject
{
public:
    // конструктор
    CDynamite(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return (dir == RIGHT) ? 496 : 515; };
    virtual int GetSprW() { return 32; };
    virtual int GetSprH() { return 19; };

    // тип объекта
    virtual sprType GetType() { return DYNAMITE; };
    // поведение объекта в очередном цикле
    virtual void Move();
};



// взрыв
class CExplosion : public CSprite
{
public:
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;

    // конструктор
    CExplosion(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return 64 * step; };
    virtual int GetSprY() { return 890; };
    virtual int GetSprW() { return CHR * 4; };
    virtual int GetSprH() { return CHR * 4; };

    // тип объекта
    virtual sprType GetType() { return EXPLOSION; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // уничтожить объект
    virtual void Destroy();
};



// пустой объект
class CExplodedNothing : public CStaticObject
{
public:
    // конструктор
    CExplodedNothing(int fl, int offs);

    // параметры спрайта
    virtual int GetSprX() { return 208; };
    virtual int GetSprY() { return 544; };
    virtual int GetSprW() { return CHR; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return EXPLODEDNOTHING; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return false; };
};




// падающий пожарный
class CFallingPlayer : public CSprite
{
public:
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;

    // конструктор
    CFallingPlayer(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return 448; };
    virtual int GetSprY() { return 288 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return FALLINGPLAYER; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // уничтожить объект
    virtual void Destroy();
};



// слон
class CElephant : public CExtinguishingObject
{
public:
    // создается, когда истек timeout
    static int createTimeout;
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;
    // звук
    LPDIRECTSOUNDBUFFER sound;

    // конструктор
    CElephant(int xx, int yy);
    // деструктор
    ~CElephant();

    // параметры спрайта
    virtual int GetSprX() { return 376; };
    virtual int GetSprY() { return 640 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return ELEPHANT; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // дальность действия
    virtual int HowFar() { return 7; };
    // остановить звук, связанный с объектом
    virtual void StopSound() { if (sound) sound->Stop(); }
    // возобновить звук
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, DSBPLAY_LOOPING); }
};


const int MAN_NUM = 2;
const int MAN_X[MAN_NUM] = {272, 0};
const int MAN_Y[MAN_NUM] = {32, 960};

// человек
class CMan : public CSprite
{
public:
    // такт движения
    int step;
    /*
    // изменение спрайта
    int stepTimeout;
    */
    // направление движения
    int dx, dy;
    // счетчик (паники?)
    int cnt;
    // таймер изменения скорости
    int fasterTimeout;
    // направление движения
    enumDir dir;
    // тип человека
    int num;

    // конструктор
    CMan(int xx, int yy, int n);

    // параметры спрайта
    virtual int GetSprX() { return 336; };
    virtual int GetSprY() { return 32; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return MAN; };
    // копирование спрайта
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return burning || x <= 0; };
    // уничтожить объект
    virtual void Destroy();
};



// падающий человек
class CFallingMan : public CSprite
{
public:
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;
    // тип человека
    int num;

    // конструктор
    CFallingMan(int xx, int yy, int n);

    // параметры спрайта
    virtual int GetSprX() { return MAN_X[num] + 64; };
    virtual int GetSprY() { return MAN_Y[num] + 149 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return FALLINGMAN; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // уничтожить объект
    virtual void Destroy();
};


// горящий человек
class CBurningMan : public CSprite
{
public:
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;
    // тип человека
    int num;

    // конструктор
    CBurningMan(int xx, int yy, int n);

    // параметры спрайта
    virtual int GetSprX() { return MAN_X[num] + 96; };
    virtual int GetSprY() { return MAN_Y[num] + 149 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return BURNINGMAN; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // уничтожить объект
    virtual void Destroy();
};



// поджигатель
class CEnemy : public CSprite
{
public:
    // такт движения
    int step;
    // направление движения
    int dx, dy;
    // таймер изменения скорости
    int fasterTimeout;
    // направление движения
    enumDir dir;
    // таймер поджигания
    int useTimeout;
    // потушен
    bool extinguished;

    // конструктор
    CEnemy(int xx, int yy);

    // параметры спрайта
    virtual int GetSprX() { return 128; };
    virtual int GetSprY() { return 960; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return ENEMY; };
    // копирование спрайта
    virtual void CopyToSurface(LPDIRECTDRAWSURFACE pSurf);
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed() { return burning || extinguished; };
    // уничтожить объект
    virtual void Destroy();
    // потушить
    virtual void Extinguish();
};



// падающий поджигатель
class CFallingEnemy : public CSprite
{
public:
    // такт движения
    int step;
    // изменение спрайта
    int stepTimeout;
    // звук
    LPDIRECTSOUNDBUFFER sound;

    // конструктор
    CFallingEnemy(int xx, int yy);
    // деструктор
    virtual ~CFallingEnemy();

    // параметры спрайта
    virtual int GetSprX() { return 192; };
    virtual int GetSprY() { return 1109 + 32 * step; };
    virtual int GetSprW() { return CHR * 2; };
    virtual int GetSprH() { return CHR * 2; };

    // тип объекта
    virtual sprType GetType() { return FALLINGENEMY; };
    // поведение объекта в очередном цикле
    virtual void Move();
    // должен ли объект быть уничтожен
    virtual bool ToBeDestroyed();
    // уничтожить объект
    virtual void Destroy();
    // остановить звук, связанный с объектом
    virtual void StopSound() { if (sound) sound->Stop(); }
    // возобновить звук
    virtual void ResumeSound() { if (sound) sound->Play(0, 0, 0); }
};


#endif
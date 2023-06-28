#ifndef CAPTAINFIZZ_SOUNDS
#define CAPTAINFIZZ_SOUNDS

#include "cwave.h"
#include <dsound.h>



// идентификаторы звуков
extern int SND_FIRE;
extern int SND_DYNAMITE;
extern int SND_ENEMY;
extern int SND_ANGEL;
extern int SND_ELEPHANT;
extern int SND_DROP;
extern int SND_DROPLADDER;
extern int SND_DROPBUCKET;
extern int SND_DROPEXTINGUISHER;
extern int SND_PICKUPLADDER;
extern int SND_PICKUP;
extern int SND_MANDIE[];
extern int SND_EXTINGUISHER;
extern int SND_WIN;
extern int SND_BUCKET;
extern int SND_STEAM;
extern int SND_ENEMYDIE;
extern int SND_WATER;
extern int SND_DOOR;



BOOL InitSounds();
BOOL DeInitSounds();
void TickSound();
LPDIRECTSOUNDBUFFER PlayDublicateSound(int buf);
LPDIRECTSOUNDBUFFER PlayInfiniteSound(int buf);

// остановить все звуки
void StopSounds();



#endif
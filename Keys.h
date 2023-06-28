#ifndef KEYS_H
#define KEYS_H

#include <windows.h>

extern int keyLeft;
extern int keyRight;
extern int keyUp;
extern int keyDown;
extern int keyFire;
extern int keyDrop;
extern int keyDoor;
extern int keyPause;
extern int keyBoss;

// инициализирует процесс выбора управления
void EnterKeys();
// рисует экран выбора управления
void DrawKeys();
// инициализирует переменные
void CreateKeys();
// освободить память
void ReleaseKeys();
// обработка сообщений
bool KeysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// войти в состояние
void EnterKeys();
// такт работы
void TickKeys();


#endif
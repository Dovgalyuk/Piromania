#ifndef BOSS_H
#define BOSS_H

#include <windows.h>

// нарисовать
void DrawBoss();
// обработка сообщений
bool BossMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// такт
void BossTick();


#endif
#ifndef BOSS_H
#define BOSS_H

#include <windows.h>

// ����������
void DrawBoss();
// ��������� ���������
bool BossMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// ����
void BossTick();


#endif
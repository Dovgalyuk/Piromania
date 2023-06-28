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

// �������������� ������� ������ ����������
void EnterKeys();
// ������ ����� ������ ����������
void DrawKeys();
// �������������� ����������
void CreateKeys();
// ���������� ������
void ReleaseKeys();
// ��������� ���������
bool KeysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// ����� � ���������
void EnterKeys();
// ���� ������
void TickKeys();


#endif
#ifndef MENU_H
#define MENU_H

#include <windows.h>

// ���������� ������� ����
void DrawMainMenu();
// ���������������� ����������
void CreateMainMenu();
// ���������� ������
void ReleaseMainMenu();
// ��������� ���������
bool MainMenuMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif
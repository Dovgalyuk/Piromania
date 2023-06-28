#ifndef SCORES_H
#define SCORES_H

#include <windows.h>

extern int scores[];



// ������������� �������� ����� �����
void EnterName();
// ���������� ������������
void EnterNameTick();
// ���������� ������� ����� �����
void DrawEnterName();
// ��������� ���������
bool EnterNameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

// ��������� ������� ��������
void CreateScores();
// ��������� ������� ��������
void ReleaseScores();
// ���������� ������� ��������
void DrawScores();
// ��������� ���������
bool ScoresMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
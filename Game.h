#ifndef GAME_H
#define GAME_H

#include "Piromania.h"
#include "Objects.h"
#include <windows.h>
#include <list>
using namespace std;


// ���������� �����
const int MAPS_NUM = 8;
// ���������� ������
const int FLOORS_NUM = 7;
// ������ ������� ������
const int FLOORS_X = 5;
// ������ ������� ������
const int FLOORS_WIDTH = CHARS_NUM - FLOORS_X;
// ������ �����
//const int FLOORS_Y[FLOORS_NUM] = {23 * CHR, 20 * CHR, 17 * CHR, 14 * CHR, 11 * CHR, 8 * CHR, 5 * CHR};
const int FLOORS_Y[FLOORS_NUM] = {5 * CHR, 8 * CHR, 11 * CHR, 14 * CHR, 17 * CHR, 20 * CHR, 23 * CHR};


// ������� �������
extern int level;
// ������� ����
extern int score;
// ���������� �������� �� ������ �����
extern int deadMen;
// ����
extern CFloor floors[FLOORS_NUM - 1][FLOORS_WIDTH];
// ����������� �������
extern CStaticObject* staticObjects[FLOORS_NUM][CHARS_NUM];
// ������������ �������
typedef list<CSprite*> TObjList;
extern TObjList dynamicObjects;
// ������� �������
extern bool won;



// ���������� ������� ��������
void DrawGame();
// ���������������� ����������
void CreateGame();
// ���������� ������
void ReleaseGame();
// ��������� ���������
bool GameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// ����� � ���������
void EnterGame();
// ����� �� ���������
void LeaveGame();
// ���� ����
void TickGame();


void ResumeGameSounds();

#endif
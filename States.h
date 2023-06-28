#ifndef STATES_H
#define STATES_H

#include <windows.h>
#include <vector>

using namespace std;

typedef void (*TickMethod)();
typedef void (*DrawMethod)();
typedef bool (*MessageMethod)(UINT, WPARAM, LPARAM); // returns true if processed message
typedef void (*CreateMethod)();
typedef void (*ReleaseMethod)();
typedef void (*EnterMethod)();
typedef void (*LeaveMethod)();


struct State
{
    TickMethod fTick;
    DrawMethod fDraw;
    MessageMethod fMessage;
    CreateMethod fCreate;
    ReleaseMethod fRelease;
    EnterMethod fEnter;
    LeaveMethod fLeave;
};

extern vector<State> states;

// ������� ���������
extern int ST_MENU;
extern int ST_KEYS;
extern int ST_GAME;
extern int ST_SCORES;
extern int ST_ENTERNAME;
extern int ST_GAMEOVER;
extern int ST_WON;
extern int ST_BOSS;


// ������� ���������
extern int currentState;

// �������� ��� ���������
void InitStates();
// ���������� ������
void DeInitStates();
// ����
void TickState();
// ���������� ������� ���������
void DrawState();
// ���������� ������� ���������
bool MessageState(UINT uMsg, WPARAM wParam, LPARAM lParam);
// ������� ������� ���������
void EnterState(int state);
// ������� ������� ��������� ��� ������ ������������ �������
void SafeEnterState(int state);



#endif
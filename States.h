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

// индексы состояний
extern int ST_MENU;
extern int ST_KEYS;
extern int ST_GAME;
extern int ST_SCORES;
extern int ST_ENTERNAME;
extern int ST_GAMEOVER;
extern int ST_WON;
extern int ST_BOSS;


// текущее состояние
extern int currentState;

// добавить все состояния
void InitStates();
// освободить память
void DeInitStates();
// такт
void TickState();
// отобразить текущее состояние
void DrawState();
// обработчик оконных сообщений
bool MessageState(UINT uMsg, WPARAM wParam, LPARAM lParam);
// сменить текущее состояние
void EnterState(int state);
// сменить текущее состояние без вызова обработчиков событий
void SafeEnterState(int state);



#endif
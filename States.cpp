#include "States.h"
#include "Menu.h"
#include "Game.h"
#include "Keys.h"
#include "Scores.h"
#include "GameEnds.h"
#include "Sounds.h"
#include "Boss.h"

// массив состояний
vector<State> states;

// индексы состояний
int ST_MENU = -1;
int ST_KEYS = -1;
int ST_GAME = -1;
int ST_SCORES = -1;
int ST_ENTERNAME = -1;
int ST_GAMEOVER = -1;
int ST_WON = -1;
int ST_BOSS = -1;

// текущее состояние
int currentState = -1;

// методы по умолчанию
void DefaultVoid() {}
bool DefaultMessage(UINT, WPARAM, LPARAM) { return false; }


// добавить состояние
int AddState(TickMethod fTick,
             DrawMethod fDraw,
             MessageMethod fMessage,
             CreateMethod fCreate,
             ReleaseMethod fRelease,
             EnterMethod fEnter,
             LeaveMethod fLeave)
{
    State s;

    s.fCreate = fCreate;
    s.fDraw = fDraw;
    s.fEnter = fEnter;
    s.fLeave = fLeave;
    s.fMessage = fMessage;
    s.fRelease = fRelease;
    s.fTick = fTick;

    states.push_back(s);

    return states.size() - 1;
}


// добавить все состояния
void InitStates()
{
    ST_MENU = AddState(DefaultVoid, DrawMainMenu, MainMenuMessage,
        CreateMainMenu, ReleaseMainMenu, DefaultVoid, DefaultVoid);
    ST_GAME = AddState(TickGame, DrawGame, GameMessage,
        CreateGame, ReleaseGame, EnterGame, LeaveGame);
    ST_KEYS = AddState(TickKeys, DrawKeys, KeysMessage,
        CreateKeys, ReleaseKeys, EnterKeys, DefaultVoid);
    ST_SCORES = AddState(DefaultVoid, DrawScores, ScoresMessage,
        CreateScores, ReleaseScores, DefaultVoid, DefaultVoid);
    ST_ENTERNAME = AddState(EnterNameTick, DrawEnterName, EnterNameMessage,
        DefaultVoid, DefaultVoid, EnterName, DefaultVoid);
    ST_GAMEOVER = AddState(GameEndTick, DrawGameEnd, DefaultMessage,
        DefaultVoid, DefaultVoid, InitGameOver, DefaultVoid);
    ST_WON = AddState(GameEndTick, DrawGameEnd, DefaultMessage,
        DefaultVoid, DefaultVoid, InitGameWon, DefaultVoid);
    ST_BOSS = AddState(BossTick, DrawBoss, BossMessage,
        DefaultVoid, DefaultVoid, DefaultVoid, DefaultVoid);

    for (int i = 0 ; i < states.size() ; i++)
        states[i].fCreate();
}


// освободить память
void DeInitStates()
{
    for (int i = 0 ; i < states.size() ; i++)
        states[i].fRelease();
}

// такт
void TickState()
{
    states[currentState].fTick();
}

// отобразить текущее состояние
void DrawState()
{
    states[currentState].fDraw();
}


// обработчик оконных сообщений
bool MessageState(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (currentState >= 0)
        return states[currentState].fMessage(uMsg, wParam, lParam);
    else
        return false;
}


// сменить текущее состояние
void EnterState(int state)
{
    StopSounds();

    if (currentState >= 0)
        states[currentState].fLeave();

    currentState = state;

    if (currentState >= 0)
        states[currentState].fEnter();
}


// сменить текущее состояние без вызова обработчиков событий
void SafeEnterState(int state)
{
    StopSounds();

    currentState = state;
}


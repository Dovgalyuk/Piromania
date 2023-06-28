#ifndef GAME_H
#define GAME_H

#include "Piromania.h"
#include "Objects.h"
#include <windows.h>
#include <list>
using namespace std;


// количество домов
const int MAPS_NUM = 8;
// количество этажей
const int FLOORS_NUM = 7;
// начало верхних этажей
const int FLOORS_X = 5;
// ширина верхних этажей
const int FLOORS_WIDTH = CHARS_NUM - FLOORS_X;
// уровни полов
//const int FLOORS_Y[FLOORS_NUM] = {23 * CHR, 20 * CHR, 17 * CHR, 14 * CHR, 11 * CHR, 8 * CHR, 5 * CHR};
const int FLOORS_Y[FLOORS_NUM] = {5 * CHR, 8 * CHR, 11 * CHR, 14 * CHR, 17 * CHR, 20 * CHR, 23 * CHR};


// текущий уровень
extern int level;
// текущий счет
extern int score;
// количество погибших на уровне людей
extern int deadMen;
// полы
extern CFloor floors[FLOORS_NUM - 1][FLOORS_WIDTH];
// статические объекты
extern CStaticObject* staticObjects[FLOORS_NUM][CHARS_NUM];
// динамические объекты
typedef list<CSprite*> TObjList;
extern TObjList dynamicObjects;
// уровень пройден
extern bool won;



// нарисовать игровую ситуацию
void DrawGame();
// инициализировать переменные
void CreateGame();
// освободить память
void ReleaseGame();
// обработка сообщений
bool GameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
// войти в состояние
void EnterGame();
// выйти из состояния
void LeaveGame();
// такт игры
void TickGame();


void ResumeGameSounds();

#endif
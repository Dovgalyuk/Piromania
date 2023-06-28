#ifndef GAMEENDS_H
#define GAMEENDS_H

#include <windows.h>


// конец игры
void InitGameOver();
// пройдены все уровни
void InitGameWon();
// нарисовать 
void DrawGameEnd();
// tick
void GameEndTick();

#endif
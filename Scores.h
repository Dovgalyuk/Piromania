#ifndef SCORES_H
#define SCORES_H

#include <windows.h>

extern int scores[];



// инициализация процесса ввода имени
void EnterName();
// вызывается периодически
void EnterNameTick();
// нарисовать процесс ввода имени
void DrawEnterName();
// обработка сообщений
bool EnterNameMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

// загрузить таблицу рекордов
void CreateScores();
// сохранить таблицу рекордов
void ReleaseScores();
// нарисовать таблицу рекордов
void DrawScores();
// обработка сообщений
bool ScoresMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif